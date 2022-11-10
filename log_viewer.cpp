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
 * @file    log_viewer.cpp
 * @brief   Log viewer component
 * @author  Timo Sandmann
 * @date    12.04.2020
 */

#include <QQmlApplicationEngine>

#include "log_viewer.h"
#include "connection_manager.h"


LogViewerV1::LogViewerV1(QQmlApplicationEngine* p_engine, ConnectionManagerV1& command_eval) : p_engine_ { p_engine }, p_log_ {}, p_minilog_ {} {
    command_eval.register_cmd(ctbot::CommandCodes::CMD_LOG, [this, &command_eval](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_LOG received: " << cmd << "\n";

        if (command_eval.get_version() != command_eval.version_active()) {
            return false;
        }

        auto root { p_engine_->rootObjects() };
        if (!p_log_) {
            p_log_ = root.first()->findChild<QObject*>("log_viewer");
        }
        if (!p_minilog_) {
            p_minilog_ = root.first()->findChild<QObject*>("mini_log_viewer");
        }

        if (!p_log_ && !p_minilog_) {
            return false;
        }

        QString data { QString::fromUtf8(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int>(cmd.get_payload_size())) };
        data.replace(regex_replace_0_, ".");
        data.replace(regex_replace_1_, ".");
        data.replace(regex_replace_2_, "#");
        data.append("\r\n");

        if (p_log_) {
            QMetaObject::invokeMethod(p_log_, "add", Qt::DirectConnection, Q_ARG(QVariant, data));
        }

        if (p_minilog_) {
            QMetaObject::invokeMethod(p_minilog_, "add", Qt::DirectConnection, Q_ARG(QVariant, data));
            const auto pos { p_minilog_->property("length").toInt() };
            p_minilog_->setProperty("cursorPosition", pos + 1);
        }

        return true;
    });
}


LogViewerV2::LogViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval) : p_engine_ { p_engine }, p_log_ {}, p_minilog_ {} {
    command_eval.register_cmd("log", [this, &command_eval](const std::string_view& str) {
        if (command_eval.get_version() != command_eval.version_active()) {
            return false;
        }

        auto root { p_engine_->rootObjects() };
        if (!p_log_) {
            p_log_ = root.first()->findChild<QObject*>("log_viewer");
        }
        if (!p_minilog_) {
            p_minilog_ = root.first()->findChild<QObject*>("mini_log_viewer");
        }

        if (!p_log_ && !p_minilog_) {
            return false;
        }

        std::string input { str };

        if (DEBUG_) {
            qDebug() << "LogViewerV2: input=" << QString::fromStdString(input);
        }

        QString data { QString::fromStdString(input) };
        data.replace(regex_replace_0_, ".");
        data.replace(regex_replace_1_, ".");
        data.replace(regex_replace_2_, "#");

        if (p_log_) {
            QMetaObject::invokeMethod(p_log_, "add", Qt::DirectConnection, Q_ARG(QVariant, data));
        }

        if (p_minilog_) {
            QMetaObject::invokeMethod(p_minilog_, "add", Qt::DirectConnection, Q_ARG(QVariant, data));
            const auto pos { p_minilog_->property("length").toInt() };
            p_minilog_->setProperty("cursorPosition", pos + 1);
        }

        return true;
    });
}
