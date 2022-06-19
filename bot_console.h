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

#include <deque>

#include <QString>

#include "connect_button.h"


class QQmlApplicationEngine;
class ConnectionManagerV2;

class BotConsole {
    static constexpr size_t HISTORY_SIZE_ { 30 };

    QQmlApplicationEngine* p_engine_;
    ConnectionManagerV2& conn_manager_;
    QObject* p_console_;
    ConnectButton* p_cmd_button_;
    ConnectButton* p_key_pressed_;
    std::deque<QString> history_;
    size_t current_history_view_;

public:
    BotConsole(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval);

    void register_buttons();
};
