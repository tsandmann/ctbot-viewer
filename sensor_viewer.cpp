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
 * @file    sensor_viewer.cpp
 * @brief   Sensor viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#include <QString>
#include <QQmlContext>
#include <QDebug>
#include <iostream>

#include "sensor_viewer.h"
#include "connection_manager.h"
#include "command.h"


SensorViewer::SensorViewer(QQmlApplicationEngine* p_engine, ConnectionManager& command_eval) : ValueViewer { p_engine } {
    qmlRegisterType<ValueModel>("Sensors", 1, 0, "SensorModel");
    qmlRegisterUncreatableType<ValueList>("Sensors", 1, 0, "ValueList", QStringLiteral("Sensors should not be created in QML"));

    list_.appendItem(QStringLiteral("Wheel enc left"));
    list_.appendItem(QStringLiteral("Wheel enc right"));
    list_.appendItem(QStringLiteral("Distance left"));
    list_.appendItem(QStringLiteral("Distance right"));
    list_.appendItem(QStringLiteral("Line left"));
    list_.appendItem(QStringLiteral("Line right"));
    list_.appendItem(QStringLiteral("Border left"));
    list_.appendItem(QStringLiteral("Border right"));
    list_.appendItem(QStringLiteral("Light left"));
    list_.appendItem(QStringLiteral("Light right"));
    list_.appendItem(QStringLiteral("Mouse dX"));
    list_.appendItem(QStringLiteral("Mouse dY"));
    list_.appendItem(QStringLiteral("Door"));
    list_.appendItem(QStringLiteral("Transport pocket"));
    list_.appendItem(QStringLiteral("RC-5"));
    list_.appendItem(QStringLiteral("BPS"));
    list_.appendItem(QStringLiteral("Error"));

    update_map();
    register_model(QStringLiteral("sensorModel"));

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_IR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_IR received: " << cmd << "\n";
        model_.setData(map_["Distance left"], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_["Distance right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_ENC, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_ENC received: " << cmd << "\n";
        model_.setData(map_["Wheel enc left"], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_["Wheel enc right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_BORDER, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_BORDER received: " << cmd << "\n";
        model_.setData(map_["Border left"], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_["Border right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_LINE, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_LINE received: " << cmd << "\n";
        model_.setData(map_["Line left"], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_["Line right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_LDR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_LDR received: " << cmd << "\n";
        model_.setData(map_["Light left"], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_["Light right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_TRANS, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_TRANS received: " << cmd << "\n";
        model_.setData(map_["Transport pocket"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_DOOR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_DOOR received: " << cmd << "\n";
        model_.setData(map_["Door"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_RC5, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_RC5 received: " << cmd << "\n";
        model_.setData(map_["RC-5"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_BPS, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_BPS received: " << cmd << "\n";
        model_.setData(map_["BPS"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_ERROR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_ERROR received: " << cmd << "\n";
        model_.setData(map_["Error"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });
}
