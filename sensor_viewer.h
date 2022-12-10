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
 * @file    sensor_viewer.h
 * @brief   Sensor viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#pragma once

#include "value_viewer.h"

#include <string_view>
#include <regex>


class ConnectionManagerV1;
class ConnectionManagerV2;

class SensorViewerV1 : public ValueViewer {
protected:
    static constexpr std::string_view SPEED_ENC_L_ { "Wheel enc left" };
    static constexpr std::string_view SPEED_ENC_R_ { "Wheel enc right" };
    static constexpr std::string_view DISTANCE_L_ { "Distance left" };
    static constexpr std::string_view DISTANCE_R_ { "Distance right" };
    static constexpr std::string_view LINE_L_ { "Line left" };
    static constexpr std::string_view LINE_R_ { "Line right" };
    static constexpr std::string_view BORDER_L_ { "Border left" };
    static constexpr std::string_view BORDER_R_ { "Border right" };
    static constexpr std::string_view LIGHT_L_ { "Light left" };
    static constexpr std::string_view LIGHT_R_ { "Light right" };
    static constexpr std::string_view MOUSE_DX_ { "Mouse dX" };
    static constexpr std::string_view MOUSE_DY_ { "Mouse dY" };
    static constexpr std::string_view DOOR_ { "Door" };
    static constexpr std::string_view TRANSPORT_ { "Transport pocket" };
    static constexpr std::string_view RC5_ { "RC-5 Command" };
    static constexpr std::string_view BPS_ { "BPS" };
    static constexpr std::string_view ERROR_ { "Error" };

public:
    SensorViewerV1(QQmlApplicationEngine* p_engine, ConnectionManagerV1& command_eval);
};


class SensorViewerV2 : public ValueViewer {
protected:
    static constexpr std::string_view SPEED_ENC_L_ { "Speed enc left [mm/s]" };
    static constexpr std::string_view SPEED_ENC_R_ { "Speed enc right [mm/s]" };
    static constexpr std::string_view SPEED_ENC_RE_ { R"(enc: (-?\d*) (-?\d*))" };
    static constexpr std::string_view DISTANCE_L_ { "Distance left [mm]" };
    static constexpr std::string_view DISTANCE_R_ { "Distance right [mm]" };
    static constexpr std::string_view DISTANCE_RE_ { R"(dist: (\d*) (\d*))" };
    static constexpr std::string_view LINE_L_ { "Line left" };
    static constexpr std::string_view LINE_R_ { "Line right" };
    static constexpr std::string_view LINE_RE_ { R"(line: (\d*) (\d*))" };
    static constexpr std::string_view BORDER_L_ { "Border left" };
    static constexpr std::string_view BORDER_R_ { "Border right" };
    static constexpr std::string_view BORDER_RE_ { R"(border: (\d*) (\d*))" };
    static constexpr std::string_view DOOR_ { "Door" };
    static constexpr std::string_view TRANSPORT_ { "Transport pocket" };
    static constexpr std::string_view TRANSPORT_MM_ { "Transport pocket [mm]" };
    static constexpr std::string_view TRANSPORT_RE_ { R"(trans: (\d*) (\d*))" };
    static constexpr std::string_view RC5_ { "RC-5 Command" };
    static constexpr std::string_view RC5_RE_ { R"(rc5: (\d*) (\d*) (?:\d*))" };
    static constexpr std::string_view BPS_ { "BPS" };
    static constexpr std::string_view CURRENT_5V_ { "5V rail current [mA]" };
    static constexpr std::string_view CURRENT_SERVO_ { "Servo current [mA]" };
    static constexpr std::string_view CURRENT_RE_ { R"(currents: (\d*) (\d*))" };
    static constexpr std::string_view CURRENT_MOTOR_ { "Motor current [mA]" };
    static constexpr std::string_view CURRENT_MOTOR_RE_ { R"(mcurrent: (\d*))" };
    static constexpr std::string_view BAT_VOLTAGE_ { "Battery [mV]" };
    static constexpr std::string_view BAT_VOLTAGE_CELL_ { "Battery (per cell) [mV]" };
    static constexpr std::string_view BAT_VOLTAGE_RE_ { R"(bat: (\d*\.\d*) (\d*\.\d*))" };

    static inline const std::regex enc_regex_ { SPEED_ENC_RE_.cbegin() };
    static inline const std::regex dist_regex_ { DISTANCE_RE_.cbegin() };
    static inline const std::regex line_regex_ { LINE_RE_.cbegin() };
    static inline const std::regex border_regex_ { BORDER_RE_.cbegin() };
    static inline const std::regex trans_regex_ { TRANSPORT_RE_.cbegin() };
    static inline const std::regex rc5_regex_ { RC5_RE_.cbegin() };
    static inline const std::regex bat_regex_ { BAT_VOLTAGE_RE_.cbegin() };
    static inline const std::regex currents_regex_ { CURRENT_RE_.cbegin() };
    static inline const std::regex mcurrent_regex_ { CURRENT_MOTOR_RE_.cbegin() };

public:
    SensorViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval);
};
