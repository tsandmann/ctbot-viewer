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
 * @file    bot_console.h
 * @brief   Bot console component
 * @author  Timo Sandmann
 * @date    06.06.2022
 */

#pragma once

#include <QString>
#include <QRegularExpression>
#include <string_view>

#include "connect_button.h"


class QQmlApplicationEngine;
class ConnectionManagerV2;

class BotConsole {
    static inline const QRegularExpression regex_ignore_0_ { "\377.." };
    static inline const QRegularExpression regex_ignore_1_ { "[\001-\011]" };
    static inline const QRegularExpression regex_ignore_2_ { "[\013-\037]" };
    static inline const QRegularExpression regex_ignore_3_ { "[\177-\377]" };

    static inline const QRegularExpression regex_eol_ { "\n" };
    static inline const QRegularExpression regex_tab_ { "\t" };
    static inline const QRegularExpression regex_space_ { " " };
    static inline const QRegularExpression regex_discard_color_ { "\\[[0-9]m" };
    static inline const QRegularExpression regex_red_start_ { "\\[31;40m" };
    static inline const QRegularExpression regex_green_start_ { "\\[32;40m" };
    static inline const QRegularExpression regex_yellow_start_ { "\\[33;40m" };
    static inline const QRegularExpression regex_color_end_ { "\\[37;40m" };

    static constexpr std::string_view HTML_COLOR_RED_START_ { "<span style=\"color:red\">" };
    static constexpr std::string_view HTML_COLOR_GREEN_START_ { "<span style=\"color:green\">" };
    static constexpr std::string_view HTML_COLOR_YELLOW_START_ { "<span style=\"color:yellow\">" };
    static constexpr std::string_view HTML_COLOR_END_ { "</span>" };
    static constexpr std::string_view HTML_EOL_ { "<br />" };
    static constexpr std::string_view HTML_SPACE_ { "&nbsp;" };
    static constexpr std::string_view HTML_TAB_ { "&nbsp;&nbsp;" };

    QQmlApplicationEngine* p_engine_;
    ConnectionManagerV2& conn_manager_;
    QObject* p_console_;
    ConnectButton* p_cmd_button_;

public:
    BotConsole(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval);

    void register_buttons();
};
