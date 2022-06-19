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
 * @file    value_viewer.h
 * @brief   Value viewer abstraction
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#pragma once

#include <QString>
#include <QHash>
#include <QModelIndex>

#include <regex>

#include "value_model.h"
#include "value_list.h"


class QQmlApplicationEngine;

class ValueViewer {
protected:
    QQmlApplicationEngine* p_engine_;
    ValueList list_;
    ValueModel model_;
    QHash<QString, QModelIndex> map_;

    void update_map();
    void register_model(const QString& modelname);
    bool parse(const std::string_view& str, const std::regex& regex, int16_t& value) const;
    bool parse(const std::string_view& str, const std::regex& regex, int16_t& left, int16_t& right) const;
    bool parse(const std::string_view& str, const std::regex& regex, float& value) const;
    bool parse(const std::string_view& str, const std::regex& regex, float& value1, float& value2) const;

public:
    ValueViewer(QQmlApplicationEngine* p_engine);
};
