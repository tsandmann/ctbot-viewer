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

#include <QQmlContext>

#include <charconv>
#include <cstdlib>
#include <clocale>

#include "value_viewer.h"


ValueViewer::ValueViewer(QQmlApplicationEngine* p_engine) : p_engine_ { p_engine } {
    model_.setList(&list_);
}

void ValueViewer::update_map() {
    for (int i {}; i < model_.rowCount(); ++i) {
        const auto& e { model_.data(model_.index(i, 0), ValueModel::Name) };
        map_[e.toString()] = model_.index(i, 0);
    }
}

void ValueViewer::register_model(const QString& modelname) {
    p_engine_->rootContext()->setContextProperty(modelname, &model_);
}

bool ValueViewer::parse(const std::string_view& str, const std::regex& regex, int16_t& value) const {
    std::match_results<std::string_view::const_iterator> matches;
    if (std::regex_search(str.cbegin(), str.cend(), matches, regex)) {
        auto [ptr, ec] { std::from_chars(matches[1].str().c_str(), matches[1].str().c_str() + matches[1].str().size(), value) };
        if (ec != std::errc()) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool ValueViewer::parse(const std::string_view& str, const std::regex& regex, int16_t& left, int16_t& right) const {
    std::match_results<std::string_view::const_iterator> matches;
    if (std::regex_search(str.cbegin(), str.cend(), matches, regex)) {
        {
            auto [ptr, ec] { std::from_chars(matches[1].str().c_str(), matches[1].str().c_str() + matches[1].str().size(), left) };
            if (ec != std::errc()) {
                return false;
            }
        }
        {
            auto [ptr, ec] { std::from_chars(matches[2].str().c_str(), matches[2].str().c_str() + matches[2].str().size(), right) };
            if (ec != std::errc()) {
                return false;
            }
        }
    } else {
        return false;
    }

    return true;
}

bool ValueViewer::parse(const std::string_view& str, const std::regex& regex, float& value) const {
    std::match_results<std::string_view::const_iterator> matches;
    if (std::regex_search(str.cbegin(), str.cend(), matches, regex)) {
        const char* old_locale { std::setlocale(LC_ALL, nullptr) };
        std::setlocale(LC_ALL, "C");
        value = std::strtof(matches[1].str().c_str(), nullptr);
        std::setlocale(LC_ALL, old_locale);
    } else {
        return false;
    }

    return true;
}

bool ValueViewer::parse(const std::string_view& str, const std::regex& regex, float& value1, float& value2) const {
    std::match_results<std::string_view::const_iterator> matches;
    if (std::regex_search(str.cbegin(), str.cend(), matches, regex)) {
        const char* old_locale { std::setlocale(LC_ALL, nullptr) };
        std::setlocale(LC_ALL, "C");
        value1 = std::strtof(matches[1].str().c_str(), nullptr);
        value2 = std::strtof(matches[2].str().c_str(), nullptr);
        std::setlocale(LC_ALL, old_locale);
    } else {
        return false;
    }

    return true;
}
