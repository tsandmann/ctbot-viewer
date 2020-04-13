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
 * @file    actuator_viewer.cpp
 * @brief   Actuator viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#include <QString>
#include <QRegularExpression>
#include <QDebug>
#include <cstring>
#include <iostream>

#include "actuator_viewer.h"
#include "connection_manager.h"
#include "command.h"


ActuatorViewer::ActuatorViewer(QQmlApplicationEngine* p_engine, ConnectionManager& command_eval) : ValueViewer { p_engine }, p_lcd_ {} {
    qmlRegisterType<ValueModel>("Actuators", 1, 0, "ActuatorModel");
    qmlRegisterUncreatableType<ValueList>("Actuators", 1, 0, "ValueList", QStringLiteral("Actuators should not be created in QML"));

    list_.appendItem(QStringLiteral("Motor left"));
    list_.appendItem(QStringLiteral("Motor right"));
    list_.appendItem(QStringLiteral("LEDs"));

    update_map();
    register_model(QStringLiteral("actuatorModel"));

    command_eval.register_cmd(ctbot::CommandCodes::CMD_AKT_MOT, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_MOT received: " << cmd << "\n";
        model_.setData(map_["Motor left"], cmd.get_cmd_data_l(), ValueModel::Value);
        model_.setData(map_["Motor right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_AKT_LED, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_LED received: " << cmd << std::endl;
        model_.setData(map_["LEDs"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_AKT_LCD, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_LCD received: " << cmd << "\n";

        if (!p_lcd_) {
            p_lcd_ = p_engine_->rootObjects().first()->findChild<QObject*>("LCD");
            if (!p_lcd_) {
                return false;
            }
        }

        switch (cmd.get_cmd_subcode()) {
            case ctbot::CommandCodes::CMD_SUB_LCD_CLEAR: {
                // qDebug() << "Display CLEAR received.";
                std::memset(&lcd_text_[0][0], ' ', sizeof(lcd_text_));
                lcd_text_[0][20] = 0;
                lcd_text_[1][20] = 0;
                lcd_text_[2][20] = 0;
                lcd_text_[3][20] = 0;

                QString data = lcd_text_[0];
                data += "\n";
                data += lcd_text_[1];
                data += "\n";
                data += lcd_text_[2];
                data += "\n";
                data += lcd_text_[3];

                p_lcd_->setProperty("text", data);

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

                std::strncpy(&lcd_text_[row][col], reinterpret_cast<const char*>(cmd.get_payload().data()), len);
                lcd_text_[row][col + len] = 0;

                QString data = lcd_text_[0];
                data += "\n";
                data += lcd_text_[1];
                data += "\n";
                data += lcd_text_[2];
                data += "\n";
                data += lcd_text_[3];
                data.replace(QRegularExpression("[\001-\011]"), ".");
                data.replace(QRegularExpression("[\013-\037]"), ".");
                data.replace(QRegularExpression("[\177-\377]"), "#");
                // qDebug() << "data = " << data;

                p_lcd_->setProperty("text", data);

                return true;
            }

            default: return false;
        }
    });
}
