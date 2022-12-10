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
 * @file    map_image.cpp
 * @brief   QML QuickPaintedItem for map viewer
 * @author  Timo Sandmann
 * @date    16.03.2020
 */

#include "map_image.h"
#include <QPainterPath>
#include <QFile>


MapImageItem::MapImageItem(QQuickItem* parent)
    : QQuickPaintedItem { parent }, current_image_ { MAP_PIXEL_SIZE_, MAP_PIXEL_SIZE_, QImage::Format_Indexed8 },
      min_ { MAP_PIXEL_SIZE_ / 2, MAP_PIXEL_SIZE_ / 2 }, max_ { MAP_PIXEL_SIZE_ / 2, MAP_PIXEL_SIZE_ / 2 }, p_update_timer_ {}, p_update_thread_ {},
      needs_update_ {}, bot_pen_ { QColor { 255, 0, 0 } }, bot_brush_ { QColor { 255, 0, 0 }, Qt::SolidPattern } {
    QVector<QRgb> table;
    for (int i {}; i < 256; ++i) {
        table.push_back(qRgb(i, i, i));
    }
    current_image_.setColorTable(table);
    current_image_.fill(128);

    p_update_thread_ = new QThread { this };
    p_update_timer_ = new QTimer { nullptr };
    p_update_timer_->setInterval(100);
    p_update_timer_->moveToThread(p_update_thread_);

    connect(p_update_timer_, &QTimer::timeout, this, [this]() {
        if (needs_update_) {
            needs_update_ = false;
            MapImageItem::update();
        }
    });
    p_update_timer_->connect(p_update_thread_, SIGNAL(started()), SLOT(start()));
    p_update_thread_->start();
}

MapImageItem::~MapImageItem() {
    p_update_thread_->quit();
    p_update_timer_->deleteLater();
    p_update_thread_->wait();
    delete p_update_thread_;
}

void MapImageItem::paint(QPainter* painter) {
    painter->drawImage(min_, current_image_, QRect(min_, max_));

    for (auto& line : lines_) {
        const QPen pen { std::get<1>(line) };
        painter->setPen(pen);
        painter->drawLine(std::get<0>(line));
    }

    for (auto& circle : circles_) {
        const QPen pen { std::get<2>(circle) };
        painter->setPen(pen);
        painter->drawEllipse(std::get<0>(circle), std::get<1>(circle), std::get<1>(circle));
    }

    if (!bot_pos_.isNull()) {
        const QPointF pos { static_cast<qreal>(MAP_PIXEL_SIZE_ - bot_pos_.x()), static_cast<qreal>(MAP_PIXEL_SIZE_ - bot_pos_.y()) };
        const QRectF rect { pos - QPointF { MAP_RESOULTION_ * 0.12 / 2., MAP_RESOULTION_ * 0.12 / 2. },
            pos + QPointF { MAP_RESOULTION_ * 0.12 / 2., MAP_RESOULTION_ * 0.12 / 2. } };
        QPainterPath path { pos };
        path.arcTo(rect, bot_heading_ - 50., 280.);

        painter->setBrush(bot_brush_);
        painter->setPen(bot_pen_);
        painter->drawPath(path);
    }
}

QImage MapImageItem::image() const {
    return current_image_;
}

void MapImageItem::setImage(const QImage& image) {
    current_image_ = image;
    update();
}

void MapImageItem::clear() {
    current_image_.fill(128);
    lines_.clear();
    circles_.clear();
}

void MapImageItem::set_pixel(const size_t x, const size_t y, const uint8_t value) {
    current_image_.setPixel(x, y, value);
}

void MapImageItem::draw_line(const QPoint& from, const QPoint& to, const QColor& color) {
    lines_.emplace_back(std::make_tuple(QLine { from, to }, color)); // FIXME: lock
}

void MapImageItem::draw_cicle(const QPoint& center, size_t radius, const QColor& color) {
    circles_.emplace_back(std::make_tuple(center, radius, color)); // FIXME: lock
}

void MapImageItem::clear_lines(const size_t remaining) {
    const auto to_delete { lines_.size() <= remaining ? 0 : lines_.size() - remaining };
    lines_.erase(lines_.begin(), lines_.begin() + to_delete);
}

void MapImageItem::clear_circles(const size_t remaining) {
    const auto to_delete { circles_.size() <= remaining ? 0 : circles_.size() - remaining };
    circles_.erase(circles_.begin(), circles_.begin() + to_delete);
}

bool MapImageItem::save_to_file(const QString& filename) const {
    QUrl url(filename);
    QFile file(url.toLocalFile());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return current_image_.save(&file, "PNG");
    }

    qDebug() << "MapImageItem::save_to_file(" << filename << ") failed.";

    return false;
}

void MapImageItem::update_map(const uint8_t* data, const size_t block, const size_t from, const size_t to) {
    const auto x { ((block * (MAP_SECTION_SIZE_ * 2)) % MAP_MACROBLOCK_SIZE_ + (block / MAP_MACROBLOCK_SIZE_) * MAP_MACROBLOCK_SIZE_)
        % MAP_PIXEL_SIZE_ }; // 2 sections per block in X orientation of map
    const auto y { (((block / MAP_SECTION_SIZE_) * MAP_SECTION_SIZE_) % MAP_MACROBLOCK_SIZE_)
        + (block / MAP_PIXEL_SIZE_) * MAP_MACROBLOCK_SIZE_ }; // 1 section per block in Y orientation of map

    /* copy received data to map-image */
    size_t pic_x {}, pic_y {};
    size_t bufferIndex {};
    for (size_t j { from }; j <= to && bufferIndex < (MAP_PIXEL_SIZE_ * MAP_PIXEL_SIZE_); ++j) { // rows
        pic_y = x + j; // X of map is Y of viewer's coordinate system
        if (pic_y >= static_cast<int>(MAP_PIXEL_SIZE_)) {
            return; // invalid data
        }
        for (size_t i {}; i < MAP_SECTION_SIZE_; ++i) { // colums
            pic_x = y + i; // Y of map is X of viewer's coordinate system
            if (pic_x >= static_cast<int>(MAP_PIXEL_SIZE_)) {
                return; // invalid data
            }
            /* calc gray value based on index table */
            int8_t gray { static_cast<int8_t>(data[bufferIndex++]) };
            const auto index { static_cast<int>(gray) + 128 };
            current_image_.setPixel(pic_x, pic_y, static_cast<unsigned>(index));
        }
    }

    /* round coordinates to block size */
    pic_x &= ~(MAP_SECTION_SIZE_ - 1);
    pic_y &= ~(MAP_SECTION_SIZE_ * 2 - 1); // 2 sections per block in X orientation of map
    if (pic_x < static_cast<size_t>(min_.x())) {
        min_.setX(pic_x);
    } else if (pic_x > static_cast<size_t>(max_.x())) {
        max_.setX(pic_x);
    }
    if (pic_y < static_cast<size_t>(min_.y())) {
        min_.setY(pic_y);
    } else if (pic_y > static_cast<size_t>(max_.y())) {
        max_.setY(pic_y);
    }
}

void MapImageItem::commit() {
    needs_update_ = true;
}
