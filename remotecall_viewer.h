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
 * @file    remotecall_viewer.h
 * @brief   RemoteCall viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#pragma once

#include <QList>
#include <QString>

#include "remotecall_model.h"
#include "remotecall_list.h"
#include "connect_button.h"


class QQmlApplicationEngine;
class QTcpSocket;
class QQuickItem;
class ConnectionManagerV1;

class RemotecallViewer {
    RCList* p_rcList_;
    RCModel rc_model_;
    QQmlApplicationEngine* p_engine_;
    QTcpSocket* p_socket_;
    QObject* p_rc_viewer_;
    QObject* p_current_label_;
    ConnectButton* p_fetch_button_;
    ConnectButton* p_clear_button_;
    ConnectButton* p_abort_button_;
    ConnectButton* p_rc_button_;

    static QQuickItem* find_item(const QList<QObject*>& nodes, const QString& name);

public:
    RemotecallViewer(QQmlApplicationEngine* p_engine, ConnectionManagerV1& command_eval);

    ~RemotecallViewer();

    void register_buttons();
};
