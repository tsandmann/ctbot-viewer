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
 * @file    sensor_viewer.cpp
 * @brief   Sensor viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#include <QQmlApplicationEngine>
#include <QString>
#include <QQmlContext>
#include <QDebug>

#include <iostream>

#include "sensor_viewer.h"
#include "connection_manager.h"
#include "command.h"


SensorViewerV1::SensorViewerV1(QQmlApplicationEngine* p_engine, ConnectionManagerV1& command_eval) : ValueViewer { p_engine } {
    qmlRegisterType<ValueModel>("Sensors", 1, 0, "SensorModel");
    qmlRegisterUncreatableType<ValueList>("Sensors", 1, 0, "ValueList", QStringLiteral("Sensors should not be created in QML"));

    list_.appendItem(SPEED_ENC_L_.cbegin());
    list_.appendItem(SPEED_ENC_R_.cbegin());
    list_.appendItem(DISTANCE_L_.cbegin());
    list_.appendItem(DISTANCE_R_.cbegin());
    list_.appendItem(LINE_L_.cbegin());
    list_.appendItem(LINE_R_.cbegin());
    list_.appendItem(BORDER_L_.cbegin());
    list_.appendItem(BORDER_R_.cbegin());
    list_.appendItem(LIGHT_L_.cbegin());
    list_.appendItem(LIGHT_R_.cbegin());
    list_.appendItem(MOUSE_DX_.cbegin());
    list_.appendItem(MOUSE_DY_.cbegin());
    list_.appendItem(DOOR_.cbegin());
    list_.appendItem(TRANSPORT_.cbegin());
    list_.appendItem(RC5_.cbegin());
    list_.appendItem(BPS_.cbegin());
    list_.appendItem(ERROR_.cbegin());

    update_map();
    register_model(QStringLiteral("sensorModel"));

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_IR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_IR received: " << cmd << "\n";
        model_.setData(map_[DISTANCE_L_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_[DISTANCE_R_.cbegin()], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_ENC, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_ENC received: " << cmd << "\n";
        model_.setData(map_[SPEED_ENC_L_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_[SPEED_ENC_R_.cbegin()], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_BORDER, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_BORDER received: " << cmd << "\n";
        model_.setData(map_[BORDER_L_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_[BORDER_R_.cbegin()], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_LINE, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_LINE received: " << cmd << "\n";
        model_.setData(map_[LINE_L_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_[LINE_R_.cbegin()], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_LDR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_LDR received: " << cmd << "\n";
        model_.setData(map_[LIGHT_L_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_[LIGHT_R_.cbegin()], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_TRANS, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_TRANS received: " << cmd << "\n";
        model_.setData(map_[TRANSPORT_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_DOOR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_DOOR received: " << cmd << "\n";
        model_.setData(map_[DOOR_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_RC5, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_RC5 received: " << cmd << "\n";
        model_.setData(map_[RC5_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_BPS, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_BPS received: " << cmd << "\n";
        model_.setData(map_[BPS_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_ERROR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_ERROR received: " << cmd << "\n";
        model_.setData(map_[ERROR_.cbegin()], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });
}


SensorViewerV2::SensorViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval) : ValueViewer { p_engine } {
    qmlRegisterType<ValueModel>("Sensors", 1, 0, "SensorModel");
    qmlRegisterType<ValueModel>("Sensors", 1, 0, "SensorModel");
    qmlRegisterUncreatableType<ValueList>("Sensors", 1, 0, "ValueList", QStringLiteral("Sensors should not be created in QML"));

    list_.appendItem(SPEED_ENC_L_.cbegin());
    list_.appendItem(SPEED_ENC_R_.cbegin());
    list_.appendItem(DISTANCE_L_.cbegin());
    list_.appendItem(DISTANCE_R_.cbegin());
    list_.appendItem(LINE_L_.cbegin());
    list_.appendItem(LINE_R_.cbegin());
    list_.appendItem(BORDER_L_.cbegin());
    list_.appendItem(BORDER_R_.cbegin());
    // list_.appendItem(DOOR_.cbegin());
    list_.appendItem(TRANSPORT_.cbegin());
    list_.appendItem(TRANSPORT_MM_.cbegin());
    list_.appendItem(RC5_.cbegin());
    // list_.appendItem(BPS_.cbegin());
    list_.appendItem(CURRENT_5V_.cbegin());
    list_.appendItem(CURRENT_MOTOR_.cbegin());
    list_.appendItem(CURRENT_SERVO_.cbegin());
    list_.appendItem(BAT_VOLTAGE_.cbegin());
    list_.appendItem(BAT_VOLTAGE_CELL_.cbegin());

    update_map();
    register_model(QStringLiteral("sensorModelV2"));

    command_eval.register_cmd("sens", [this, &command_eval](const std::string_view& str) {
        // qDebug() << "SENSORS received: " << QString::fromUtf8(str.data(), str.size());

        if (command_eval.get_version() != command_eval.version_active()) {
            return false;
        }

        if (!str.length()) {
            return false;
        }

        int16_t values[2];
        if (parse(str, enc_regex_, values[0], values[1])) {
            model_.setData(map_[SPEED_ENC_L_.cbegin()], values[0], ValueModel::Value);
            model_.setData(map_[SPEED_ENC_R_.cbegin()], values[1], ValueModel::Value);
        }

        if (parse(str, dist_regex_, values[0], values[1])) {
            model_.setData(map_[DISTANCE_L_.cbegin()], values[0], ValueModel::Value);
            model_.setData(map_[DISTANCE_R_.cbegin()], values[1], ValueModel::Value);
        }

        if (parse(str, line_regex_, values[0], values[1])) {
            model_.setData(map_[LINE_L_.cbegin()], values[0], ValueModel::Value);
            model_.setData(map_[LINE_R_.cbegin()], values[1], ValueModel::Value);
        }

        if (parse(str, border_regex_, values[0], values[1])) {
            model_.setData(map_[BORDER_L_.cbegin()], values[0], ValueModel::Value);
            model_.setData(map_[BORDER_R_.cbegin()], values[1], ValueModel::Value);
        }

        // Door

        if (parse(str, trans_regex_, values[0], values[1])) {
            model_.setData(map_[TRANSPORT_.cbegin()], values[0], ValueModel::Value);
            model_.setData(map_[TRANSPORT_MM_.cbegin()], values[1], ValueModel::Value);
        }

        if (parse(str, rc5_regex_, values[0], values[1])) {
            model_.setData(map_[RC5_.cbegin()], values[1], ValueModel::Value);
        }

        // BPS

        if (parse(str, currents_regex_, values[0], values[1])) {
            model_.setData(map_[CURRENT_5V_.cbegin()], values[0], ValueModel::Value);
            model_.setData(map_[CURRENT_SERVO_.cbegin()], values[1], ValueModel::Value);
        }

        if (parse(str, mcurrent_regex_, values[0])) {
            model_.setData(map_[CURRENT_MOTOR_.cbegin()], values[0], ValueModel::Value);
        }

        float bat[2];
        if (parse(str, bat_regex_, bat[0], bat[1])) {
            model_.setData(map_[BAT_VOLTAGE_.cbegin()], static_cast<int>(bat[0] * 1'000.f), ValueModel::Value);
            model_.setData(map_[BAT_VOLTAGE_CELL_.cbegin()], static_cast<int>(bat[1] * 1'000.f), ValueModel::Value);
            // qDebug() << "Bat=" << bat[0] << " " << bat[1];
        }

        return true;
    });
}
