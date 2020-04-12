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
 * @file    remotecontrol_viewer.cpp
 * @brief   IR remote control component
 * @author  Timo Sandmann
 * @date    12.04.2020
 */

#include <QTcpSocket>

#include "remotecontrol_viewer.h"
#include "command.h"


RemoteControlViewer::RemoteControlViewer(QQmlApplicationEngine* p_engine, QTcpSocket* p_socket)
    : p_engine_ { p_engine }, p_socket_ { p_socket }, p_rc_button_ {}, rc5_codes_ { { "Power", 0x118c }, { "1", 0x1181 }, { "2", 0x1182 }, { "3", 0x1183 },
          { "4", 0x1184 }, { "5", 0x1185 }, { "6", 0x1186 }, { "7", 0x1187 }, { "8", 0x1188 }, { "9", 0x1189 }, { "10", 0x1180 }, { "11", 0x118a },
          { "12", 0x11a3 }, { "Gr", 0x01ba }, { "Re", 0x01bd }, { "Ye", 0x01b1 }, { "Bl", 0x01b0 }, { "I/II", 0x11ab }, { "TV", 0x11b8 }, { "Gr", 0x01ba },
          { "||", 0x11a9 }, { "<<", 0x11b2 }, { "\u25B6", 0x11b5 }, { ">>", 0x11b4 }, { "\u25A0", 0x11b6 }, { "\u25CF", 0x11ab }, { "CH*P/C", 0x11bf },
          { "V+", 0x1190 }, { "M", 0x01bf }, { "C+", 0x11a0 }, { "V-", 0x1191 }, { "C-", 0x11a1 } } {}

RemoteControlViewer::~RemoteControlViewer() {
    delete p_rc_button_;
}

void RemoteControlViewer::register_buttons() {
    p_rc_button_ = new ConnectButton { [this](QString button, QString) {
        const auto it { rc5_codes_.find(button) };
        if (it == rc5_codes_.end()) {
            qDebug() << "RemoteControlViewer: unknown RC button pressed.";
            return;
        }

        const auto rc5code { static_cast<int16_t>(it->second) };
        // qDebug() << "RemoteControlViewer: button " << button << "pressed, code =" << rc5code;

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_SENS_RC5, ctbot::CommandCodes::CMD_SUB_NORM, rc5code, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };

        if (p_socket_->isOpen()) {
            qint64 sent { p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData)) };
            if (cmd.get_payload_size()) {
                sent += p_socket_->write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            }
            // qDebug() << "RemoteControlViewer: sent" << sent << "bytes.";
        }
    } };
    QObject::connect(p_engine_->rootObjects().first()->findChild<QObject*>("RCButton"), SIGNAL(rcButtonClicked(QString, QString)), p_rc_button_,
        SLOT(cppSlot(QString, QString)));
}
