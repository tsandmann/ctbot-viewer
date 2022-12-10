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
 * @file    map_image.h
 * @brief   QML QuickPaintedItem for map viewer
 * @author  Timo Sandmann
 * @date    16.03.2020
 */

#pragma once

#include <cstdint>
#include <atomic>
#include <deque>
#include <tuple>

#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QImage>
#include <QPoint>
#include <QLine>
#include <QColor>
#include <QTimer>
#include <QThread>


class MapImageItem : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

public:
    static constexpr qreal MAP_SIZE_ { 12.288 };
    static constexpr size_t MAP_RESOULTION_ { 125 };
    static constexpr size_t MAP_SECTION_SIZE_ { 16 };
    static constexpr size_t MAP_MACROBLOCK_SIZE_ { 512 };
    static constexpr size_t MAP_PIXEL_SIZE_ { static_cast<size_t>(MAP_SIZE_ * MAP_RESOULTION_) };

    MapImageItem(QQuickItem* parent = nullptr);
    ~MapImageItem();

    void setImage(const QImage& image);

    void paint(QPainter* painter);

    QImage image() const;

    void set_pixel(const size_t x, const size_t y, const uint8_t value);

    void update_map(const uint8_t* data, const size_t block, const size_t from, const size_t to);

    void clear();

    Q_INVOKABLE void commit();

    void set_bot_x(const uint16_t new_pos) {
        bot_pos_.setX(new_pos);
    }

    void set_bot_y(const uint16_t new_pos) {
        bot_pos_.setY(new_pos);
    }

    void set_bot_heading(const uint16_t new_head) {
        bot_heading_ = new_head;
    }

    void draw_line(const QPoint& from, const QPoint& to, const QColor& color);

    void draw_cicle(const QPoint& center, size_t radius, const QColor& color);

    void clear_lines(const size_t remaining);

    void clear_circles(const size_t remaining);

    bool save_to_file(const QString& filename) const;

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
    const QPen bot_pen_;
    const QBrush bot_brush_;
    std::deque<std::tuple<QLine, QColor>> lines_;
    std::deque<std::tuple<QPoint, size_t, QColor>> circles_;
};
