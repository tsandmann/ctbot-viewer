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
 * @file    bot_console.cpp
 * @brief   Bot console component
 * @author  Timo Sandmann
 * @date    06.06.2022
 */

#include <QQmlApplicationEngine>
#include <QString>
#include <QRegularExpression>
#include <QDebug>

#include "bot_console.h"
#include "connection_manager.h"


BotConsole::BotConsole(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval)
    : p_engine_ { p_engine }, conn_manager_ { command_eval }, p_console_ {}, p_cmd_button_ {}, p_key_pressed_ {}, current_history_view_ {} {
    conn_manager_.register_cmd("", [this](const std::string_view& str) {
        // qDebug() << "CONSOLE received: " << QString::fromUtf8(str.data(), str.size());

        if (conn_manager_.get_version() != conn_manager_.version_active()) {
            return false;
        }

        auto root { p_engine_->rootObjects() };
        if (!p_console_) {
            p_console_ = root.first()->findChild<QObject*>("bot_console");
        }

        if (!p_console_) {
            return false;
        }

        static const QRegularExpression regex0 { QRegularExpression("\377..") };
        static const QRegularExpression regex1 { QRegularExpression("[\001-\011]") };
        static const QRegularExpression regex2 { QRegularExpression("[\013-\037]") };
        static const QRegularExpression regex3 { QRegularExpression("[\177-\377]") };

        static const QRegularExpression regex_eol { QRegularExpression("\n") };
        static const QRegularExpression regex_tab { QRegularExpression("\t") };
        static const QRegularExpression regex_space { QRegularExpression(" ") };
        static const QRegularExpression regex_discard { QRegularExpression("\\[[0-9]m") };
        static const QRegularExpression regex_red_start { QRegularExpression("\\[31;40m") };
        static const QRegularExpression regex_green_start { QRegularExpression("\\[32;40m") };
        static const QRegularExpression regex_yellow_start { QRegularExpression("\\[33;40m") };
        static const QRegularExpression regex_color_end { QRegularExpression("\\[37;40m") };

        QString data { QString::fromUtf8(str.data(), str.size()) };
        data.replace(regex_tab, "&nbsp;&nbsp;");
        data.replace(regex_space, "&nbsp;");
        data.remove(regex0);
        data.remove(regex1);
        data.remove(regex2);
        data.remove(regex3);
        data.remove(regex_discard);

        data.replace(regex_red_start, "<span style=\"color:red\">");
        data.replace(regex_green_start, "<span style=\"color:green\">");
        data.replace(regex_yellow_start, "<span style=\"color:yellow\">");
        data.replace(regex_color_end, "</span>");
        data.replace(regex_eol, "<br />");

        if (data.length() < 1) {
            return false;
        }
        // qDebug() << "CONSOLE received: " << data;

        QMetaObject::invokeMethod(p_console_, "add", Qt::DirectConnection, Q_ARG(QVariant, data));

        return true;
    });
}

void BotConsole::register_buttons() {
    p_cmd_button_ = new ConnectButton { [this](QString cmd, QString) {
        if (p_console_) {
            QMetaObject::invokeMethod(p_console_, "add", Qt::DirectConnection, Q_ARG(QVariant, "% "));
        }

        if (history_.empty() || history_.front() != cmd) {
            history_.emplace_front(cmd);
            if (history_.size() > HISTORY_SIZE_) {
                history_.pop_back();
            }
        }
        current_history_view_ = 0;

        cmd += "\r\n";
        if (conn_manager_.get_socket()->isOpen()) {
            conn_manager_.get_socket()->write(cmd.toUtf8(), cmd.length());
        }

        // qDebug() << "CMD sent: " << cmd;
    } };
    QObject::connect(p_engine_->rootObjects().at(0)->findChild<QObject*>("Cmd"), SIGNAL(sendClicked(QString)), p_cmd_button_, SLOT(cppSlot(QString)));

    p_key_pressed_ = new ConnectButton { [this](QString key, QString) {
        // qDebug() << "Key pressed: " << key;

        auto cmd_field { p_engine_->rootObjects().at(0)->findChild<QObject*>("Cmd") };
        if (key == "up" && history_.size()) {
            const auto cmd { history_[current_history_view_++] };
            if (current_history_view_ == history_.size()) {
                --current_history_view_;
            }
            // qDebug() << "HISTORY cmd=" << cmd;

            if (cmd_field) {
                cmd_field->setProperty("text", cmd);
            }
        } else if (key == "down") {
            if (current_history_view_) {
                const auto cmd { history_[--current_history_view_] };
                // qDebug() << "HISTORY cmd=" << cmd;

                if (cmd_field) {
                    cmd_field->setProperty("text", cmd);
                }
            } else if (cmd_field) {
                cmd_field->setProperty("text", "");
            }
        }
    } };
    QObject::connect(p_engine_->rootObjects().at(0)->findChild<QObject*>("Cmd"), SIGNAL(keyPressed(QString)), p_key_pressed_, SLOT(cppSlot(QString)));
}
