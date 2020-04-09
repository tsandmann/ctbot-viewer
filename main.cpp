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
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <cstring>

#include "value_model.h"
#include "value_list.h"
#include "remotecall_model.h"
#include "remotecall_list.h"
#include "command.h"
#include "connect_button.h"
#include "map_image.h"


static QQuickItem* FindItemByName(QList<QObject*> nodes, const QString& name) {
    for (int i {}; i < nodes.size(); ++i) {
        // search for node
        if (nodes.at(i) && nodes.at(i)->objectName() == name) {
            return dynamic_cast<QQuickItem*>(nodes.at(i));
        }
        // search in children
        else if (nodes.at(i) && nodes.at(i)->children().size() > 0) {
            QQuickItem* item = FindItemByName(nodes.at(i)->children(), name);
            if (item) {
                return item;
            }
        }
    }
    // not found
    return nullptr;
}


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


    qmlRegisterType<RCModel>("RemoteCalls", 1, 0, "RemotecallModel");
    qmlRegisterUncreatableType<RCList>("RemoteCalls", 1, 0, "RCList", QStringLiteral("RemoteCalls should not be created in QML"));

    RCList* p_rcList { new RCList };
    RCModel rcModel;
    rcModel.setList(p_rcList);

    qmlRegisterType<MapImageItem>("mapimage", 1, 0, "MapImageItem");


    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("sensorModel"), &sensorModel);
    engine.rootContext()->setContextProperty(QStringLiteral("actorModel"), &actorModel);
    engine.rootContext()->setContextProperty(QStringLiteral("remotecallModel"), &rcModel);
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

    QObject* p_lcd = engine.rootObjects().first()->findChild<QObject*>("LCD");
    QObject* p_log = engine.rootObjects().first()->findChild<QObject*>("log_viewer");
    QObject* p_script = engine.rootObjects().first()->findChild<QObject*>("script_viewer");


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

    MapImageItem* p_map { engine.rootObjects().first()->findChild<MapImageItem*>("Map") };

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
        auto object { engine.rootObjects().at(0)->findChild<QObject*>("Hostname") };
        socket.close();
        QMetaObject::invokeMethod(object, "disconnected", Q_ARG(QVariant, ""));
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
            case ctbot::CommandCodes::CMD_SUB_LCD_CLEAR: {
                // qDebug() << "Display CLEAR received.";
                std::memset(&text[0][0], ' ', sizeof(text));
                text[0][20] = 0;
                text[1][20] = 0;
                text[2][20] = 0;
                text[3][20] = 0;

                QString data = text[0];
                data += "\n";
                data += text[1];
                data += "\n";
                data += text[2];
                data += "\n";
                data += text[3];

                if (p_lcd) {
                    p_lcd->setProperty("text", data);
                }

                return true;
            }

            case ctbot::CommandCodes::CMD_SUB_LCD_DATA: {
                // std::string payload { reinterpret_cast<const char*>(cmd.get_payload().data()), cmd.get_payload_size() };
                // qDebug() << "Display DATA received: col =" << cmd.get_cmd_data_l() << " row = " << cmd.get_cmd_data_r() << " len = " <<
                // cmd.get_payload().size() << " text = " << QString::fromStdString(payload);

                size_t col { static_cast<size_t>(cmd.get_cmd_data_l()) };
                if (col >= 20) {
                    qDebug() << "invalid display command received: col =" << col;
                    //  std::cout << cmd << std::endl;
                    col = 19;
                }
                size_t row { static_cast<size_t>(cmd.get_cmd_data_r()) };
                if (row >= 4) {
                    qDebug() << "invalid display command received: row =" << row;
                    //  std::cout << cmd << std::endl;
                    return false;
                }

                size_t len { cmd.get_payload_size() };
                if (len + col > 20) {
                    // qDebug() << "len updated: len =" << len << " col =" << col;
                    len = 20 - col;
                }
                if (!len) {
                    qDebug() << "invalid display command received: len =" << len << " col =" << col;
                    std::cout << cmd << std::endl;
                    return false;
                }

                std::strncpy(&text[row][col], reinterpret_cast<const char*>(cmd.get_payload().data()), len);
                text[row][col + len] = 0;

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
                // qDebug() << "data = " << data;

                if (p_lcd) {
                    p_lcd->setProperty("text", data);
                }

                return true;
            }

            default: return false;
        }
    });

    commands_[ctbot::CommandCodes::CMD_MAP].push_back([&](const ctbot::CommandBase& cmd) {
        static unsigned receive_state {};
        static uint16_t last_block {};

        // std::cout << "CMD_MAP received: " << cmd << "\n";

        if (!p_map) {
            return false;
        }

        const auto block = cmd.get_cmd_data_l(); // 16 Bit Adresse des Map-Blocks
        switch (cmd.get_cmd_subcode()) {
        case ctbot::CommandCodes::CMD_SUB_MAP_DATA_1: {
            if (receive_state != 0) {
                receive_state = 0;
                return false;
            }
            p_map->set_bot_y(MapImageItem::MAP_PIXEL_SIZE_ - cmd.get_cmd_data_r()); // Bot-Position, X-Komponente, wird im Bild in Y-Richtung gezaehlt
            p_map->update_map(cmd.get_payload().data(), block, 0, 7);
            receive_state = 1;
            last_block = block;
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_DATA_2: {
            if (receive_state != 1 || last_block != block) {
                receive_state = 0;
                return false;
            }
            p_map->set_bot_x(MapImageItem::MAP_PIXEL_SIZE_ - cmd.get_cmd_data_r()); // Bot-Position, Y-Komponente, wird im Bild in X-Richtung gezaehlt
            p_map->update_map(cmd.get_payload().data(), block, 8, 15);
            receive_state = 2;
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_DATA_3: {
            if (receive_state != 2 || last_block != block) {
                receive_state = 0;
                return false;
            }
            p_map->set_bot_heading(cmd.get_cmd_data_r());
            p_map->update_map(cmd.get_payload().data(), block, 16, 23);
            receive_state = 3;
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_DATA_4: {
            static QPoint last_bot_pos {};

            if (receive_state != 3 || last_block != block) {
                receive_state = 0;
                return false;
            }
            p_map->update_map(cmd.get_payload().data(), block, 24, 31);
            p_map->commit();
            const auto bot_pos { p_map->get_bot_pos() };
            if ((last_bot_pos - bot_pos).manhattanLength() > 10) {
                last_bot_pos = bot_pos;

                QMetaObject::invokeMethod(p_map, "scroll_to", Q_ARG(QVariant, bot_pos.x()), Q_ARG(QVariant, bot_pos.y()));
            }
            receive_state = 0;
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_LINE: {
            if (cmd.get_payload_size() < 8) {
                return false;
            }

            QColor color;
            switch (block) {
            case 0:
                color.setRgb(0, 255, 0);
                break;

            case 1:
                color.setRgb(255, 0, 0);
                break;

            default:
                color.setRgb(0, 0, 0);
                break;
            }

            const auto y1 { cmd.get_payload()[0] | cmd.get_payload()[1] << 8 };
            const auto x1 { cmd.get_payload()[2] | cmd.get_payload()[3] << 8 };
            const auto y2 { cmd.get_payload()[4] | cmd.get_payload()[5] << 8 };
            const auto x2 { cmd.get_payload()[6] | cmd.get_payload()[7] << 8 };

            p_map->draw_line(QPoint { x1, y1 }, QPoint { x2, y2 }, color);
            p_map->commit();
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_CIRCLE: {
            if (cmd.get_payload_size() < 4) {
                return false;
            }

            QColor color;
            switch (block) {
            case 0:
                color.setRgb(0, 255, 0);
                break;

            case 1:
                color.setRgb(255, 0, 0);
                break;

            default:
                color.setRgb(0, 0, 0);
                break;
            }

            const auto radius { cmd.get_cmd_data_r() };
            const auto y { cmd.get_payload()[0] | cmd.get_payload()[1] << 8 };
            const auto x { cmd.get_payload()[2] | cmd.get_payload()[3] << 8 };

            p_map->draw_cicle(QPoint { x, y }, radius, color);
            p_map->commit();
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_CLEAR_LINES: {
            p_map->clear_lines(block);
            p_map->commit();
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_CLEAR_CIRCLES: {
            p_map->clear_circles(block);
            p_map->commit();
            break;
        }

        default:
            return false;
        }

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

    commands_[ctbot::CommandCodes::CMD_REMOTE_CALL].push_back([&](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_REMOTE_CALL received: " << cmd << "\n";

        if (cmd.get_cmd_subcode() != ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_ENTRY) {
            if (cmd.get_cmd_subcode() == ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_DONE) {
                auto entry = FindItemByName(engine.rootObjects(), "RemoteCallViewer");
                if (entry) {
                    entry->setProperty("enabled", true);

                    auto e = FindItemByName(engine.rootObjects(), "CurrentRemoteCallLabel");
                    if (e) {
                        e->setProperty("text", "Active Remote Call: none");
                    }

                    qDebug() << "RemoteCall done.";
                    return true;
                }
            }

            return false;
        }

        if (cmd.get_payload_size() < 66) {
            return false;
        }

        const QString name { QString::fromUtf8(reinterpret_cast<const char*>(cmd.get_payload().data() + 4)) };
        const QString params { QString::fromUtf8(reinterpret_cast<const char*>(cmd.get_payload().data() + 4 + 21)) };

        // qDebug() << "name =" << name << ", params =" << params;

        if (name.length()) {
            auto items { p_rcList->items() };
            for (int i {}; i < items.size(); ++i) {
                if (items[i].name == name) {
                    return true;
                }
            }

            p_rcList->appendItem(name, params);
        }

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
            // qDebug() << "sent" << sent << "bytes.";
        }
    } };
    QObject::connect(
        engine.rootObjects().first()->findChild<QObject*>("RCButton"), SIGNAL(rcButtonClicked(QString, QString)), &rc_button, SLOT(cppSlot(QString, QString)));

    ConnectButton remotecall_fetch { [&](QString, QString) {
        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_REMOTE_CALL, ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_LIST, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        if (socket.isOpen()) {
            socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
        }
    } };
    QObject::connect(engine.rootObjects().first()->findChild<QObject*>("RemoteCallActions"), SIGNAL(remoteCallFetch()), &remotecall_fetch, SLOT(cppSlot()));

    ConnectButton remotecall_clear { [&](QString, QString) {
        rcModel.setList(nullptr);
        delete p_rcList;
        p_rcList = new RCList;
        rcModel.setList(p_rcList);

        auto entry = FindItemByName(engine.rootObjects(), "RemoteCallViewer");
        if (entry) {
            entry->setProperty("enabled", true);
        }
        auto e = FindItemByName(engine.rootObjects(), "CurrentRemoteCallLabel");
        if (e) {
            e->setProperty("text", "Active Remote Call: none");
        }
    } };
    QObject::connect(engine.rootObjects().first()->findChild<QObject*>("RemoteCallActions"), SIGNAL(remoteCallClear()), &remotecall_clear, SLOT(cppSlot()));

    ConnectButton remotecall_abort { [&](QString, QString) {
        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_REMOTE_CALL, ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_ABORT, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        if (socket.isOpen()) {
            socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
        }

        auto entry = FindItemByName(engine.rootObjects(), "RemoteCallViewer");
        if (entry) {
            entry->setProperty("enabled", true);
        }
        auto e = FindItemByName(engine.rootObjects(), "CurrentRemoteCallLabel");
        if (e) {
            e->setProperty("text", "Active Remote Call: none");
        }
    } };
    QObject::connect(engine.rootObjects().first()->findChild<QObject*>("RemoteCallActions"), SIGNAL(remoteCallAbort()), &remotecall_abort, SLOT(cppSlot()));


    ConnectButton remotecall_button { [&](QString name, QString parameter) {
        qDebug() << "remotecall " << name << "(" << parameter << ")";

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_REMOTE_CALL, ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_ORDER, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };

        QByteArray payload;
        payload.append(name.replace("():", ""));
        payload.append('\0');

        const QStringList par_list { parameter.split(',', QString::SkipEmptyParts) };
        uint32_t par[] { static_cast<uint32_t>(par_list.at(0).toLong()), static_cast<uint32_t>(par_list.at(1).toLong()),
            static_cast<uint32_t>(par_list.at(2).toLong()) };

        for (size_t i {}; i < 3; ++i) {
            payload.append(static_cast<char>((par[i] >> 0) & 0xff));
            payload.append(static_cast<char>((par[i] >> 8) & 0xff));
            payload.append(static_cast<char>((par[i] >> 16) & 0xff));
            payload.append(static_cast<char>((par[i] >> 24) & 0xff));
        }

        // qDebug() << "payload =" << payload;

        cmd.add_payload(payload.data(), static_cast<size_t>(payload.size()));

        if (socket.isOpen()) {
            qint64 sent { socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData)) };
            if (cmd.get_payload_size()) {
                sent += socket.write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            }
            qDebug() << "sent" << sent << "bytes.";
        }

        auto entry = FindItemByName(engine.rootObjects(), "RemoteCallViewer");
        if (entry) {
            entry->setProperty("enabled", false);
        }
        auto e = FindItemByName(engine.rootObjects(), "CurrentRemoteCallLabel");
        if (e) {
            e->setProperty("text", "Active Remote Call: " + name);
        }
    } };
    QObject::connect(engine.rootObjects().first()->findChild<QObject*>("RemoteCallViewer"), SIGNAL(remoteCallClicked(QString, QString)), &remotecall_button,
        SLOT(cppSlot(QString, QString)));

    ConnectButton map_fetch { [&](QString, QString) {
        if (!p_map) {
            return;
        }

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_MAP, ctbot::CommandCodes::CMD_SUB_MAP_REQUEST, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        if (socket.isOpen()) {
            socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
        }
    } };
    QObject::connect(engine.rootObjects().first()->findChild<QObject*>("MapViewer"), SIGNAL(mapFetch()), &map_fetch, SLOT(cppSlot()));

    ConnectButton map_clear { [&](QString, QString) {
        if (!p_map) {
            return;
        }
        p_map->clear();
        p_map->set_bot_x(0);
        p_map->set_bot_y(0);
        p_map->set_bot_heading(0);
        p_map->commit();
    } };
    QObject::connect(engine.rootObjects().first()->findChild<QObject*>("MapViewer"), SIGNAL(mapClear()), &map_clear, SLOT(cppSlot()));

    ConnectButton map_save { [&](QString filename, QString) {
        if (!p_map) {
            return;
        }

        p_map->save_to_file(filename);
    } };
    QObject::connect(engine.rootObjects().first()->findChild<QObject*>("MapViewer"), SIGNAL(mapSave(QString)), &map_save, SLOT(cppSlot(QString)));

    ConnectButton scripts_load { [&](QString filename, QString) {
        if (!p_script) {
            return;
        }
        auto p_editor{ p_script->findChild<QQuickItem*>("script_editor") };
        if (!p_editor) {
            return;
        }

        QUrl url { filename };
        QFile file { url.toLocalFile() };
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }
        QTextStream stream { &file };
        QString text;
        while (!stream.atEnd()) {
            text.append(stream.readLine());
            text.append("\n");
        }
        file.close();

        p_editor->setProperty("text", text);

        qDebug() << "script loaded from: " << url.toLocalFile();
    } };
    QObject::connect(p_script, SIGNAL(scriptLoad(QString)), &scripts_load, SLOT(cppSlot(QString)));

    ConnectButton scripts_save { [&](QString filename, QString) {
        if (!p_script) {
            return;
        }
        auto p_editor{ p_script->findChild<QQuickItem*>("script_editor") };
        if (!p_editor) {
            return;
        }

        QUrl url { filename };
        QFile file { url.toLocalFile() };
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return;
        }
        QTextStream stream { &file };
        const QString text { p_editor->property("text").toString() };
        stream << text;
        file.close();

        qDebug() << "script saved to: " << url.toLocalFile();
    } };
    QObject::connect(p_script, SIGNAL(scriptSave(QString)), &scripts_save, SLOT(cppSlot(QString)));

    ConnectButton scripts_send { [&](QString, QString) {
        if (!p_script || !socket.isOpen()) {
            return;
        }

        auto p_type { p_script->findChild<QQuickItem*>("scripts_abl") };
        auto p_execute { p_script->findChild<QQuickItem*>("script_execute") };
        auto p_filename { p_script->findChild<QQuickItem*>("script_remote_filename") };
        auto p_content { p_script->findChild<QQuickItem*>("script_editor") };

        if (!p_type || !p_execute || !p_filename || !p_content) {
            return;
        }

        const bool type { p_type->property("checked").toBool() }; // false: basic, true: abl
        const bool execute { p_execute->property("checked").toBool() };
        const QByteArray remote_filename { p_filename->property("text").toString().toLatin1() };
        const QByteArray content_array { p_content->property("text").toString().toLatin1() };
        const auto content_length { static_cast<int16_t>(content_array.length()) };

        // qDebug() << "type=" << type << "execute=" << execute << "filename=" << remote_filename << "length=" << content_length;
        // qDebug() << "content=" << content_array;

        if (remote_filename.length() < 5) {
            return;
        }

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_PREPARE, static_cast<int16_t>(type), content_length, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        cmd.add_payload(remote_filename.constData(), remote_filename.length());

        qint64 sent { socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData)) };
        sent += socket.write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
        socket.flush();

        // qDebug() << "script prepared, sent=" << sent;
        QThread::usleep(75 * 3'000);

        sent = 0;
        int16_t i {};
        for (; i < content_length / 64; ++i) {
            ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_DATA, static_cast<int16_t>(type), static_cast<int16_t>(i * 64),
                ctbot::CommandBase::ADDR_SIM, ctbot::CommandBase::ADDR_BROADCAST };
            cmd.add_payload(&content_array.constData()[i * 64], 64);
            sent += socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
            sent += socket.write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            socket.flush();

            QThread::usleep(75 * 1'000);
        }

        const auto to_send { content_length % 64 };
        // qDebug() << "to_send=" << to_send;

        if (to_send) {
            ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_DATA, static_cast<int16_t>(type), static_cast<int16_t>(i * 64),
                ctbot::CommandBase::ADDR_SIM, ctbot::CommandBase::ADDR_BROADCAST };
            cmd.add_payload(&content_array.constData()[i * 64], to_send);
            sent += socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
            sent += socket.write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            socket.flush();

            QThread::usleep(75 * 1'000);
        }

        qDebug() << "script sent," << sent << "bytes.";

        if (execute) {
            ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_START, static_cast<int16_t>(type), 0,
                ctbot::CommandBase::ADDR_SIM, ctbot::CommandBase::ADDR_BROADCAST };
            socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));

            qDebug() << "script started.";
        }

    } };
    QObject::connect(p_script, SIGNAL(scriptSend()), &scripts_send, SLOT(cppSlot()));

    ConnectButton scripts_abort { [&](QString, QString) {
        auto p_type { p_script->findChild<QQuickItem*>("scripts_abl") };
        if (!p_type || !socket.isOpen()) {
            return;
        }

        const bool type { p_type->property("checked").toBool() }; // false: basic, true: abl

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_STOP, static_cast<int16_t>(type), 0,
            ctbot::CommandBase::ADDR_SIM, ctbot::CommandBase::ADDR_BROADCAST };
        socket.write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));

        qDebug() << "script aborted.";
    } };
    QObject::connect(p_script, SIGNAL(scriptAbort()), &scripts_abort, SLOT(cppSlot()));

    return app.exec();
}
