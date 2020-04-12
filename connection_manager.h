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
 * @file    connection_manager.h
 * @brief   Connection and ct-Bot command management
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#pragma once

#include <QQmlApplicationEngine>
#include <QTcpSocket>
#include <QByteArray>
#include <map>
#include <vector>
#include <functional>

#include "command.h"
#include "connect_button.h"


class ConnectionManager {
    QQmlApplicationEngine* p_engine_;
    QTcpSocket socket_;
    QByteArray in_buffer_;
    bool connected_;
    ConnectButton* p_connect_button_;
    std::map<ctbot::CommandCodes /*cmd*/, std::vector<std::function<bool(const ctbot::CommandBase&)>> /*functions*/> commands_;

protected:
    bool evaluate_cmd(const ctbot::CommandNoCRC* p_cmd) const;

public:
    ConnectionManager(QQmlApplicationEngine* p_engine);

    ~ConnectionManager();

    void register_buttons();

    void register_cmd(const ctbot::CommandCodes& cmd, std::function<bool(const ctbot::CommandBase&)>&& func);

    auto get_socket() {
        return &socket_;
    }
};
