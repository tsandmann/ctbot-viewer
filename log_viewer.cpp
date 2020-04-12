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
 * @file    log_viewer.cpp
 * @brief   Log viewer component
 * @author  Timo Sandmann
 * @date    12.04.2020
 */

#include <QString>
#include <QRegularExpression>

#include "log_viewer.h"
#include "connection_manager.h"


LogViewer::LogViewer(QQmlApplicationEngine* p_engine, ConnectionManager& command_eval) : p_engine_ { p_engine }, p_log_ {}, p_minilog_ {} {
    command_eval.register_cmd(ctbot::CommandCodes::CMD_LOG, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_LOG received: " << cmd << "\n";

        if (!p_log_) {
            p_log_ = p_engine_->rootObjects().first()->findChild<QObject*>("log_viewer");
        }
        if (!p_minilog_) {
            p_minilog_ = p_engine_->rootObjects().first()->findChild<QObject*>("mini_log_viewer");
        }

        if (!p_log_ && !p_minilog_) {
            return false;
        }

        QString data { QString::fromUtf8(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int>(cmd.get_payload_size())) };
        data.replace(QRegularExpression("[\001-\011]"), ".");
        data.replace(QRegularExpression("[\013-\037]"), ".");
        data.replace(QRegularExpression("[\177-\377]"), "#");

        if (p_log_) {
            QMetaObject::invokeMethod(p_log_, "append", Qt::DirectConnection, Q_ARG(QString, data));
        }

        if (p_minilog_) {
            const auto pos { p_minilog_->property("length").toInt() };
            QMetaObject::invokeMethod(p_minilog_, "append", Qt::DirectConnection, Q_ARG(QString, data));
            p_minilog_->setProperty("cursorPosition", pos + 1);
        }

        return true;
    });
}
