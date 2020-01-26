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
#include <QQmlContext>
#include <QtNetwork>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <cstring>

#include "value_model.h"
#include "value_list.h"
#include "command.h"
#include "connect_button.h"


int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<ValueModel>("Sensors", 1, 0, "SensorModel");
    qmlRegisterUncreatableType<ValueList>("Sensors", 1, 0, "ValueList", QStringLiteral("Sensors should not be created in QML"));


    ValueList sensorList;
    sensorList.appendItem(QStringLiteral("Wheel enc left"));
    sensorList.appendItem(QStringLiteral("Wheel enc right"));
    sensorList.appendItem(QStringLiteral("Distance left"));
    sensorList.appendItem(QStringLiteral("Distance right"));
    sensorList.appendItem(QStringLiteral("Line left"));
    sensorList.appendItem(QStringLiteral("Line right"));
    sensorList.appendItem(QStringLiteral("Border left"));
    sensorList.appendItem(QStringLiteral("Border right"));
    sensorList.appendItem(QStringLiteral("Light left"));
    sensorList.appendItem(QStringLiteral("Light right"));
    sensorList.appendItem(QStringLiteral("Mouse dX"));
    sensorList.appendItem(QStringLiteral("Mouse dY"));
    sensorList.appendItem(QStringLiteral("Door"));
    sensorList.appendItem(QStringLiteral("Transport pocket"));
    sensorList.appendItem(QStringLiteral("RC-5"));
    sensorList.appendItem(QStringLiteral("BPS"));
    sensorList.appendItem(QStringLiteral("Error"));

    ValueModel sensorModel;
    sensorModel.setList(&sensorList);

    qmlRegisterType<ValueModel>("Actors", 1, 0, "ActorModel");
    qmlRegisterUncreatableType<ValueList>("Actors", 1, 0, "ValueList", QStringLiteral("Actors should not be created in QML"));

    ValueList actorList;
    actorList.appendItem(QStringLiteral("Motor left"));
    actorList.appendItem(QStringLiteral("Motor right"));
    actorList.appendItem(QStringLiteral("LEDs"));
    ValueModel actorModel;
    actorModel.setList(&actorList);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("sensorModel"), &sensorModel);
    engine.rootContext()->setContextProperty(QStringLiteral("actorModel"), &actorModel);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);
    engine.load(url);

    QObject* p_lcd = engine.rootObjects().at(0)->findChild<QObject*>("LCD"); // FIXME: use data model?
    QObject* p_log = engine.rootObjects().at(0)->findChild<QObject*>("log_viewer"); // FIXME: use data model?


    QHash<QString, QModelIndex> sensor_map;
    for (int i = 0; i < sensorModel.rowCount(); ++i) {
        const auto& e = sensorModel.data(sensorModel.index(i, 0), ValueModel::Name);
        sensor_map[e.toString()] = sensorModel.index(i, 0);
    }

    QHash<QString, QModelIndex> actor_map;
    for (int i = 0; i < actorModel.rowCount(); ++i) {
        const auto& e = actorModel.data(actorModel.index(i, 0), ValueModel::Name);
        actor_map[e.toString()] = actorModel.index(i, 0);
    }


    QTcpSocket socket;

    std::map<ctbot::CommandCodes /*cmd*/, std::vector<std::function<bool(const ctbot::CommandBase&)>> /*functions*/> commands_;

    commands_[ctbot::CommandCodes::CMD_WELCOME].push_back([](const ctbot::CommandBase&) {
        // std::cout << "CMD_WELCOME received: " << cmd << "\n";
        return true;
    });

    commands_[ctbot::CommandCodes::CMD_DONE].push_back([](const ctbot::CommandBase&) {
        // std::cout << "CMD_DONE received: " << cmd << "\n";
        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SHUTDOWN].push_back([&](const ctbot::CommandBase&) {
        // std::cout << "CMD_SHUTDOWN received: " << cmd << "\n";
        app.exit(0);
        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_IR].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_IR received: " << cmd << "\n";

        sensorModel.setData(sensor_map["Distance left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensorModel.setData(sensor_map["Distance right"], cmd.get_cmd_data_r(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_ENC].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_ENC received: " << cmd << "\n";

        sensorModel.setData(sensor_map["Wheel enc left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensorModel.setData(sensor_map["Wheel enc right"], cmd.get_cmd_data_r(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_BORDER].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_BORDER received: " << cmd << "\n";

        sensorModel.setData(sensor_map["Border left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensorModel.setData(sensor_map["Border right"], cmd.get_cmd_data_r(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_LINE].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_LINE received: " << cmd << "\n";

        sensorModel.setData(sensor_map["Line left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensorModel.setData(sensor_map["Line right"], cmd.get_cmd_data_r(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_LDR].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_LDR received: " << cmd << "\n";

        sensorModel.setData(sensor_map["Light left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensorModel.setData(sensor_map["Light right"], cmd.get_cmd_data_r(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_TRANS].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_TRANS received: " << cmd << "\n";

        sensorModel.setData(sensor_map["Transport pocket"], cmd.get_cmd_data_l(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_DOOR].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_DOOR received: " << cmd << "\n";

        sensorModel.setData(sensor_map["Door"], cmd.get_cmd_data_l(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_RC5].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_RC5 received: " << cmd << "\n";

        sensorModel.setData(sensor_map["RC-5"], cmd.get_cmd_data_l(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_BPS].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_BPS received: " << cmd << "\n";

        sensorModel.setData(sensor_map["BPS"], cmd.get_cmd_data_l(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_SENS_ERROR].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_ERROR received: " << cmd << "\n";

        sensorModel.setData(sensor_map["Error"], cmd.get_cmd_data_l(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_AKT_MOT].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_MOT received: " << cmd << "\n";

        actorModel.setData(actor_map["Motor left"], cmd.get_cmd_data_l(), ValueModel::Value);
        actorModel.setData(actor_map["Motor right"], cmd.get_cmd_data_r(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_AKT_LED].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_LED received: " << cmd << std::endl;

        actorModel.setData(actor_map["LEDs"], cmd.get_cmd_data_l(), ValueModel::Value);

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_AKT_LCD].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_LCD received: " << cmd << "\n";

        // FIXME: just a test implementation!
        static char text[4][21];

        switch (cmd.get_cmd_subcode()) {
        case ctbot::CommandCodes::CMD_SUB_LCD_CLEAR:
            std::memset(&text[0][0], 0, sizeof(text));
            [[fallthrough]];

        case ctbot::CommandCodes::CMD_SUB_LCD_DATA: {
            size_t col { static_cast<size_t>(cmd.get_cmd_data_l()) };
            if (col >= 20) {
                qDebug() << "invalid display command received: col =" << col;
//                std::cout << cmd << std::endl;
                col = 19;
            }
            size_t row { static_cast<size_t>(cmd.get_cmd_data_r()) };
            if (row >= 4) {
                qDebug() << "invalid display command received: row =" << row;
//                std::cout << cmd << std::endl;
                return false;
            }

            size_t len { cmd.get_payload_size() };
            if (len + col > 20) {
                qDebug() << "len updated: len =" << len << " col =" << col;
                len = 20 - col;
            }
            if (cmd.get_cmd_subcode() == ctbot::CommandCodes::CMD_SUB_LCD_DATA && !len) {
                qDebug() << "invalid display command received: len =" << len << " col =" << col;
                std::cout << cmd << std::endl;
                return false;
            }

            std::strncpy(&text[row][col], reinterpret_cast<const char*>(cmd.get_payload().data()), len);
            text[row][len + col] = 0;

            QString data = text[0];
            data += "\n";
            data += text[1];
            data += "\n";
            data += text[2];
            data += "\n";
            data += text[3];
            data.replace(QRegularExpression("[\001-\011]"), ".");
            data.replace(QRegularExpression("[\013-\037]"), ".");
            data.replace(QRegularExpression("[\177-\377]"), "#");

            if (p_lcd) {
                p_lcd->setProperty("text", data);
            }

            return true;
        }

        default:
            return false;
        }
    });

    commands_[ctbot::CommandCodes::CMD_MAP].push_back([&](const ctbot::CommandBase&) {
        // std::cout << "CMD_MAP received: " << cmd << "\n";

        return true;
    });

    commands_[ctbot::CommandCodes::CMD_LOG].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_LOG received: " << cmd << "\n";

        if (!p_log) {
            return false;
        }

        QString data { QString::fromUtf8(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int>(cmd.get_payload_size())) };
        data.replace(QRegularExpression("[\001-\011]"), ".");
        data.replace(QRegularExpression("[\013-\037]"), ".");
        data.replace(QRegularExpression("[\177-\377]"), "#");

        QMetaObject::invokeMethod(p_log, "append", Qt::DirectConnection, Q_ARG(QString, data));

        return true;
    });


    ConnectButton button { [&](QString hostname, QString port) {
        static bool connected { false };

        auto object { engine.rootObjects().at(0)->findChild<QObject*>("Hostname") };
        if (!connected) {
            socket.connectToHost(hostname, static_cast<quint16>(port.toUInt()));
            if (socket.waitForConnected()) {
                qDebug() << "Connected to " << hostname << ":" << port;
                QMetaObject::invokeMethod(object, "connected", Q_ARG(QVariant, hostname));
                connected = true;
            } else {
                qDebug() << "Connection to " << hostname << ":" << port << " failed.";
                QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, hostname));
                connected = false;
            }
        } else {
            socket.close();
            qDebug() << "Connection to " << hostname << ":" << port<< " closed.";
            QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, hostname));
            connected = false;
        }
    } };
    QObject::connect(engine.rootObjects().at(0)->findChild<QObject*>("Hostname"), SIGNAL(connectClicked(QString, QString)), &button, SLOT(cppSlot(QString, QString)));


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
                    size_t n {};
                    while (in_buffer.size() < static_cast<int>(p_cmd->get_payload_size())) {
                        QCoreApplication::processEvents();
                        if (socket.bytesAvailable()) {
                            in_buffer.append(socket.readAll());
                        }
                        if (++n > 100) {
                            break;
                        }
                        QThread::msleep(1);
                    }
                    if (!p_cmd->append_payload(in_buffer, p_cmd->get_payload_size())) {
                        qDebug() << "could not receive payload of cmd:";
                        std::cout << *p_cmd << std::endl;
                        return;
                    }
                }

                try {
                    bool result { true };
                    for (auto& func : commands_.at(p_cmd->get_cmd_code())) {
                        result &= func(*p_cmd);
                    }
                } catch (const std::out_of_range&) {
                    qDebug() << "CMD code '" << static_cast<char>(p_cmd->get_cmd_code_uint()) << "' not registered:";
                    std::cout << *p_cmd << std::endl;
                    return;
                }
            }
        }
    });

    QObject::connect(&socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Connection closed.";
        auto object { engine.rootObjects().at(0)->findChild<QObject*>("Hostname") };
        QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, ""));
    });


    std::map<QString /*RC-Code*/, uint16_t> rc5_codes { { "Power", 0x118c }, { "1", 0x1181 }, { "2", 0x1182 }, { "3", 0x1183 }, { "4", 0x1184 },
        { "5", 0x1185 }, { "6", 0x1186 }, { "7", 0x1187 }, { "8", 0x1188 }, { "9", 0x1189 }, { "10", 0x1180 }, { "11", 0x118a }, { "12", 0x11a3 },
        { "Gr", 0x01ba }, { "Re", 0x01bd }, { "Ye", 0x01b1 }, { "Bl", 0x01b0 }, { "I/II", 0x11ab }, { "TV", 0x11b8 }, { "Gr", 0x01ba }, { "^", 0x11a9 },
        { "<<", 0x11b2 }, { ">", 0x11b5 }, { ">>", 0x11b4 }, { "v", 0x11b6 }, { "*", 0x11ab }, { "CH*P/P", 0x11bf }, { "V+", 0x1190 }, { "M", 0x01bf },
        { "C+", 0x11a0 }, { "V-", 0x1191 }, { "C-", 0x11a1 } };

    ConnectButton rc_button { [&](QString button, QString) {
        const auto rc5code { static_cast<int16_t>(rc5_codes[button]) };
        // qDebug() << "button " << button << "pressed, code =" << rc5code;

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_SENS_RC5, ctbot::CommandCodes::CMD_SUB_NORM, rc5code, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };

        if (socket.isOpen()) {
            qint64 sent { socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData)) };
            if (cmd.get_payload_size()) {
                sent += socket.write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            }
            //  qDebug() << "sent" << sent << "bytes.";
        }
    } };
    QObject::connect(engine.rootObjects().at(0)->findChild<QObject*>("RCButton"), SIGNAL(connectClicked(QString, QString)), &rc_button, SLOT(cppSlot(QString, QString)));


    return app.exec();
}
