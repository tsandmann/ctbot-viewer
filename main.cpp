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
#include <QString>

#include "connection_manager.h"
#include "sensor_viewer.h"
#include "actuator_viewer.h"
#include "remotecontrol_viewer.h"
#include "remotecall_viewer.h"
#include "log_viewer.h"
#include "map_viewer.h"
#include "script_editor.h"


int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app { argc, argv };
    QQmlApplicationEngine engine;

    ConnectionManager connection { &engine };

    SensorViewer sensor_viewer { &engine, connection };
    ActuatorViewer actuator_viewer { &engine, connection };
    RemoteControlViewer rc5_viewer { &engine, connection.get_socket() };
    RemotecallViewer remotecall_viewer { &engine, connection };
    LogViewer log_viewer { &engine, connection };
    MapViewer map_viewer { &engine, connection };
    ScriptEditor script_editor { &engine, connection.get_socket() };

    const QUrl main_qlm { QStringLiteral("qrc:/Main.qml") };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [main_qlm](QObject* p_object, const QUrl& object_url) {
        if (!p_object && main_qlm == object_url) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    engine.load(main_qlm);

    connection.register_buttons();
    rc5_viewer.register_buttons();
    remotecall_viewer.register_buttons();
    map_viewer.register_buttons();
    script_editor.register_buttons();

    return app.exec();
}
