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
 * @file    actuator_viewer.h
 * @brief   Actuator viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#pragma once

#include "value_viewer.h"


class ConnectionManager;

class ActuatorViewer : public ValueViewer {
    QObject* p_lcd_;

public:
    ActuatorViewer(QQmlApplicationEngine* p_engine, ConnectionManager& command_eval);
};
