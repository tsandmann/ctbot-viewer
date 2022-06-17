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

#include <QString>
#include <QRegularExpression>

#include <regex>

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

        static const QRegularExpression regex1 { QRegularExpression("[\001-\011]") };
        static const QRegularExpression regex2 { QRegularExpression("[\013-\037]") };
        static const QRegularExpression regex3 { QRegularExpression("[\177-\377]") };

        QString data { QString::fromUtf8(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int>(cmd.get_payload_size())) };
        data.replace(regex1, ".");
        data.replace(regex2, ".");
        data.replace(regex3, "#");

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


LogViewerV2::LogViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval) : p_engine_ { p_engine }, p_log_ {}, p_minilog_ {} {
    command_eval.register_cmd("log", [this, &command_eval](const std::string_view& str) {
        static const std::regex log_regex { R"((.*\r?\n?)(</log>\r\n<log>\r\n))" };

        static const QRegularExpression regex1 { QRegularExpression("[\001-\007]") }; // FIXME: use std::regex only
        static const QRegularExpression regex2 { QRegularExpression("[\016-\037]") };
        static const QRegularExpression regex3 { QRegularExpression("[\177-\377]") };

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

        std::match_results<std::string::const_iterator> matches;
        while (std::regex_search(input, matches, log_regex)) {
            // qDebug() << "LogViewerV2: Match found:";
            // for (size_t i { 0 }; i < matches.size(); ++i) {
            //     qDebug() << i << ":" << QString::fromStdString(matches[i].str());
            // }

            QString data { QString::fromStdString(matches[1].str()) };
            data.replace(regex1, ".");
            data.replace(regex2, ".");
            data.replace(regex3, "#");

            if (p_log_) {
                QMetaObject::invokeMethod(p_log_, "add", Qt::DirectConnection, Q_ARG(QVariant, data));
            }

            if (p_minilog_) {
                QMetaObject::invokeMethod(p_minilog_, "add", Qt::DirectConnection, Q_ARG(QVariant, data));
                const auto pos { p_minilog_->property("length").toInt() };
                p_minilog_->setProperty("cursorPosition", pos + 1);
            }

            input.erase(0, matches[0].str().length());
            // qDebug() << "\t" << QString::fromStdString(input);
        }

        QString data { QString::fromStdString(input) };
        data.replace(regex1, ".");
        data.replace(regex2, ".");
        data.replace(regex3, "#");

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