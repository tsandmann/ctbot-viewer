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
    : ValueViewer { p_engine }, p_cpu_util_ {}, last_cpu_util_ { -1.f }, p_ram_util_ {}, last_ram1_ {}, last_ram2_ {}, last_ram3_ {} {
    qmlRegisterType<ValueModel>("SystemData", 1, 0, "SystemDataModel");
    qmlRegisterUncreatableType<ValueList>("SystemData", 1, 0, "ValueList", QStringLiteral("SystemData should not be created in QML"));

    update_map();
    register_model(QStringLiteral("systemModelV2"));

    command_eval.register_cmd("sys", [this, &command_eval](const std::string_view& str) {
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
                if (p_cpu_util_ && last_cpu_util_ >= 0.f) {
                    QMetaObject::invokeMethod(p_cpu_util_, "set_cpu", Q_ARG(QVariant, last_cpu_util_));
                    // p_cpu_util_->setProperty("width", cpu_util * cpu_bar_width_ / 100.f);
                }
            } else if (id == 0) {
                last_cpu_util_ = 100.f - task_util;
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

        size_t size, data_used, bss_used {}, heap_used {}, itcm {};
        if (parse(str, ram_regex_, id, size, data_used, bss_used, heap_used, itcm)) {
            // qDebug() << "RAM" << id << ": size=" << size << "data=" << data_used << "bss=" << bss_used << "heap=" << heap_used << "itcm=" << itcm;
            switch (id) {
                case 1: {
                    auto& [rsize, ritcm, data, bss, heap] { last_ram1_ };
                    rsize = size;
                    ritcm = itcm;
                    data = data_used;
                    bss = bss_used;
                    heap = heap_used;
                    if (p_ram_util_) {
                        QMetaObject::invokeMethod(p_ram_util_, "set_ram1", Qt::DirectConnection, Q_ARG(QVariant, static_cast<float>(size)),
                            Q_ARG(QVariant, static_cast<float>(itcm)), Q_ARG(QVariant, static_cast<float>(data_used)),
                            Q_ARG(QVariant, static_cast<float>(bss_used)), Q_ARG(QVariant, static_cast<float>(heap_used)));
                    }
                    break;
                }

                case 2: {
                    auto& [rsize, bss] { last_ram2_ };
                    rsize = size;
                    bss = data_used;
                    if (p_ram_util_) {
                        QMetaObject::invokeMethod(p_ram_util_, "set_ram2", Qt::DirectConnection, Q_ARG(QVariant, static_cast<float>(size)),
                            Q_ARG(QVariant, static_cast<float>(data_used)));
                    }
                    break;
                }

                case 3: {
                    auto& [rsize, bss] { last_ram3_ };
                    rsize = size;
                    bss = data_used;
                    if (p_ram_util_) {
                        QMetaObject::invokeMethod(p_ram_util_, "set_ram3", Qt::DirectConnection, Q_ARG(QVariant, static_cast<float>(size)),
                            Q_ARG(QVariant, static_cast<float>(data_used)));
                    }
                    break;
                }
            }
        }

        return true;
    });
}

void SystemViewerV2::init() {
    auto root { p_engine_->rootObjects() };
    p_cpu_util_ = root.first()->findChild<QObject*>("cpu_util");
    p_ram_util_ = root.first()->findChild<QObject*>("ram_util");
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

bool SystemViewerV2::parse(
    const std::string_view& str, const std::regex& regex, int32_t& id, size_t& v1, size_t& v2, size_t& v3, size_t& v4, size_t& v5) const {
    std::match_results<std::string_view::const_iterator> matches;
    if (std::regex_search(str.cbegin(), str.cend(), matches, regex)) {
        const char* old_locale { std::setlocale(LC_ALL, nullptr) };
        std::setlocale(LC_ALL, "C");
        {
            auto [ptr, ec] { std::from_chars(matches[1].str().c_str(), matches[1].str().c_str() + matches[1].str().size(), id) };
            if (ec != std::errc()) {
                return false;
            }
        }
        {
            auto [ptr, ec] { std::from_chars(matches[2].str().c_str(), matches[2].str().c_str() + matches[2].str().size(), v1) };
            if (ec != std::errc()) {
                return false;
            }
        }
        {
            auto [ptr, ec] { std::from_chars(matches[3].str().c_str(), matches[3].str().c_str() + matches[3].str().size(), v2) };
            if (ec != std::errc()) {
                return false;
            }
        }
        std::from_chars(matches[4].str().c_str(), matches[4].str().c_str() + matches[4].str().size(), v3);
        std::from_chars(matches[5].str().c_str(), matches[5].str().c_str() + matches[5].str().size(), v4);
        std::from_chars(matches[6].str().c_str(), matches[6].str().c_str() + matches[6].str().size(), v5);

        std::setlocale(LC_ALL, old_locale);
    } else {
        return false;
    }

    return true;
}
