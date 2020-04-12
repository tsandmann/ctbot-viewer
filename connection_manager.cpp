/*
 * This file is part of the c't-Bot remote viewer tool.
 * Copyright (c) 2020 Timo Sandmann
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file    connection_manager.cpp
 * @brief   Connection and ct-Bot command management
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#include <QObject>
#include <QCoreApplication>
#include <QThread>
#include <QDebug>
#include <iostream>

#include "connection_manager.h"


ConnectionManager::ConnectionManager(QQmlApplicationEngine* p_engine) : p_engine_ { p_engine }, connected_ {}, p_connect_button_ {} {
    QObject::connect(&socket_, &QTcpSocket::readyRead, [this]() {
        if (socket_.bytesAvailable()) {
            in_buffer_.append(socket_.readAll());

            while (in_buffer_.size() > static_cast<int>(sizeof(ctbot::CommandData))) {
                std::unique_ptr<ctbot::CommandNoCRC> p_cmd;
                try {
                    p_cmd = std::make_unique<ctbot::CommandNoCRC>(in_buffer_);
                } catch (const std::runtime_error& e) {
                    qDebug() << "ConnectionManager: invalid command received: " << e.what();
                    return;
                }

                if (p_cmd->get_payload_size()) {
                    const int len { static_cast<int>(p_cmd->get_payload_size()) };
                    size_t n {};
                    while (in_buffer_.size() < len) { // FIXME: improve?
                        QCoreApplication::processEvents(); // FIXME: usefull?
                        if (socket_.bytesAvailable()) {
                            in_buffer_.append(socket_.readAll());
                        }
                        if (in_buffer_.size() < len) {
                            if (++n > 100) {
                                break;
                            }
                            QThread::msleep(1);
                        }
                    }
                    if (in_buffer_.size() < len) {
                        continue;
                    }

                    if (!p_cmd->append_payload(in_buffer_, p_cmd->get_payload_size())) {
                        qDebug() << "ConnectionManager: could not receive payload of cmd:";
                        std::cout << *p_cmd << std::endl;
                        return;
                    }
                }

                evaluate_cmd(p_cmd.get());
            }
        }
    });

    QObject::connect(&socket_, &QTcpSocket::connected, [this]() {
        socket_.setSocketOption(QAbstractSocket::LowDelayOption, 1);
        qDebug() << "ConnectionManager: Connected to " << socket_.peerName() << ":" << socket_.peerPort();
        auto p_hostname { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (p_hostname) {
            QMetaObject::invokeMethod(p_hostname, "connected", Q_ARG(QVariant, socket_.peerName()));
        }
        connected_ = true;
    });

    QObject::connect(&socket_, &QTcpSocket::disconnected, [this]() {
        qDebug() << "ConnectionManager: Connection closed.";
        auto p_hostname { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (p_hostname) {
            QMetaObject::invokeMethod(p_hostname, "disconnected", Q_ARG(QVariant, ""));
        }
        connected_ = false;
    });

    QObject::connect(&socket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [this](QAbstractSocket::SocketError socketError) {
        qDebug() << "ConnectionManager: Connection to " << socket_.peerName() << " failed:" << socketError;
        auto p_hostname { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (p_hostname) {
            QMetaObject::invokeMethod(p_hostname, "disconnected", Q_ARG(QVariant, socket_.peerName()));
        }
        connected_ = false;
    });


    register_cmd(ctbot::CommandCodes::CMD_WELCOME, [](const ctbot::CommandBase&) {
        // std::cout << "CMD_WELCOME received: " << cmd << "\n";
        return true;
    });

    register_cmd(ctbot::CommandCodes::CMD_DONE, [](const ctbot::CommandBase&) {
        // std::cout << "CMD_DONE received: " << cmd << "\n";
        return true;
    });

    register_cmd(ctbot::CommandCodes::CMD_SHUTDOWN, [&](const ctbot::CommandBase&) {
        // std::cout << "CMD_SHUTDOWN received: " << cmd << "\n";
        socket_.close();
        auto p_hostname { p_engine->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (p_hostname) {
            QMetaObject::invokeMethod(p_hostname, "disconnected", Q_ARG(QVariant, ""));
        }
        return true;
    });
}

ConnectionManager::~ConnectionManager() {
    delete p_connect_button_;
}

void ConnectionManager::register_buttons() {
    p_connect_button_ = new ConnectButton { [this](QString hostname, QString port) {
        auto object { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (!connected_) {
            socket_.connectToHost(hostname, static_cast<quint16>(port.toUInt()));
        } else {
            socket_.close();
            QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, hostname));
        }
    } };
    QObject::connect(p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname"), SIGNAL(connectClicked(QString, QString)), p_connect_button_,
        SLOT(cppSlot(QString, QString)));
}

void ConnectionManager::register_cmd(const ctbot::CommandCodes& cmd, std::function<bool(const ctbot::CommandBase&)>&& func) {
    commands_[cmd].emplace_back(func);
}

bool ConnectionManager::evaluate_cmd(const ctbot::CommandNoCRC* p_cmd) const {
    try {
        bool result { true };
        for (auto& func : commands_.at(p_cmd->get_cmd_code())) {
            result &= func(*p_cmd);
        }
        return result;
    } catch (const std::out_of_range&) {
        qDebug() << "ConnectionManager::evaluate_cmd(): CMD code '" << static_cast<char>(p_cmd->get_cmd_code_uint()) << "' not registered:";
        std::cout << *p_cmd << std::endl;
        return false;
    }
}
