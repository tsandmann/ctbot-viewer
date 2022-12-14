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
#include <tuple>


class ConnectionManagerV2;

class SystemViewerV2 : public ValueViewer {
protected:
    static constexpr std::string_view TASK_RE_ { R"(task:(\-?\d+):([[:print:]]+):(\d*\.\d*))" };
    static constexpr std::string_view RAM_RE_ {
        R"(ram:(\d):(\d+):(\d+):?(\d*):?(\d*):?(\d*))"
    }; // id=1, size, data used, bss used, heap used, itcm; id=2, size, used; id=3, size, used

    QObject* p_cpu_util_;
    float last_cpu_util_;
    QObject* p_ram_util_;
    std::tuple<size_t, size_t, size_t, size_t, size_t> last_ram1_; // size, itcm, data, bss, heap
    std::tuple<size_t, size_t> last_ram2_; // size, bss
    std::tuple<size_t, size_t> last_ram3_; // size, bss

    static inline const std::regex task_regex_ { TASK_RE_.cbegin() };
    static inline const std::regex ram_regex_ { RAM_RE_.cbegin() };

    bool parse(const std::string_view& str, const std::regex& regex, int32_t& id, QString& name, float& value) const;
    bool parse(const std::string_view& str, const std::regex& regex, int32_t& id, size_t& v1, size_t& v2, size_t& v3, size_t& v4, size_t& v5) const;

public:
    SystemViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval);

    void init();
};
