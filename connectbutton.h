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
 * @file    connectbutton.h
 * @brief   QML button abstraction for use with lambda onclick-handler
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#pragma once

#include <QObject>
#include <QString>
#include <functional>


class ConnectButton : public QObject {
    Q_OBJECT
    std::function<void(QString)> func_;

public:
    explicit ConnectButton(std::function<void(QString)> func, QObject* = nullptr) : func_ { func } {}

public slots:
    void cppSlot(QString msg);
};
