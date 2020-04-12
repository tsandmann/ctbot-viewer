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
 * @file    main.cpp
 * @brief   c't-Bot remote viewer tool
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlProperty>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickItem>
#include <QtNetwork>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <memory>
#include <cstring>

#include "sensor_viewer.h"
#include "actuator_viewer.h"
#include "remotecall_viewer.h"
#include "log_viewer.h"
#include "map_viewer.h"
#include "script_editor.h"
#include "command_evaluator.h"

#include "command.h"
#include "connect_button.h"


int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app { argc, argv };
    QQmlApplicationEngine engine;

    CommandEvaluator command_eval_;
    QTcpSocket socket;

    SensorViewer sensor_viewer { &engine, command_eval_ };
    ActuatorViewer actuator_viewer { &engine, command_eval_ };
    RemotecallViewer remotecall_viewer { &engine, command_eval_, &socket };
    LogViewer log_viewer { &engine, command_eval_ };
    MapViewer map_viewer { &engine, command_eval_, &socket };
    ScriptEditor script_editor { &engine, &socket };


    const QUrl url { QStringLiteral("qrc:/Main.qml") };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    engine.load(url);

    remotecall_viewer.register_buttons();
    map_viewer.register_buttons();
    script_editor.register_buttons();


    command_eval_.register_cmd(ctbot::CommandCodes::CMD_WELCOME, [](const ctbot::CommandBase&) {
        // std::cout << "CMD_WELCOME received: " << cmd << "\n";
        return true;
    });

    command_eval_.register_cmd(ctbot::CommandCodes::CMD_DONE, [](const ctbot::CommandBase&) {
        // std::cout << "CMD_DONE received: " << cmd << "\n";
        return true;
    });

    command_eval_.register_cmd(ctbot::CommandCodes::CMD_SHUTDOWN, [&](const ctbot::CommandBase&) {
        // std::cout << "CMD_SHUTDOWN received: " << cmd << "\n";
        auto object { engine.rootObjects().at(0)->findChild<QObject*>("Hostname") };
        socket.close();
        QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, ""));
        return true;
    });



    bool connected { false };

    ConnectButton button { [&](QString hostname, QString port) {
        auto object { engine.rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (!connected) {
            socket.connectToHost(hostname, static_cast<quint16>(port.toUInt()));
        } else {
            socket.close();
            QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, hostname));
        }
    } };
    QObject::connect(
        engine.rootObjects().at(0)->findChild<QObject*>("Hostname"), SIGNAL(connectClicked(QString, QString)), &button, SLOT(cppSlot(QString, QString)));


    QByteArray in_buffer;

    QObject::connect(&socket, &QTcpSocket::readyRead, [&]() {
        if (socket.bytesAvailable()) {
            in_buffer.append(socket.readAll());

            while (in_buffer.size() > static_cast<int>(sizeof(ctbot::CommandData))) {
                std::unique_ptr<ctbot::CommandNoCRC> p_cmd;
                try {
                    p_cmd = std::make_unique<ctbot::CommandNoCRC>(in_buffer);
                } catch (const std::runtime_error& e) {
                    qDebug() << "invalid command received: " << e.what();
                    return;
                }

                if (p_cmd->get_payload_size()) {
                    const int len { static_cast<int>(p_cmd->get_payload_size()) };
                    size_t n {};
                    while (in_buffer.size() < len) { // FIXME: improve?
                        QCoreApplication::processEvents();
                        if (socket.bytesAvailable()) {
                            in_buffer.append(socket.readAll());
                        }
                        if (in_buffer.size() < len) {
                            if (++n > 100) {
                                break;
                            }
                            QThread::msleep(1);
                        }
                    }
                    if (in_buffer.size() < len) {
                        continue;
                    }

                    if (!p_cmd->append_payload(in_buffer, p_cmd->get_payload_size())) {
                        qDebug() << "could not receive payload of cmd:";
                        std::cout << *p_cmd << std::endl;
                        return;
                    }
                }

                command_eval_.evaluate(p_cmd.get());
            }
        }
    });

    QObject::connect(&socket, &QTcpSocket::connected, [&]() {
        socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
        qDebug() << "Connected to " << socket.peerName() << ":" << socket.peerPort();
        auto object { engine.rootObjects().at(0)->findChild<QObject*>("Hostname") };
        QMetaObject::invokeMethod(object, "connected", Q_ARG(QVariant, socket.peerName()));
        connected = true;
    });

    QObject::connect(&socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Connection closed.";
        auto object { engine.rootObjects().at(0)->findChild<QObject*>("Hostname") };
        QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, ""));
        connected = false;
    });

    QObject::connect(&socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [&](QAbstractSocket::SocketError socketError) {
        qDebug() << "Connection to " << socket.peerName() << " failed:" << socketError;
        auto object { engine.rootObjects().at(0)->findChild<QObject*>("Hostname") };
        QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, socket.peerName()));
        connected = false;
    });


    std::map<QString /*RC-Code*/, uint16_t> rc5_codes { { "Power", 0x118c }, { "1", 0x1181 }, { "2", 0x1182 }, { "3", 0x1183 }, { "4", 0x1184 },
        { "5", 0x1185 }, { "6", 0x1186 }, { "7", 0x1187 }, { "8", 0x1188 }, { "9", 0x1189 }, { "10", 0x1180 }, { "11", 0x118a }, { "12", 0x11a3 },
        { "Gr", 0x01ba }, { "Re", 0x01bd }, { "Ye", 0x01b1 }, { "Bl", 0x01b0 }, { "I/II", 0x11ab }, { "TV", 0x11b8 }, { "Gr", 0x01ba }, { "||", 0x11a9 },
        { "<<", 0x11b2 }, { "\u25B6", 0x11b5 }, { ">>", 0x11b4 }, { "\u25A0", 0x11b6 }, { "\u25CF", 0x11ab }, { "CH*P/C", 0x11bf }, { "V+", 0x1190 }, { "M", 0x01bf },
        { "C+", 0x11a0 }, { "V-", 0x1191 }, { "C-", 0x11a1 } };

    ConnectButton rc_button { [&](QString button, QString) {
        const auto it { rc5_codes.find(button) };
        if (it == rc5_codes.end()) {
            qDebug() << "unknown RC button pressed.";
            return;
        }

        const auto rc5code { static_cast<int16_t>(it->second) };
        // qDebug() << "button " << button << "pressed, code =" << rc5code;

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_SENS_RC5, ctbot::CommandCodes::CMD_SUB_NORM, rc5code, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };

        if (socket.isOpen()) {
            qint64 sent { socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData)) };
            if (cmd.get_payload_size()) {
                sent += socket.write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            }
            // qDebug() << "sent" << sent << "bytes.";
        }
    } };
    QObject::connect(
        engine.rootObjects().first()->findChild<QObject*>("RCButton"), SIGNAL(rcButtonClicked(QString, QString)), &rc_button, SLOT(cppSlot(QString, QString)));


    return app.exec();
}
