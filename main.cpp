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

#include "command_evaluator.h"
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

    CommandEvaluator command_eval_ { &engine };

    SensorViewer sensor_viewer { &engine, command_eval_ };
    ActuatorViewer actuator_viewer { &engine, command_eval_ };
    RemoteControlViewer rc5_viewer { &engine, command_eval_.get_socket() };
    RemotecallViewer remotecall_viewer { &engine, command_eval_ };
    LogViewer log_viewer { &engine, command_eval_ };
    MapViewer map_viewer { &engine, command_eval_ };
    ScriptEditor script_editor { &engine, command_eval_.get_socket() };


    const QUrl url { QStringLiteral("qrc:/Main.qml") };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    engine.load(url);

    command_eval_.register_buttons();
    rc5_viewer.register_buttons();
    remotecall_viewer.register_buttons();
    map_viewer.register_buttons();
    script_editor.register_buttons();

    return app.exec();
}
