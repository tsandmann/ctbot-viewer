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
 * @file    system_viewer.cpp
 * @brief   System data viewer component
 * @author  Timo Sandmann
 * @date    09.12.2022
 */

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

#include <charconv>
#include <cstring>
#include <clocale>
#include <iostream>

#include "system_viewer.h"
#include "connection_manager.h"
#include "command.h"


SystemViewerV2::SystemViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval)
    : ValueViewer { p_engine }, p_cpu_util_ {}, p_cpu_util_bar_ {} {
    qmlRegisterType<ValueModel>("SystemData", 1, 0, "SystemDataModel");
    qmlRegisterUncreatableType<ValueList>("SystemData", 1, 0, "ValueList", QStringLiteral("SystemData should not be created in QML"));

    update_map();
    register_model(QStringLiteral("systemModelV2"));

    command_eval.register_cmd("sys", [this, &command_eval](const std::string_view& str) {
        static float cpu_util { -1.f };

        if (command_eval.get_version() != command_eval.version_active()) {
            return false;
        }

        if (!str.length()) {
            return false;
        }

        int32_t id;
        QString task_name;
        float task_util;
        if (parse(str, task_regex_, id, task_name, task_util)) {
            // qDebug() << "task " << id << task_name << ":" << task_util;
            if (id == -1) {
                model_.sort();
                if (p_cpu_util_ && p_cpu_util_bar_) {
                    const auto bar_width { p_cpu_util_bar_->property("width").toFloat() };
                    p_cpu_util_->setProperty("width", cpu_util * bar_width / 100.f);
                }
            } else if (id == 0) {
                cpu_util = 100.f - task_util;
            } else {
                QString id_string { QString::number(id) };
                if (!map_.contains(id_string)) {
                    list_.appendItem(id_string);
                    update_map();
                }
                model_.setData(map_[id_string], task_name, ValueModel::Name);
                model_.setData(map_[id_string], task_util, ValueModel::Value);
            }
        }
        return true;
    });
}

void SystemViewerV2::init() {
    auto root { p_engine_->rootObjects() };
    p_cpu_util_ = root.first()->findChild<QObject*>("cpu_util");
    p_cpu_util_bar_ = root.first()->findChild<QObject*>("cpu_util_bar");
}

bool SystemViewerV2::parse(const std::string_view& str, const std::regex& regex, int32_t& id, QString& name, float& value) const {
    std::match_results<std::string_view::const_iterator> matches;
    if (std::regex_search(str.cbegin(), str.cend(), matches, regex)) {
        const char* old_locale { std::setlocale(LC_ALL, nullptr) };
        std::setlocale(LC_ALL, "C");
        auto [ptr, ec] { std::from_chars(matches[1].str().c_str(), matches[1].str().c_str() + matches[1].str().size(), id) };
        if (ec != std::errc()) {
            return false;
        }
        name = QString::fromStdString(matches[2].str());
        value = std::strtof(matches[3].str().c_str(), nullptr);
        std::setlocale(LC_ALL, old_locale);
    } else {
        return false;
    }

    return true;
}
