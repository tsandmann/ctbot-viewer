/*
 * This file is part of the ct-Bot remote viewer tool.
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
 * @file    main.cpp
 * @brief   ct-Bot remote viewer tool
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QString>

#include "connection_manager.h"
#include "sensor_viewer.h"
#include "actuator_viewer.h"
#include "system_viewer.h"
#include "remotecontrol_viewer.h"
#include "remotecall_viewer.h"
#include "log_viewer.h"
#include "map_viewer.h"
#include "script_editor.h"
#include "bot_console.h"


int main(int argc, char* argv[]) {
    QApplication app { argc, argv };
    app.setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QQmlApplicationEngine engine;

    ConnectionManagerV1 connection_v1 { &engine };
    ConnectionManagerV2 connection_v2 { &engine };

    SensorViewerV1 sensor_viewer_v1 { &engine, connection_v1 };
    SensorViewerV2 sensor_viewer_v2 { &engine, connection_v2 };
    ActuatorViewerV1 actuator_viewer_v1 { &engine, connection_v1 };
    ActuatorViewerV2 actuator_viewer_v2 { &engine, connection_v2 };
    SystemViewerV2 system_viewer_v2 { &engine, connection_v2 };
    RemoteControlViewerV1 rc5_viewer_v1 { &engine, connection_v1 };
    RemoteControlViewerV2 rc5_viewer_v2 { &engine, connection_v2 };
    RemotecallViewer remotecall_viewer { &engine, connection_v1 };
    LogViewerV1 log_viewer_v1 { &engine, connection_v1 };
    LogViewerV2 log_viewer_v2 { &engine, connection_v2 };
    MapViewer map_viewer { &engine, connection_v1 };
    ScriptEditor script_editor { &engine, connection_v1.get_socket() };
    BotConsole bot_console { &engine, connection_v2 };

    const QUrl main_qlm { QStringLiteral("qrc:/Main.qml") };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [main_qlm](QObject* p_object, const QUrl& object_url) {
            if (!p_object && main_qlm == object_url) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);

    engine.load(main_qlm);

    connection_v1.register_buttons();
    connection_v2.register_buttons();
    system_viewer_v2.init();
    rc5_viewer_v1.register_buttons();
    rc5_viewer_v2.register_buttons();
    remotecall_viewer.register_buttons();
    map_viewer.register_buttons();
    script_editor.register_buttons();
    bot_console.register_buttons();

    return app.exec();
}
