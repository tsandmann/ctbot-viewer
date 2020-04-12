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
 * @file    map_viewer.cpp
 * @brief   Map viewer component
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#include <QTcpSocket>

#include "map_viewer.h"
#include "map_image.h"
#include "command_evaluator.h"


MapViewer::MapViewer(QQmlApplicationEngine* p_engine, CommandEvaluator& command_eval, QTcpSocket* p_socket) : p_engine_ { p_engine }, p_socket_ { p_socket },
    p_fetch_button_ {}, p_clear_button_ {}, p_save_button_ {}, p_map_ {}, receive_state_ {}, last_block_ {} {
    qmlRegisterType<MapImageItem>("MapImage", 1, 0, "MapImageItem");

    command_eval.register_cmd(ctbot::CommandCodes::CMD_MAP, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_MAP received: " << cmd << "\n";

        if (!p_map_) {
            p_map_ = p_engine_->rootObjects().first()->findChild<MapImageItem*>("Map");
            if (!p_map_) {
                return false;
            }
        }

        const auto block { cmd.get_cmd_data_l() }; // 16 Bit Adresse des Map-Blocks
        switch (cmd.get_cmd_subcode()) {
        case ctbot::CommandCodes::CMD_SUB_MAP_DATA_1: {
            if (receive_state_ != 0) {
                receive_state_ = 0;
                return false;
            }
            p_map_->set_bot_y(MapImageItem::MAP_PIXEL_SIZE_ - cmd.get_cmd_data_r()); // Bot-Position, X-Komponente, wird im Bild in Y-Richtung gezaehlt
            p_map_->update_map(cmd.get_payload().data(), block, 0, 7);
            receive_state_ = 1;
            last_block_ = block;
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_DATA_2: {
            if (receive_state_ != 1 || last_block_ != block) {
                receive_state_ = 0;
                return false;
            }
            p_map_->set_bot_x(MapImageItem::MAP_PIXEL_SIZE_ - cmd.get_cmd_data_r()); // Bot-Position, Y-Komponente, wird im Bild in X-Richtung gezaehlt
            p_map_->update_map(cmd.get_payload().data(), block, 8, 15);
            receive_state_ = 2;
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_DATA_3: {
            if (receive_state_ != 2 || last_block_ != block) {
                receive_state_ = 0;
                return false;
            }
            p_map_->set_bot_heading(cmd.get_cmd_data_r());
            p_map_->update_map(cmd.get_payload().data(), block, 16, 23);
            receive_state_ = 3;
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_DATA_4: {
            static QPoint last_bot_pos {};

            if (receive_state_ != 3 || last_block_ != block) {
                receive_state_ = 0;
                return false;
            }
            p_map_->update_map(cmd.get_payload().data(), block, 24, 31);
            p_map_->commit();
            const auto bot_pos { p_map_->get_bot_pos() };
            if ((last_bot_pos - bot_pos).manhattanLength() > 10) {
                last_bot_pos = bot_pos;

                QMetaObject::invokeMethod(p_map_, "scroll_to", Q_ARG(QVariant, bot_pos.x()), Q_ARG(QVariant, bot_pos.y()));
            }
            receive_state_ = 0;
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_LINE: {
            if (cmd.get_payload_size() < 8) {
                return false;
            }

            QColor color;
            switch (block) {
            case 0:
                color.setRgb(0, 255, 0);
                break;

            case 1:
                color.setRgb(255, 0, 0);
                break;

            default:
                color.setRgb(0, 0, 0);
                break;
            }

            const auto y1 { cmd.get_payload()[0] | cmd.get_payload()[1] << 8 };
            const auto x1 { cmd.get_payload()[2] | cmd.get_payload()[3] << 8 };
            const auto y2 { cmd.get_payload()[4] | cmd.get_payload()[5] << 8 };
            const auto x2 { cmd.get_payload()[6] | cmd.get_payload()[7] << 8 };

            p_map_->draw_line(QPoint { x1, y1 }, QPoint { x2, y2 }, color);
            p_map_->commit();
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_CIRCLE: {
            if (cmd.get_payload_size() < 4) {
                return false;
            }

            QColor color;
            switch (block) {
            case 0:
                color.setRgb(0, 255, 0);
                break;

            case 1:
                color.setRgb(255, 0, 0);
                break;

            default:
                color.setRgb(0, 0, 0);
                break;
            }

            const auto radius { cmd.get_cmd_data_r() };
            const auto y { cmd.get_payload()[0] | cmd.get_payload()[1] << 8 };
            const auto x { cmd.get_payload()[2] | cmd.get_payload()[3] << 8 };

            p_map_->draw_cicle(QPoint { x, y }, radius, color);
            p_map_->commit();
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_CLEAR_LINES: {
            p_map_->clear_lines(block);
            p_map_->commit();
            break;
        }

        case ctbot::CommandCodes::CMD_SUB_MAP_CLEAR_CIRCLES: {
            p_map_->clear_circles(block);
            p_map_->commit();
            break;
        }

        default:
            return false;
        }

        return true;
    });
}

MapViewer::~MapViewer() {
    delete p_clear_button_;
    delete p_fetch_button_;
}

void MapViewer::register_buttons() {
    p_map_ = p_engine_->rootObjects().first()->findChild<MapImageItem*>("Map");
    if (!p_map_) {
        return;
    }

    p_fetch_button_ = new ConnectButton { [this](QString, QString) {
        ctbot::CommandNoCRC cmd { ctbot::CommandCodes::CMD_MAP, ctbot::CommandCodes::CMD_SUB_MAP_REQUEST, 0, 0, ctbot::CommandBase::ADDR_SIM,
            ctbot::CommandBase::ADDR_BROADCAST };
        if (p_socket_->isOpen()) {
            p_socket_->write(reinterpret_cast<const char*>(&cmd.get_cmd()), sizeof(ctbot::CommandData));
        }
    } };
    QObject::connect(p_engine_->rootObjects().first()->findChild<QObject*>("MapViewer"), SIGNAL(mapFetch()), p_fetch_button_, SLOT(cppSlot()));

    p_clear_button_ = new ConnectButton { [this](QString, QString) {
        p_map_->clear();
        p_map_->set_bot_x(0);
        p_map_->set_bot_y(0);
        p_map_->set_bot_heading(0);
        p_map_->commit();
    } };
    QObject::connect(p_engine_->rootObjects().first()->findChild<QObject*>("MapViewer"), SIGNAL(mapClear()), p_clear_button_, SLOT(cppSlot()));

    p_save_button_ = new ConnectButton { [this](QString filename, QString) {
        p_map_->save_to_file(filename);
    } };
    QObject::connect(p_engine_->rootObjects().first()->findChild<QObject*>("MapViewer"), SIGNAL(mapSave(QString)), p_save_button_, SLOT(cppSlot(QString)));
}
