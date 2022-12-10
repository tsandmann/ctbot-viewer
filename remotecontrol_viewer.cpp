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
 * @file    remotecontrol_viewer.cpp
 * @brief   IR remote control component
 * @author  Timo Sandmann
 * @date    12.04.2020
 */

#include <QQmlApplicationEngine>
#include <QTcpSocket>

#include "remotecontrol_viewer.h"
#include "connection_manager.h"
#include "connect_button.h"
#include "command.h"


RemoteControlViewerV1::RemoteControlViewerV1(QQmlApplicationEngine* p_engine, ConnectionManagerV1& conn_manager)
    : p_engine_ { p_engine }, conn_manager_ { conn_manager }, p_rc_button_ {}, rc5_codes_ { { "Power", 0x118c }, { "1", 0x1181 }, { "2", 0x1182 },
          { "3", 0x1183 }, { "4", 0x1184 }, { "5", 0x1185 }, { "6", 0x1186 }, { "7", 0x1187 }, { "8", 0x1188 }, { "9", 0x1189 }, { "10", 0x1180 },
          { "11", 0x118a }, { "12", 0x11a3 }, { "Gr", 0x01ba }, { "Re", 0x01bd }, { "Ye", 0x01b1 }, { "Bl", 0x01b0 }, { "I/II", 0x11ab }, { "TV", 0x11b8 },
          { "Gr", 0x01ba }, { "||", 0x11a9 }, { "<<", 0x11b2 }, { "\u25B6", 0x11b5 }, { ">>", 0x11b4 }, { "\u2B1B", 0x11b6 }, { "\u25CF", 0x11b7 },
          { "CH*P/C", 0x11bf }, { "V+", 0x1190 }, { "M", 0x01bf }, { "C+", 0x11a0 }, { "V-", 0x1191 }, { "C-", 0x11a1 } } {}

RemoteControlViewerV1::~RemoteControlViewerV1() {
    delete p_rc_button_;
}

void RemoteControlViewerV1::register_buttons() {
    p_rc_button_ = new ConnectButton { [this](QString button, QString) {
        if (conn_manager_.get_version() != conn_manager_.version_active()) {
            return;
        }

        const auto it { rc5_codes_.find(button) };
        if (it == rc5_codes_.end()) {
            qDebug() << "RemoteControlViewerV1: unknown RC button pressed.";
            return;
        }

        const auto rc5code { static_cast<int16_t>(it->second) };
        // qDebug() << "RemoteControlViewerV1: button " << button << "pressed, code =" << rc5code;

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_SENS_RC5, ctbot::CommandCodes::CMD_SUB_NORM, rc5code, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };

        if (conn_manager_.get_socket()->isOpen()) {
            qint64 sent { conn_manager_.get_socket()->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData)) };
            if (cmd.get_payload_size()) {
                sent +=
                    conn_manager_.get_socket()->write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            }
            // qDebug() << "RemoteControlViewerV1: sent" << sent << "bytes.";
            static_cast<void>(sent);
        }
    } };
    auto root { p_engine_->rootObjects() };
    QObject::connect(root.first()->findChild<QObject*>("RCButton"), SIGNAL(rcButtonClicked(QString, QString)), p_rc_button_, SLOT(cppSlot(QString, QString)));
}


RemoteControlViewerV2::RemoteControlViewerV2(QQmlApplicationEngine* p_engine, ConnectionManagerV2& conn_manager)
    : p_engine_ { p_engine }, conn_manager_ { conn_manager }, p_rc_button_ {}, rc5_codes_ { { "Power", 0xc }, { "1", 0 }, { "2", 0 }, { "3", 0 }, { "4", 0 },
          { "5", 0 }, { "6", 0 }, { "7", 0 }, { "8", 0 }, { "9", 0 }, { "10", 0 }, { "11", 0 }, { "12", 0 }, { "Gr", 0 }, { "Re", 0 }, { "Ye", 0 }, { "Bl", 0 },
          { "I/II", 0x2b }, { "TV", 0 }, { "Gr", 0 }, { "||", 0x29 }, { "<<", 0x32 }, { "\u25B6", 0x35 }, { ">>", 0x34 }, { "\u2B1B", 0x36 }, { "\u25CF", 0 },
          { "CH*P/C", 0 }, { "V+", 0x10 }, { "M", 0xbf }, { "C+", 0 }, { "V-", 0x11 }, { "C-", 0 } } {}

RemoteControlViewerV2::~RemoteControlViewerV2() {
    delete p_rc_button_;
}

void RemoteControlViewerV2::register_buttons() {
    p_rc_button_ = new ConnectButton { [this](QString button, QString) {
        if (conn_manager_.get_version() != conn_manager_.version_active()) {
            return;
        }

        const auto it { rc5_codes_.find(button) };
        if (it == rc5_codes_.end()) {
            qDebug() << "RemoteControlViewerV2: unknown RC button pressed.";
            return;
        }

        const auto rc5code { static_cast<int16_t>(it->second) };
        // qDebug() << "RemoteControlViewerV2: button " << button << "pressed, code =" << rc5code;

        QString data { "s rc5 6 " + QString::number(rc5code) + "\r\n" };

        if (conn_manager_.get_socket()->isOpen()) {
            conn_manager_.get_socket()->write(data.toUtf8(), data.length());
        }
    } };
    auto root { p_engine_->rootObjects() };
    QObject::connect(root.first()->findChild<QObject*>("RCButton"), SIGNAL(rcButtonClicked(QString, QString)), p_rc_button_, SLOT(cppSlot(QString, QString)));
}
