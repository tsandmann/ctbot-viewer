/*
 * This file is part of the c't-Bot remote viewer tool.
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
 * @file    actuator_viewer.h
 * @brief   Actuator viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#pragma once

#include "value_viewer.h"

#include <QRegularExpression>

#include <string_view>
#include <regex>


class ConnectionManagerV1;
class ConnectionManagerV2;

class ActuatorViewerV1 : public ValueViewer {
protected:
    static constexpr std::string_view MOTOR_L_ { "Motor left" };
    static constexpr std::string_view MOTOR_R_ { "Motor right" };
    static constexpr std::string_view LEDS_ { "LEDs" };

    static inline const QRegularExpression regex_replace_0_ { "[\001-\007]" };
    static inline const QRegularExpression regex_replace_1_ { "[\016-\037]" };
    static inline const QRegularExpression regex_replace_2_ { "[\177-\377]" };

    QObject* p_lcd_;
    char lcd_text_[4][21];

public:
    ActuatorViewerV1(QQmlApplicationEngine* p_engine, ConnectionManagerV1& command_eval);
};


class ActuatorViewerV2 : public ValueViewer {
protected:
    static constexpr std::string_view MOTOR_L_ { "Motor left [%]" };
    static constexpr std::string_view MOTOR_R_ { "Motor right [%]" };
    static constexpr std::string_view MOTOR_RE_ { R"(motor: (-?\d*) (-?\d*))" };
    static constexpr std::string_view SERVO_1_ { "Servo 1" };
    static constexpr std::string_view SERVO_1_RE_ { R"(servo1: (\d*))" };
    static constexpr std::string_view SERVO_2_ { "Servo 2" };
    static constexpr std::string_view SERVO_2_RE_ { R"(servo2: (\d*))" };
    static constexpr std::string_view LEDS_ { "LEDs" };
    static constexpr std::string_view LEDS_RE_ { R"(leds: (\d*))" };

    static inline const std::regex motor_regex_ { MOTOR_RE_.cbegin() };
    static inline const std::regex servo1_regex_ { SERVO_1_RE_.cbegin() };
    static inline const std::regex servo2_regex_ { SERVO_2_RE_.cbegin() };
    static inline const std::regex led_regex_ { LEDS_RE_.cbegin() };

public:
    ActuatorViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval);
};
