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
 * @file    remotecontrol_viewer.h
 * @brief   IR remote control component
 * @author  Timo Sandmann
 * @date    12.04.2020
 */

#pragma once

#include <QString>

#include <cstdint>
#include <map>


class QQmlApplicationEngine;
class ConnectButton;
class ConnectionManagerV1;
class ConnectionManagerV2;

class RemoteControlViewerV1 {
    QQmlApplicationEngine* p_engine_;
    ConnectionManagerV1& conn_manager_;
    ConnectButton* p_rc_button_;
    std::map<QString /*RC-Code*/, uint16_t /*CMD-Code*/> rc5_codes_;

public:
    RemoteControlViewerV1(QQmlApplicationEngine* p_engine, ConnectionManagerV1& conn_manager);

    ~RemoteControlViewerV1();

    void register_buttons();
};


class RemoteControlViewerV2 {
    QQmlApplicationEngine* p_engine_;
    ConnectionManagerV2& conn_manager_;
    ConnectButton* p_rc_button_;
    std::map<QString /*RC-Code*/, uint8_t /*CMD-Code*/> rc5_codes_;

public:
    RemoteControlViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& conn_manager);

    ~RemoteControlViewerV2();

    void register_buttons();
};
