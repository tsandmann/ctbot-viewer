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
 * @file    map_image.h
 * @brief   QML QuickPaintedItem for map viewer
 * @author  Timo Sandmann
 * @date    16.03.2020
 */

#pragma once

#include <cstdint>
#include <atomic>
#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QThread>


class MapImageItem : public QQuickPaintedItem {
Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

public:
    static constexpr float MAP_SIZE_ { 12.288 };
    static constexpr size_t MAP_RESOULTION_ { 125 };
    static constexpr size_t MAP_SECTION_SIZE_ { 16 };
    static constexpr size_t MAP_MACROBLOCK_SIZE_ { 512 };
    static constexpr size_t MAP_PIXEL_SIZE_ { static_cast<size_t>(MAP_SIZE_ * MAP_RESOULTION_) };

    MapImageItem(QQuickItem* parent = nullptr);
    ~MapImageItem();

    Q_INVOKABLE void setImage(const QImage& image);

    void paint(QPainter* painter);

    QImage image() const;

    void set_pixel(const size_t x, const size_t y, const uint8_t value);

    void update_map(const uint8_t* data, const size_t block, const size_t from, const size_t to);

    void commit();

    void set_bot_x(const uint16_t new_pos) {
        bot_pos_.setX(new_pos);
    }

    void set_bot_y(const uint16_t new_pos) {
        bot_pos_.setY(new_pos);
    }

    void set_bot_heading(const uint16_t new_head) {
        bot_heading_ = new_head;
    }

    QPoint get_bot_pos() const {
        return bot_pos_;
    }

signals:
    void imageChanged();

private:
    QImage current_image_;
    QPoint min_;
    QPoint max_;
    QTimer* p_update_timer_;
    QThread* p_update_thread_;
    std::atomic<bool> needs_update_;
    QPoint bot_pos_;
    unsigned bot_heading_;
};
