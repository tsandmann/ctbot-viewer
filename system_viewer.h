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
 * @file    system_viewer.h
 * @brief   System data viewer component
 * @author  Timo Sandmann
 * @date    09.12.2022
 */

#pragma once

#include "value_viewer.h"

#include <QString>
#include <QRegularExpression>

#include <string_view>
#include <regex>


class ConnectionManagerV2;

class SystemViewerV2 : public ValueViewer {
protected:
    static constexpr std::string_view TASK_RE_ { R"(task:(\-?\d+):([[:print:]]+):(\d*\.\d*))" };
    static constexpr std::string_view TASK_IDLE_ { "IDLE [%]" };
    static constexpr std::string_view TASK_MAIN_ { "main [%]" };

    QObject* p_cpu_util_;
    QObject* p_cpu_util_bar_;

    static inline const std::regex task_regex_ { TASK_RE_.cbegin() };

    bool parse(const std::string_view& str, const std::regex& regex, int32_t& id, QString& name, float& value) const;

public:
    SystemViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval);

    void init();
};
