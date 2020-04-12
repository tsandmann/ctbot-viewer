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
 * @file    script_editor.cpp
 * @brief   Script editor component
 * @author  Timo Sandmann
 * @date    12.04.2020
 */

#include <QQuickItem>
#include <QTcpSocket>
#include <QThread>

#include "script_editor.h"
#include "command.h"


ScriptEditor::ScriptEditor(QQmlApplicationEngine* p_engine, QTcpSocket* p_socket) : p_engine_ { p_engine }, p_socket_ { p_socket },
    p_script_ {}, p_editor_ {}, p_type_ {}, p_execute_ {}, p_filename_ {}, p_load_button_ {}, p_save_button_ {}, p_send_button_ {}, p_abort_button_ {} {}

ScriptEditor::~ScriptEditor() {
    delete p_abort_button_;
    delete p_send_button_;
    delete p_save_button_;
    delete p_load_button_;
}

void ScriptEditor::register_buttons() {
    p_script_ = p_engine_->rootObjects().first()->findChild<QObject*>("script_viewer");
    if (!p_script_) {
        return;
    }

    p_editor_ = p_script_->findChild<QQuickItem*>("script_editor");
    p_type_ = p_script_->findChild<QQuickItem*>("scripts_abl");
    p_execute_ = p_script_->findChild<QQuickItem*>("script_execute");
    p_filename_ = p_script_->findChild<QQuickItem*>("script_remote_filename");
    if (!p_editor_ || !p_type_ || !p_execute_ || !p_filename_) {
        delete p_filename_;
        delete p_execute_;
        delete p_type_;
        delete p_editor_;
        delete p_script_;
        return;
    }

    p_load_button_ = new ConnectButton { [this](QString filename, QString) {
        const QUrl url { filename };
        QFile file { url.toLocalFile() };
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }
        QTextStream stream { &file };
        QString text;
        while (!stream.atEnd()) {
            text.append(stream.readLine());
            text.append("\n");
        }
        file.close();

        p_editor_->setProperty("text", text);

        qDebug() << "script loaded from: " << url.toLocalFile();
    } };
    QObject::connect(p_script_, SIGNAL(scriptLoad(QString)), p_load_button_, SLOT(cppSlot(QString)));

    p_save_button_ = new ConnectButton { [this](QString filename, QString) {
        QUrl url { filename };
        QFile file { url.toLocalFile() };
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return;
        }
        QTextStream stream { &file };
        const QString text { p_editor_->property("text").toString() };
        stream << text;
        file.close();

        qDebug() << "script saved to: " << url.toLocalFile();
    } };
    QObject::connect(p_script_, SIGNAL(scriptSave(QString)), p_save_button_, SLOT(cppSlot(QString)));

    p_send_button_ = new ConnectButton { [this](QString, QString) {
        if (!p_socket_->isOpen()) {
            return;
        }

        const bool type { p_type_->property("checked").toBool() }; // false: basic, true: abl
        const bool execute { p_execute_->property("checked").toBool() };
        const QByteArray remote_filename { p_filename_->property("text").toString().toLatin1() };
        const QByteArray content_array { p_editor_->property("text").toString().toLatin1() };
        const auto content_length { static_cast<int16_t>(content_array.length()) };

        // qDebug() << "type=" << type << "execute=" << execute << "filename=" << remote_filename << "length=" << content_length;
        // qDebug() << "content=" << content_array;

        if (remote_filename.length() < 5) {
            return;
        }

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_PREPARE, static_cast<int16_t>(type), content_length, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        cmd.add_payload(remote_filename.constData(), remote_filename.length());

        qint64 sent { p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData)) };
        sent += p_socket_->write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
        p_socket_->flush();

        // qDebug() << "script prepared, sent=" << sent;
        QThread::usleep(75 * 3'000); // FIXME: use timer?

        sent = 0;
        int16_t i {};
        for (; i < content_length / 64; ++i) {
            ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_DATA, static_cast<int16_t>(type), static_cast<int16_t>(i * 64),
                ctbot::CommandBase::ADDR_SIM, ctbot::CommandBase::ADDR_BROADCAST };
            cmd.add_payload(&content_array.constData()[i * 64], 64);
            sent += p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
            sent += p_socket_->write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            p_socket_->flush();

            QThread::usleep(75 * 1'000); // FIXME: use timer?
        }

        const auto to_send { content_length % 64 };
        // qDebug() << "to_send=" << to_send;

        if (to_send) {
            ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_DATA, static_cast<int16_t>(type), static_cast<int16_t>(i * 64),
                ctbot::CommandBase::ADDR_SIM, ctbot::CommandBase::ADDR_BROADCAST };
            cmd.add_payload(&content_array.constData()[i * 64], to_send);
            sent += p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
            sent += p_socket_->write(reinterpret_cast<const char*>(cmd.get_payload().data()), static_cast<int64_t>(cmd.get_payload_size()));
            p_socket_->flush();

            QThread::usleep(75 * 1'000); // FIXME: use timer?
        }

        qDebug() << "script sent," << sent << "bytes.";

        if (execute) {
            ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_START, static_cast<int16_t>(type), 0,
                ctbot::CommandBase::ADDR_SIM, ctbot::CommandBase::ADDR_BROADCAST };
            p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));

            qDebug() << "script started.";
        }

    } };
    QObject::connect(p_script_, SIGNAL(scriptSend()), p_send_button_, SLOT(cppSlot()));

    p_abort_button_ = new ConnectButton { [this](QString, QString) {
        if (!p_socket_->isOpen()) {
            return;
        }

        const bool type { p_type_->property("checked").toBool() }; // false: basic, true: abl

        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_PROGRAM, ctbot::CommandCodes::CMD_SUB_PROGRAM_STOP, static_cast<int16_t>(type), 0,
            ctbot::CommandBase::ADDR_SIM, ctbot::CommandBase::ADDR_BROADCAST };
        p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));

        qDebug() << "script aborted.";
    } };
    QObject::connect(p_script_, SIGNAL(scriptAbort()), p_abort_button_, SLOT(cppSlot()));
}
