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
 * @file    command.cpp
 * @brief   Command management
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#include "command.h"


namespace ctbot {

CommandBase::CommandBase(const CommandData& cmd_data) : data_ { cmd_data }, has_crc_ {}, crc_ok_ {} {}

CommandBase::CommandBase(const CommandCodes& cmd_code, const CommandCodes& subcmd_code, int16_t data_l, int16_t data_r, uint8_t from, uint8_t to)
    : data_ { cmd_code, subcmd_code, data_l, data_r, from }, has_crc_ {}, crc_ok_ {} {
    data_.to = to;
}

CommandBase::CommandBase(QByteArray& buf) : has_crc_ {}, crc_ok_ {} {
    while (buf.indexOf(static_cast<char>(CommandCodes::CMD_STARTCODE)) != 0) {
        if (buf.indexOf(static_cast<char>(CommandCodes::CMD_STARTCODE)) == -1) {
            throw std::runtime_error("CommandBase::CommandBase(): no cmd found");
        }
        buf.remove(0, 1);
    }

    if (buf.size() < static_cast<int>(sizeof(CommandData))) {
        throw std::runtime_error("CommandBase::CommandBase(): no cmd found");
    }

    std::copy(buf.begin(), buf.begin() + sizeof(CommandData), reinterpret_cast<char*>(&data_));

    buf.remove(0, sizeof(CommandData));

    if (!valid()) {
        std::cerr << "CommandBase::CommandBase(): invalid command:\n" << *this << "\n";
        throw std::runtime_error("CommandBase::CommandBase(): invalid command (CRC)");
    }
}

bool CommandBase::append_payload(QByteArray& buf, const size_t len) {
    const auto n { len > MAX_PAYLOAD ? MAX_PAYLOAD : len };

    if (static_cast<int>(n) > buf.size()) {
        return false;
    }

    payload_.reserve(n);
    payload_.assign(buf.begin(), buf.begin() + static_cast<ptrdiff_t>(n));
    buf.remove(0, static_cast<int>(n));

    return true;
}

void CommandBase::append_crc(uint16_t crc) {
    has_crc_ = true;
    data_.to = static_cast<uint8_t>(crc & 0xff);
    data_.from = static_cast<uint8_t>((crc >> 8) & 0xff);
}

void CommandBase::add_payload(const void* payload, const size_t len) {
    data_.payload = len > MAX_PAYLOAD ? MAX_PAYLOAD : static_cast<uint8_t>(len);
    auto ptr { static_cast<const uint8_t*>(payload) };
    payload_.insert(payload_.begin(), ptr, ptr + data_.payload);
}

std::ostream& operator<<(std::ostream& os, const CommandBase& v) {
    os << "startCode='" << static_cast<char>(v.get_cmd().startCode) << "' (" << std::hex << std::showbase << static_cast<uint16_t>(v.get_cmd().startCode)
       << ")";
    if (v.get_cmd().startCode != static_cast<uint8_t>(CommandCodes::CMD_STARTCODE)) {
        os << " should be '" << static_cast<char>(CommandCodes::CMD_STARTCODE) << "' (" << std::hex << static_cast<uint16_t>(CommandCodes::CMD_STARTCODE)
           << ")";
    }
    os << "\n";
    os << "command='" << static_cast<char>(v.get_cmd().command) << "' (" << std::hex << static_cast<uint16_t>(v.get_cmd().command) << ")\n";
    os << "subcommand='" << static_cast<char>(v.get_cmd().subcommand) << "' (" << std::hex << static_cast<uint16_t>(v.get_cmd().subcommand) << ")\n";
    os << "direction=" << (v.get_cmd().direction == static_cast<uint8_t>(CommandDirection::CMD_REQUEST) ? "request" : "answer") << "\n";
    os << "seq=" << static_cast<uint16_t>(v.get_cmd().seq) << "\n";
    if (!v.has_crc_) {
        os << "from=" << static_cast<uint16_t>(v.get_cmd().from) << "\n";
        os << "to=" << static_cast<uint16_t>(v.get_cmd().to) << "\n";
    }
    os << "data_l=" << v.get_cmd().data_l << "\n";
    os << "data_r=" << v.get_cmd().data_r << "\n";
    os << "payload=" << static_cast<uint16_t>(v.get_cmd().payload) << "\n";
    os << "CRC='" << static_cast<char>(v.get_cmd().CRC) << "' (" << std::hex << static_cast<uint16_t>(v.get_cmd().CRC) << ")\n";
    if (v.get_cmd().CRC != static_cast<uint8_t>(CommandCodes::CMD_STOPCODE)) {
        os << " should be '" << static_cast<char>(CommandCodes::CMD_STOPCODE) << "' (" << std::hex << static_cast<uint16_t>(CommandCodes::CMD_STOPCODE)
           << ")\n";
    }
    if (v.has_crc_) {
        os << "crc16=" << std::hex << (static_cast<uint16_t>(v.get_cmd().to) | (static_cast<uint16_t>(v.get_cmd().from) << 8)) << "\n";
    }
    os << std::dec << "\n";
    return os;
}

} /* namespace ctbot */
