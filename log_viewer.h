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
 * @file    log_viewer.h
 * @brief   Log viewer component
 * @author  Timo Sandmann
 * @date    12.04.2020
 */

#pragma once

#include "connect_button.h"


class QQmlApplicationEngine;
class ConnectionManagerV1;
class ConnectionManagerV2;

class LogViewerV1 {
    QQmlApplicationEngine* p_engine_;
    QObject* p_log_;
    QObject* p_minilog_;

public:
    LogViewerV1(QQmlApplicationEngine* p_engine, ConnectionManagerV1& command_eval);
};

class LogViewerV2 {
    static constexpr bool DEBUG_ { false };

    QQmlApplicationEngine* p_engine_;
    QObject* p_log_;
    QObject* p_minilog_;

public:
    LogViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval);
};
