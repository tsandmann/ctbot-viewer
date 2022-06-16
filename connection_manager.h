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
 * @file    connection_manager.h
 * @brief   Connection and ct-Bot command management
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#pragma once

#include <QQmlApplicationEngine>
#include <QTcpSocket>
#include <QByteArray>

#include <map>
#include <vector>
#include <string>
#include <functional>

#include "command.h"
#include "connect_button.h"


class ConnectionManagerBase {
    ConnectButton* p_connect_button_;

protected:
    QQmlApplicationEngine* p_engine_;
    QTcpSocket socket_;
    QByteArray in_buffer_;
    bool connected_;
    ConnectButton* p_shutdown_button_;

    virtual bool process_incoming() = 0;
    virtual void register_buttons();
    virtual void connected_hook() {}
    virtual void disconnected_hook() {}

public:
    ConnectionManagerBase(QQmlApplicationEngine* p_engine);

    virtual ~ConnectionManagerBase();

    virtual int get_version() const = 0;
    int version_active() const;

    auto get_socket() {
        return &socket_;
    }
};


class ConnectionManagerV1 : public ConnectionManagerBase {
    std::map<ctbot::CommandCodes /*cmd*/, std::vector<std::function<bool(const ctbot::CommandBase&)>> /*functions*/> commands_;

protected:
    virtual bool process_incoming() override;
    bool evaluate_cmd(const ctbot::CommandNoCRC* p_cmd) const;

public:
    ConnectionManagerV1(QQmlApplicationEngine* p_engine);

    virtual ~ConnectionManagerV1();

    virtual int get_version() const override;
    virtual void register_buttons() override;
    void register_cmd(const ctbot::CommandCodes& cmd, std::function<bool(const ctbot::CommandBase&)>&& func);
};


class ConnectionManagerV2 : public ConnectionManagerBase {
    std::map<std::string /*cmd*/, std::vector<std::function<bool(const std::string_view&)>> /*functions*/> commands_;

protected:
    virtual bool process_incoming() override;
    virtual void connected_hook() override;
    virtual void disconnected_hook() override;
    bool evaluate_cmd(const std::string_view& cmd, const std::string_view& data) const;
    
public:
    ConnectionManagerV2(QQmlApplicationEngine* p_engine);

    virtual ~ConnectionManagerV2();

    virtual int get_version() const override;
    virtual void register_buttons() override;
    void register_cmd(const std::string_view& cmd, std::function<bool(const std::string_view&)>&& func);
};
