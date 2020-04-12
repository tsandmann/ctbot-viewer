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
 * @file    map_viewer.h
 * @brief   Map viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#pragma once

#include <QQmlApplicationEngine>
#include <cstdint>

#include "connect_button.h"


class QTcpSocket;
class ConnectionManager;
class MapImageItem;

class MapViewer {
    QQmlApplicationEngine* p_engine_;
    QTcpSocket* p_socket_;
    ConnectButton* p_fetch_button_;
    ConnectButton* p_clear_button_;
    ConnectButton* p_save_button_;
    MapImageItem* p_map_;
    unsigned receive_state_;
    uint16_t last_block_;

public:
    MapViewer(QQmlApplicationEngine* p_engine, ConnectionManager& command_eval);

    ~MapViewer();

    void register_buttons();
};
