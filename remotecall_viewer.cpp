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
 * @file    remotecall_viewer.cpp
 * @brief   RemoteCall viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QTcpSocket>
#include <QDebug>

#include "remotecall_viewer.h"
#include "connection_manager.h"


RemotecallViewer::RemotecallViewer(QQmlApplicationEngine* p_engine, ConnectionManagerV1& command_eval)
    : p_rcList_ { new RCList }, p_engine_ { p_engine }, p_socket_ { command_eval.get_socket() }, p_rc_viewer_ {}, p_current_label_ {}, p_fetch_button_ {},
      p_clear_button_ {}, p_abort_button_ {}, p_rc_button_ {} {
    qmlRegisterType<RCModel>("RemoteCalls", 1, 0, "RemotecallModel");
    qmlRegisterUncreatableType<RCList>("RemoteCalls", 1, 0, "RCList", QStringLiteral("RemoteCalls should not be created in QML"));

    rc_model_.setList(p_rcList_);

    p_engine_->rootContext()->setContextProperty(QStringLiteral("remotecallModel"), &rc_model_);

    command_eval.register_cmd(ctbot::CommandCodes::CMD_REMOTE_CALL, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_REMOTE_CALL received: " << cmd << "\n";

        if (cmd.get_cmd_subcode() != ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_ENTRY) {
            if (cmd.get_cmd_subcode() == ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_DONE) {
                auto entry { find_item(p_engine_->rootObjects(), "RemoteCallViewer") };
                if (entry) {
                    entry->setProperty("enabled", true);

                    auto e { find_item(p_engine_->rootObjects(), "CurrentRemoteCallLabel") };
                    if (e) {
                        e->setProperty("text", "Active Remote Call: none");
                    }

                    qDebug() << "RemoteCall done.";
                    return true;
                }
            }

            return false;
        }

        if (cmd.get_payload_size() < 66) {
            return false;
        }

        const QString name { QString::fromUtf8(reinterpret_cast<const char*>(cmd.get_payload().data() + 4)) };
        const QString params { QString::fromUtf8(reinterpret_cast<const char*>(cmd.get_payload().data() + 4 + 21)) };

        // qDebug() << "name =" << name << ", params =" << params;

        if (name.length()) {
            auto items { p_rcList_->items() };
            for (int i {}; i < items.size(); ++i) {
                if (items[i].name == name) {
                    return true;
                }
            }

            p_rcList_->appendItem(name, params);
        }

        return true;
    });
}

RemotecallViewer::~RemotecallViewer() {
    delete p_rc_button_;
    delete p_abort_button_;
    delete p_clear_button_;
    delete p_fetch_button_;
    delete p_rcList_;
}

void RemotecallViewer::register_buttons() {
    p_rc_viewer_ = find_item(p_engine_->rootObjects(), "RemoteCallViewer");
    p_current_label_ = find_item(p_engine_->rootObjects(), "CurrentRemoteCallLabel");
    if (!p_rc_viewer_ || !p_current_label_) {
        return;
    }

    p_fetch_button_ = new ConnectButton { [this](QString, QString) {
        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_REMOTE_CALL, ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_LIST, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        if (p_socket_->isOpen()) {
            p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
        }
    } };
    auto root { p_engine_->rootObjects() };
    QObject::connect(root.first()->findChild<QObject*>("RemoteCallActions"), SIGNAL(remoteCallFetch()), p_fetch_button_, SLOT(cppSlot()));


    p_clear_button_ = new ConnectButton { [this](QString, QString) {
        rc_model_.setList(nullptr);
        delete p_rcList_;
        p_rcList_ = new RCList;
        rc_model_.setList(p_rcList_);

        p_rc_viewer_->setProperty("enabled", true);
        p_current_label_->setProperty("text", "Active Remote Call: none");
    } };
    QObject::connect(root.first()->findChild<QObject*>("RemoteCallActions"), SIGNAL(remoteCallClear()), p_clear_button_, SLOT(cppSlot()));

    p_abort_button_ = new ConnectButton { [this](QString, QString) {
        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_REMOTE_CALL, ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_ABORT, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        if (p_socket_->isOpen()) {
            p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
        }

        p_rc_viewer_->setProperty("enabled", true);
        p_current_label_->setProperty("text", "Active Remote Call: none");
    } };
    QObject::connect(root.first()->findChild<QObject*>("RemoteCallActions"), SIGNAL(remoteCallAbort()), p_abort_button_, SLOT(cppSlot()));


    p_rc_button_ = new ConnectButton { [this](QString name, QString parameter) {
        qDebug() << "remotecall " << name << "(" << parameter << ")";

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_REMOTE_CALL, ctbot::CommandCodes::CMD_SUB_REMOTE_CALL_ORDER, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };

        QByteArray payload;
        payload.append(name.replace("():", "").toUtf8());
        payload.append('\0');

        const QStringList par_list { parameter.split(',', Qt::SkipEmptyParts) };
        uint32_t par[] { static_cast<uint32_t>(par_list.at(0).toLong()), static_cast<uint32_t>(par_list.at(1).toLong()),
            static_cast<uint32_t>(par_list.at(2).toLong()) };

        for (size_t i {}; i < 3; ++i) {
            payload.append(static_cast<char>((par[i] >> 0) & 0xff));
            payload.append(static_cast<char>((par[i] >> 8) & 0xff));
            payload.append(static_cast<char>((par[i] >> 16) & 0xff));
            payload.append(static_cast<char>((par[i] >> 24) & 0xff));
        }

        // qDebug() << "payload =" << payload;

        cmd.add_payload(payload.data(), static_cast<size_t>(payload.size()));

        if (p_socket_->isOpen()) {
            qint64 sent { p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData)) };
            if (cmd.get_payload_size()) {
                sent += p_socket_->write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            }
            qDebug() << "sent" << sent << "bytes.";
        }

        p_rc_viewer_->setProperty("enabled", false);
        p_current_label_->setProperty("text", "Active Remote Call: " + name);
    } };
    QObject::connect(root.first()->findChild<QObject*>("RemoteCallViewer"), SIGNAL(remoteCallClicked(QString,QString)), p_rc_button_,
        SLOT(cppSlot(QString,QString)));
}

QQuickItem* RemotecallViewer::find_item(const QList<QObject*>& nodes, const QString& name) {
    for (int i {}; i < nodes.size(); ++i) {
        if (nodes.at(i) && nodes.at(i)->objectName() == name) { // search for node
            return dynamic_cast<QQuickItem*>(nodes.at(i));
        } else if (nodes.at(i) && nodes.at(i)->children().size() > 0) { // search in children
            QQuickItem* item { find_item(nodes.at(i)->children(), name) };
            if (item) {
                return item;
            }
        }
    }

    return nullptr;
}
