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
 * @file    bot_console.cpp
 * @brief   Bot console component
 * @author  Timo Sandmann
 * @date    06.06.2022
 */

#include <QQmlApplicationEngine>
#include <QDebug>

#include "bot_console.h"
#include "connection_manager.h"


BotConsole::BotConsole(QQmlApplicationEngine* p_engine, ConnectionManagerV2& command_eval)
    : p_engine_ { p_engine }, conn_manager_ { command_eval }, p_console_ {}, p_cmd_button_ {}, p_active_switch_ {} {
    conn_manager_.register_cmd("", [this](const std::string_view& str) {
        if constexpr (DEBUG_) {
            qDebug() << "CONSOLE received: " << QString::fromUtf8(str.data(), str.size());
        }

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

        QString data { QString::fromUtf8(str.data(), str.size()) };
        data.remove(regex_clear_);
        data.replace(regex_tab_, HTML_TAB_.cbegin());
        data.replace(regex_space_, HTML_SPACE_.cbegin());
        data.remove(regex_ignore_0_);
        data.remove(regex_ignore_1_);
        data.remove(regex_ignore_2_);
        data.remove(regex_ignore_3_);
        data.remove(regex_discard_color_);

        data.replace(regex_red_start_, HTML_COLOR_RED_START_.cbegin());
        data.replace(regex_green_start_, HTML_COLOR_GREEN_START_.cbegin());
        data.replace(regex_yellow_start_, HTML_COLOR_YELLOW_START_.cbegin());
        data.replace(regex_color_end_, HTML_COLOR_END_.cbegin());
        data.replace(regex_eol_, HTML_EOL_.cbegin());

        if (data.length() < 1) {
            return false;
        }
        if constexpr (DEBUG_) {
            qDebug() << "CONSOLE data= " << data;
        }

        QMetaObject::invokeMethod(p_console_, "add", Qt::DirectConnection, Q_ARG(QVariant, data));

        return true;
    });
}

void BotConsole::register_buttons() {
    p_cmd_button_ = new ConnectButton { [this](QString cmd, QString) {
        if (p_console_) {
            QMetaObject::invokeMethod(p_console_, "add", Qt::DirectConnection, Q_ARG(QVariant, "% "));
        }

        cmd += "\r\n";
        if (conn_manager_.get_socket()->isOpen()) {
            conn_manager_.get_socket()->write(cmd.toUtf8(), cmd.length());
        }

        if constexpr (DEBUG_) {
            qDebug() << "CMD sent: " << cmd;
        }
    } };
    QObject::connect(p_engine_->rootObjects().at(0)->findChild<QObject*>("Cmd"), SIGNAL(sendClicked(QString)), p_cmd_button_, SLOT(cppSlot(QString)));

    p_active_switch_ = new ConnectButton { [this](QString state, QString) {
        if (conn_manager_.get_socket()->isOpen()) {
            const QString cmd { "c viewer " + state + "\r\n" };
            conn_manager_.get_socket()->write(cmd.toUtf8(), cmd.length());
        }
    } };
    QObject::connect(
        p_engine_->rootObjects().at(0)->findChild<QObject*>("viewer_active"), SIGNAL(activeChanged(QString)), p_active_switch_, SLOT(cppSlot(QString)));
}
