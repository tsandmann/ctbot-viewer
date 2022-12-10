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
 * @file    script_editor.h
 * @brief   Script editor component
 * @author  Timo Sandmann
 * @date    12.04.2020
 */

#pragma once

#include "connect_button.h"


class QQmlApplicationEngine;
class QTcpSocket;

class ScriptEditor {
    QQmlApplicationEngine* p_engine_;
    QTcpSocket* p_socket_;
    QObject* p_script_;
    QObject* p_editor_;
    QObject* p_type_;
    QObject* p_execute_;
    QObject* p_filename_;

    ConnectButton* p_load_button_;
    ConnectButton* p_save_button_;
    ConnectButton* p_send_button_;
    ConnectButton* p_abort_button_;

public:
    ScriptEditor(QQmlApplicationEngine* p_engine, QTcpSocket* p_socket);

    ~ScriptEditor();

    void register_buttons();
};
