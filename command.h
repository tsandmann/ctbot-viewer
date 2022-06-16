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
 * @file    command.h
 * @brief   Command management
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <type_traits>
#include <QByteArray>


namespace ctbot {

enum class CommandDirection : uint8_t {
    CMD_REQUEST = 0, /**< direction for requests */
    CMD_ANSWER = 1, /**< direction for answers */
};
static_assert(sizeof(CommandDirection) == 1, "enum CommandDirection has wrong size");


enum class CommandCodes : uint8_t {
    CMD_STARTCODE = '>', /**< Start of command */
    CMD_STOPCODE = '<', /**< End of command */

    CMD_SUB_NORM = 'N', /**< Standard subcommand */

    CMD_DONE = 'X', /**< Ende of cycle */
    CMD_SHUTDOWN = 'q', /**< Command for bot shutdown */

    /* Sensors */
    CMD_SENS_IR = 'I', /**< Abstandssensoren */
    CMD_SENS_ENC = 'E', /**< Radencoder */
    CMD_SENS_BORDER = 'B', /**< Abgrundsensoren */
    CMD_SENS_LINE = 'L', /**< Liniensensoren */
    CMD_SENS_LDR = 'H', /**< Helligkeitssensoren */
    CMD_SENS_TRANS = 'T', /**< Ueberwachung Transportfach */
    CMD_SENS_DOOR = 'D', /**< Ueberwachung Klappe */
    CMD_SENS_MOUSE = 'm', /**< Maussensor */
    CMD_SENS_ERROR = 'e', /**< Motor- oder Batteriefehler */
    CMD_SENS_RC5 = 'R', /**< IR-Fernbedienung */
    CMD_SENS_BPS = 'b', /**< Bot Positioning System */
    CMD_SENS_MOUSE_PICTURE = 'P', /**< Bild vom Maussensor in data_l steht, welche Nummer der 1. Pixel hat */

    /* Actuators */
    CMD_AKT_MOT = 'M', /**< Motorgeschwindigkeit */
    CMD_AKT_SERVO = 'S', /**< Steuerung Servo  */
    CMD_AKT_LED = 'l', /**< LEDs steuern */
    CMD_AKT_LCD = 'c', /**< LCD Anzeige */

    /* Address management */
    CMD_ID = 'A', /**< Adressverwaltung */
    CMD_SUB_ID_REQUEST = 'R', /**< Fordere eine Adresse an */
    CMD_SUB_ID_OFFER = 'O', /**< Bot bekommt eine Adresse angeboten */
    CMD_SUB_ID_SET = 'S', /**< Bot Setzt/bestaetigt eine Adresse */

    /* Subcommands for LCD */
    CMD_SUB_LCD_CLEAR = 'c', /**< clear screen */
    CMD_SUB_LCD_DATA = 'D', /**< text w/o cursor */
    CMD_SUB_LCD_CURSOR = 'C', /**< cursor coordinates */

    /* Loging */
    CMD_LOG = 'O', /**< Logging output */

    /* c't-Sim connection */
    CMD_WELCOME = 'W', /**< Kommando zum Anmelden an c't-Sim */
    CMD_SUB_WELCOME_REAL = 'R', /**< Subkommando zum Anmelden eines realen Bots an c't-Sim */
    CMD_SUB_WELCOME_SIM = 'S', /**< Subkommando zum Anmelden eines simulierten Bots an c't-Sim */
    CMD_SUB_WELCOME_BOTS = 'B', /**< Subkommando zum Bekanntmachen der eigenen ID bei anderen Bots */

    /* Remote-Calls */
    CMD_REMOTE_CALL = 'r', /**< Kommado fuer Remote-Calls */
    CMD_SUB_REMOTE_CALL_LIST = 'L', /**< Anforderung an den Bot alle verfuegbaren Kommandos zu listen */
    CMD_SUB_REMOTE_CALL_ENTRY = 'E', /**< Hiermit liefert der Bot ein verfuegbares Kommandos an den PC */
    CMD_SUB_REMOTE_CALL_ORDER = 'O', /**< Hiermit gibt der PC einen Remote-Call in Auftrag */
    CMD_SUB_REMOTE_CALL_DONE = 'D', /**< Hiermit signalisiert der MCU dem PC die Beendigung des Auftrags. Ergebins steht in DataL 0=FAIL 1=SUCCESS */
    CMD_SUB_REMOTE_CALL_ABORT = 'A', /**< Hiermit signalisiert der PC dem MCU die Berarbeitung des laufenden Remote-Calls zu beenden */

    /* Map */
    CMD_MAP = 'Q', /**< Kommando fuer Map */
    CMD_SUB_MAP_DATA_1 = 'D', /**< Daten eines Map-Blocks (bzw. die ersten 128 Byte davon, es sind also 4 Kommandos noetig fuer einen kompletten Block) */
    CMD_SUB_MAP_DATA_2 = 'E', /**< Map-Daten Teil 2 */
    CMD_SUB_MAP_DATA_3 = 'F', /**< Map-Daten Teil 3 */
    CMD_SUB_MAP_DATA_4 = 'G', /**< Map-Daten Teil 4 */
    CMD_SUB_MAP_REQUEST = 'R', /**< Aufforderung die komplette Karte (neu) zu uebertragen */
    CMD_SUB_MAP_LINE = 'L', /**< Linie zeichnen */
    CMD_SUB_MAP_CIRCLE = 'C', /**< Kreis zeichnen */
    CMD_SUB_MAP_CLEAR_LINES = 'X', /**< Linien loeschen */
    CMD_SUB_MAP_CLEAR_CIRCLES = 'Y', /**< Kreise loeschen */

    /* Program transfer */
    CMD_PROGRAM = 'p', /**< Program data (Basic or ABL) */
    CMD_SUB_PROGRAM_PREPARE = 'P', /**< Bereitet den Programm-Versand vor */
    CMD_SUB_PROGRAM_DATA = 'D', /**< Schickt ein Skript-Programm an den Bot */
    CMD_SUB_PROGRAM_START = 'S', /**< Startet ein uebertragenes Programm auf dem Bot */
    CMD_SUB_PROGRAM_STOP = 'Q', /**< Bricht ein laufendes Programm ab */

    /* Settings */
    CMD_SETTINGS = 's', /**< Einstellungen uebertragen */
    CMD_SUB_SETTINGS_DISTSENS = 'e', /**< Auswertung der Sensordaten (L: Rohwerte 0 / vorverarbeitet 1) */
};
static_assert(sizeof(CommandCodes) == 1, "enum CommandCodes has wrong size");


struct CommandData {
    uint8_t startCode; /**< Begin of command (0) */
    uint8_t command; /**< command code (1) */
    uint8_t subcommand : 7; /**< subcommand code (2) */
    uint8_t direction : 1; /**< 0 for request, 1 for answer (2) */
    uint8_t payload; /**< # Bytes payload (3) */
    int16_t data_l; /**< Data left channel (4-5) */
    int16_t data_r; /**< Data right channel (6-7) */
    uint8_t seq; /**< Sequence number (8) */
    uint8_t from; /**< Sender address (9) */
    uint8_t to; /**< Receiver address (10) */
    uint8_t CRC; /**< End of command (11) */

    CommandData();
    CommandData(const CommandCodes& cmd_code, const CommandCodes& subcmd_code, int16_t data_l, int16_t data_r, uint8_t from);
} __attribute__((packed));
static_assert(sizeof(CommandData) == 12, "struct CommandData has wrong size, not packed?");


class CommandBase {
protected:
    CommandData data_;
    std::vector<uint8_t> payload_;
    bool has_crc_;
    bool crc_ok_;

    static constexpr uint8_t codes_to_int(const CommandCodes c) {
        return static_cast<uint8_t>(c);
    }

    static constexpr uint8_t dir_to_int(const CommandDirection d) {
        return static_cast<uint8_t>(d);
    }

    void add_payload(const void* payload, const size_t len);

public:
    CommandBase(const CommandData& cmd_data);
    CommandBase(const CommandCodes& cmd_code, const CommandCodes& subcmd_code, int16_t data_l, int16_t data_r, uint8_t from, uint8_t to = ADDR_NOT_SET);
    CommandBase(QByteArray& buf);
    ~CommandBase() {}

    const auto& get_cmd() const {
        return data_;
    }

    auto get_cmd_code() const {
        return static_cast<CommandCodes>(data_.command);
    }

    auto get_cmd_code_uint() const {
        return data_.command;
    }

    auto get_cmd_subcode() const {
        return static_cast<CommandCodes>(data_.subcommand);
    }

    auto get_cmd_subcode_uint() const {
        return data_.subcommand;
    }

    auto get_cmd_data_l() const {
        return data_.data_l;
    }

    auto get_cmd_data_r() const {
        return data_.data_r;
    }

    auto get_cmd_from() const {
        return data_.from;
    }

    auto get_cmd_to() const {
        return data_.to;
    }

    auto get_cmd_has_crc() const {
        return has_crc_;
    }

    template <typename T>
    void set_cmd_data_l(T data) {
        data_.data_l = static_cast<int16_t>(data);
    }

    template <typename T>
    void set_cmd_data_r(T data) {
        data_.data_r = static_cast<int16_t>(data);
    }

    void set_cmd_from(uint8_t addr) {
        data_.from = addr;
    }

    void set_cmd_to(uint8_t addr) {
        data_.to = addr;
    }

    void set_cmd_has_crc(bool b_crc) {
        has_crc_ = b_crc;
    }

    bool valid() const {
        return get_cmd().startCode == codes_to_int(CommandCodes::CMD_STARTCODE) && get_cmd().CRC == codes_to_int(CommandCodes::CMD_STOPCODE);
    }

    bool validCRC() const {
        return valid() && crc_ok_;
    }

    size_t get_payload_size() const {
        return get_cmd().payload;
    }

    const auto& get_payload() const {
        return payload_;
    }

    auto code() const {
        return get_cmd().command;
    }

    void append_crc(uint16_t crc);

    bool append_payload(QByteArray& buf, const size_t len);

    friend std::ostream& operator<<(std::ostream& os, const ctbot::CommandBase& v);

    static const uint8_t MAX_PAYLOAD { 255 }; /**< max. amount of payload in byte */
    static const uint8_t ADDR_BROADCAST { 0xff }; /**< Broadcast address, data is sent to all bots */
    static const uint8_t ADDR_SIM { 0xfe }; /**< "Bot" address of c't-Sim */
    static const uint8_t ADDR_NOT_SET { 0 }; /**< "Bot" address not set */
};


struct CRCNoCheck {
    static bool checkCRC(CommandBase&) {
        return true;
    }

    static void addCRC(CommandBase&) {}
};


template <class CRCPolicy = CRCNoCheck>
class Command : public CommandBase, public CRCPolicy {
protected:
    void update_crc() {
        CRCPolicy::addCRC(*this);
    }

public:
    Command(const CommandData& cmd_data) : CommandBase { cmd_data } {
        update_crc();
    }

    Command(
        const CommandCodes& cmd_code, const CommandCodes& subcmd_code, int16_t data_l, int16_t data_r, uint8_t from = ADDR_NOT_SET, uint8_t to = ADDR_NOT_SET)
        : CommandBase { cmd_code, subcmd_code, data_l, data_r, from, to } {
        update_crc();
    }

    Command(QByteArray& buf) : CommandBase { buf } {
        crc_ok_ = CRCPolicy::checkCRC(*this);

        if (!validCRC()) {
            std::cerr << "Command<>::Command(): invalid command (CRC):\n" << *this << "\n";
            throw std::runtime_error("Command<>::Command(): invalid command (CRC)");
        }
    }

    ~Command() = default;

    void add_payload(const void* payload, const size_t len) {
        CommandBase::add_payload(payload, len);
        update_crc();
    }
};


using CommandNoCRC = Command<CRCNoCheck>;


inline CommandData::CommandData()
    : startCode { static_cast<uint8_t>(CommandCodes::CMD_STARTCODE) }, command { 0 }, subcommand { static_cast<uint8_t>(CommandCodes::CMD_SUB_NORM) },
      direction { static_cast<uint8_t>(CommandDirection::CMD_REQUEST) }, payload { 0 }, data_l { 0 }, data_r { 0 }, seq { 0 }, from { Command<>::ADDR_NOT_SET },
      to { Command<>::ADDR_NOT_SET }, CRC { static_cast<uint8_t>(CommandCodes::CMD_STOPCODE) } {}

inline CommandData::CommandData(
    const CommandCodes& cmd_code, const CommandCodes& subcmd_code, int16_t data_l, int16_t data_r, uint8_t from = Command<>::ADDR_NOT_SET)
    : startCode { static_cast<uint8_t>(CommandCodes::CMD_STARTCODE) }, command { static_cast<uint8_t>(cmd_code) },
      subcommand { static_cast<uint8_t>(static_cast<uint8_t>(subcmd_code) & 0x7f) }, direction { static_cast<uint8_t>(CommandDirection::CMD_REQUEST) },
      payload { 0 }, data_l { data_l }, data_r { data_r }, seq { 0 }, from { from }, to { Command<>::ADDR_NOT_SET }, CRC { static_cast<uint8_t>(
                                                                                                                         CommandCodes::CMD_STOPCODE) } {}

} /* namespace ctbot */
