/*
 * This file is part of the c't-Bot remote viewer tool.
 * Copyright (c) 2020-2022 Timo Sandmann
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
#include <QQmlProperty>
#include <QThread>
#include <QTimer>
#include <QDebug>

#include <iostream>
#include <regex>

#include "connection_manager.h"


ConnectionManagerBase::ConnectionManagerBase(QQmlApplicationEngine* p_engine)
    : p_connect_button_ {}, p_shutdown_button_ {}, p_engine_ { p_engine }, connected_ {} {
    QObject::connect(&socket_, &QTcpSocket::readyRead, p_engine_, [this]() {
        if (socket_.bytesAvailable()) {
            // qDebug() << "socket_.bytesAvailable()=" << socket_.bytesAvailable();
            in_buffer_.append(socket_.readAll());

            process_incoming();
        }
    });

    QObject::connect(&socket_, &QTcpSocket::connected, p_engine_, [this]() {
        socket_.setSocketOption(QAbstractSocket::LowDelayOption, 1);
        qDebug() << "ConnectionManagerBase: Connected to " << socket_.peerName() << ":" << socket_.peerPort();
        auto p_hostname { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (p_hostname) {
            QMetaObject::invokeMethod(p_hostname, "connected", Q_ARG(QVariant, socket_.peerName()));
        }
        connected_ = true;

        connected_hook();
    });

    QObject::connect(&socket_, &QTcpSocket::disconnected, p_engine_, [this]() {
        qDebug() << "ConnectionManagerBase: Connection closed.";
        auto p_hostname { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (p_hostname) {
            QMetaObject::invokeMethod(p_hostname, "disconnected", Q_ARG(QVariant, ""));
        }

        connected_ = false;
    });

    QObject::connect(&socket_, &QAbstractSocket::errorOccurred, p_engine_, [this](QAbstractSocket::SocketError socketError) {
        qDebug() << "ConnectionManagerBase: Connection to " << socket_.peerName() << " failed:" << socketError;
        auto p_hostname { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (p_hostname) {
            QMetaObject::invokeMethod(p_hostname, "disconnected", Q_ARG(QVariant, socket_.peerName()));
        }
        connected_ = false;
    });
}

ConnectionManagerBase::~ConnectionManagerBase() {
    delete p_shutdown_button_;
    delete p_connect_button_;
}

int ConnectionManagerBase::version_active() const {
    const auto version { QQmlProperty::read(p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname"), "version").toInt() };
    // qDebug() << "ConnectionManagerBase::version_active(): version set in GUI is " << version;
    return version;
}

void ConnectionManagerBase::register_buttons() {
    p_connect_button_ = new ConnectButton { [this](QString hostname, QString port) {
        if (get_version() != version_active()) {
            return;
        }

        auto object { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (!connected_) {
            socket_.connectToHost(hostname, static_cast<quint16>(port.toUInt()));
        } else {
            disconnected_hook();
            socket_.flush();
            socket_.close();
            QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, hostname));
        }
    } };
    QObject::connect(p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname"), SIGNAL(connectClicked(QString, QString)), p_connect_button_,
        SLOT(cppSlot(QString, QString)));
}


ConnectionManagerV1::ConnectionManagerV1(QQmlApplicationEngine* p_engine) : ConnectionManagerBase { p_engine } {
    register_cmd(ctbot::CommandCodes::CMD_WELCOME, [](const ctbot::CommandBase&) {
        // std::cout << "CMD_WELCOME received: " << cmd << "\n";
        return true;
    });

    register_cmd(ctbot::CommandCodes::CMD_DONE, [](const ctbot::CommandBase&) {
        // std::cout << "CMD_DONE received: " << cmd << "\n";
        return true;
    });

    register_cmd(ctbot::CommandCodes::CMD_SHUTDOWN, [this](const ctbot::CommandBase&) {
        // std::cout << "CMD_SHUTDOWN received: " << cmd << "\n";
        auto p_hostname { p_engine_->rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (p_hostname) {
            QMetaObject::invokeMethod(p_hostname, "disconnected", Q_ARG(QVariant, ""));
        }

        socket_.close();
        connected_ = false;

        return true;
    });
}

ConnectionManagerV1::~ConnectionManagerV1() {}

int ConnectionManagerV1::get_version() const {
    return 1;
}

void ConnectionManagerV1::register_buttons() {
    ConnectionManagerBase::register_buttons();

    p_shutdown_button_ = new ConnectButton { [this](QString, QString) {
        if (get_version() != version_active()) {
            return;
        }

        if (!socket_.isOpen()) {
            return;
        }

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_SHUTDOWN, ctbot::CommandCodes::CMD_SUB_NORM, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        socket_.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));

        qDebug() << "Shutdown requested.";

        socket_.flush();
        socket_.close();
        connected_ = false;
    } };
    QObject::connect(p_engine_->rootObjects().at(0)->findChild<QObject*>("ShutdownButton"), SIGNAL(shutdownClicked()), p_shutdown_button_, SLOT(cppSlot()));
}

void ConnectionManagerV1::register_cmd(const ctbot::CommandCodes& cmd, std::function<bool(const ctbot::CommandBase&)>&& func) {
    commands_[cmd].emplace_back(func);
}

bool ConnectionManagerV1::process_incoming() {
    while (in_buffer_.size() > static_cast<int>(sizeof(ctbot::CommandData))) {
        std::unique_ptr<ctbot::CommandNoCRC> p_cmd;
        try {
            p_cmd = std::make_unique<ctbot::CommandNoCRC>(in_buffer_);
        } catch (const std::runtime_error& e) {
            qDebug() << "ConnectionManagerV1: invalid command received: " << e.what();
            return false;
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
                qDebug() << "ConnectionManagerV1: could not receive payload of cmd:";
                std::cout << *p_cmd << std::endl;
                return false;
            }
        }

        evaluate_cmd(p_cmd.get());
    }

    return true;
}

bool ConnectionManagerV1::evaluate_cmd(const ctbot::CommandNoCRC* p_cmd) const {
    try {
        bool result { true };
        for (auto& func : commands_.at(p_cmd->get_cmd_code())) {
            result &= func(*p_cmd);
        }
        return result;
    } catch (const std::out_of_range&) {
        qDebug() << "ConnectionManagerV1::evaluate_cmd(): CMD code '" << static_cast<char>(p_cmd->get_cmd_code_uint()) << "' not registered:";
        std::cout << *p_cmd << std::endl;
        return false;
    }
}


ConnectionManagerV2::ConnectionManagerV2(QQmlApplicationEngine* p_engine) : ConnectionManagerBase { p_engine } {}

ConnectionManagerV2::~ConnectionManagerV2() {}

void ConnectionManagerV2::register_buttons() {
    ConnectionManagerBase::register_buttons();

    p_shutdown_button_ = new ConnectButton { [this](QString, QString) {
        if (get_version() != version_active()) {
            return;
        }

        if (!socket_.isOpen()) {
            return;
        }

        socket_.write("halt\n", 5);

        qDebug() << "Shutdown requested.";

        socket_.flush();
        socket_.close();
        connected_ = false;
    } };
    QObject::connect(p_engine_->rootObjects().at(0)->findChild<QObject*>("ShutdownButton"), SIGNAL(shutdownClicked()), p_shutdown_button_, SLOT(cppSlot()));
}

void ConnectionManagerV2::register_cmd(const std::string_view& cmd, std::function<bool(const std::string_view&)>&& func) {
    commands_[std::string(cmd)].emplace_back(func);
}

int ConnectionManagerV2::get_version() const {
    return 2;
}

bool ConnectionManagerV2::process_incoming() {
    static const std::regex cmd_regex { R"(<(\w+)>\r\n((?:.*\r\n)+)<(/\1)>\r\n)" };

    bool result { true };

    if (in_buffer_.size()) {
        // qDebug() << "ConnectionManagerV2::process_incoming(): input is: " << in_buffer_;

        std::match_results<std::string_view::const_iterator> matches;
        while (std::regex_search(in_buffer_.begin(), in_buffer_.end(), matches, cmd_regex)) {
            // qDebug() << "ConnectionManagerV2::evaluate_cmd(): Match found:";
            // for (size_t i { 1 }; i < matches.size(); ++i) {
            //     qDebug() << i << ":" << QString::fromStdString(matches[i].str());
            // }

            result &= evaluate_cmd(matches[1].str(), matches[2].str());

            in_buffer_.remove(0, matches[0].length());

            // qDebug() << "ConnectionManagerV2::process_incoming(): next input is: " << in_buffer_;
        }

        if (in_buffer_.size() && !in_buffer_.contains('<')) { // FIXME: double check this
            evaluate_cmd("", std::string_view(in_buffer_.begin(), in_buffer_.size()));
            in_buffer_.clear();
        }
    }

    return result;
}

bool ConnectionManagerV2::evaluate_cmd(const std::string_view& cmd, const std::string_view& data) const {
    // qDebug() << "ConnectionManagerV2::evaluate_cmd(): cmd=" << QString::fromUtf8(cmd.data(), cmd.size())
    //          << "data=" << QString::fromUtf8(data.data(), data.size());

    try {
        bool result { true };
        for (auto& func : commands_.at(std::string(cmd))) {
            result &= func(data);
        }

        return result;
    } catch (const std::out_of_range&) {
        qDebug() << "ConnectionManagerV2::evaluate_cmd(): CMD code " << QString::fromUtf8(cmd.data(), cmd.size()) << " not registered.";
        return false;
    }

    return false;
}

void ConnectionManagerV2::connected_hook() {
    if (get_version() != version_active()) {
        return;
    }

    QTimer::singleShot(100, &socket_, [this]() {
        if (!socket_.isOpen()) {
            return;
        }
     
        socket_.write("c viewer 1\n", 11);
        qDebug() << "ConnectionManagerV2::connected_hook(): viewer config enabled";
    });
}

void ConnectionManagerV2::disconnected_hook() {
    if (get_version() != version_active()) {
        return;
    }

    if (!socket_.isOpen()) {
        return;
    }
    
    socket_.write("c viewer 0\n", 11);
    qDebug() << "ConnectionManagerV2::disconnected_hook(): viewer config disabled";
}
