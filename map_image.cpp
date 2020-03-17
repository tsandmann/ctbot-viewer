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
 * @file    map_image.cpp
 * @brief   QML QuickPaintedItem for map viewer
 * @author  Timo Sandmann
 * @date    16.03.2020
 */

#include "map_image.h"


MapImageItem::MapImageItem(QQuickItem* parent) : QQuickPaintedItem { parent }, current_image_ { MAP_PIXEL_SIZE_, MAP_PIXEL_SIZE_, QImage::Format_Indexed8 },
    min_ { MAP_PIXEL_SIZE_ / 2, MAP_PIXEL_SIZE_ / 2 }, max_ { MAP_PIXEL_SIZE_ / 2, MAP_PIXEL_SIZE_ / 2 }, p_update_timer_ {}, p_update_thread_ {}, needs_update_ {} {
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
    painter->setPen(QColor { 255, 0, 0 });
    QPoint pos { static_cast<int>(MAP_PIXEL_SIZE_) - bot_pos_.x(), static_cast<int>(MAP_PIXEL_SIZE_) - bot_pos_.y() };
    QRect rect { pos - QPoint { 10, 10 }, pos + QPoint { 10, 10 } };
    painter->drawArc(rect, (bot_heading_ -50) * 16, 280 * 16);
}

QImage MapImageItem::image() const {
    return current_image_;
}

void MapImageItem::setImage(const QImage& image) {
    current_image_ = image;
    update();
}

void MapImageItem::set_pixel(const size_t x, const size_t y, const uint8_t value) {
    current_image_.setPixel(x, y, value);
}

void MapImageItem::update_map(const uint8_t* data, const size_t block, const size_t from, const size_t to) {
    const auto x { ((block * (MAP_SECTION_SIZE_ * 2)) % MAP_MACROBLOCK_SIZE_ + (block / MAP_MACROBLOCK_SIZE_) * MAP_MACROBLOCK_SIZE_) % MAP_PIXEL_SIZE_ }; // 2 sections pro Block in X-Richtung (nach Map-Orientierung)
    const auto y { (((block / MAP_SECTION_SIZE_) * MAP_SECTION_SIZE_) % MAP_MACROBLOCK_SIZE_) + (block / MAP_PIXEL_SIZE_) * MAP_MACROBLOCK_SIZE_ }; // 1 section pro Block in Y-Richtung (nach Map-Orientierung)

    /* neu empfangene Daten ins Map-Array kopieren */
    size_t pic_x {}, pic_y {};
    size_t bufferIndex {};
    for (size_t j { from }; j <= to && bufferIndex < (MAP_PIXEL_SIZE_ * MAP_PIXEL_SIZE_); ++j) { // Zeilen
        pic_y = x + j; // X der Map ist Y beim Sim
        if (pic_y >= static_cast<int>(MAP_PIXEL_SIZE_)) {
            /* ungueltige Daten */
            return;
        }
        for (size_t i {}; i < MAP_SECTION_SIZE_; ++i) { // Spalten
            pic_x = y + i; // Spaltenindex im Block berechnen, Y der Map ist X beim Sim
            if (pic_x >= static_cast<int>(MAP_PIXEL_SIZE_)) {
                /* ungueltige Daten */
                return;
            }
            /* Grauwert von int8_t nach int umrechnen */
            int8_t gray { static_cast<int8_t>(data[bufferIndex++]) };
            const auto index { static_cast<int>(gray) + 128 };
            current_image_.setPixel(pic_x, pic_y, static_cast<unsigned>(index));
        }
    }

    // Koordinaten innerhalb des Blocks ausblenden -> Eckpunkt mit kleinsten Koordinaten:
    pic_x &= ~(MAP_SECTION_SIZE_ - 1);
    // 2 Sections pro Block in X-Richtung (Map-Orientierung) entspricht Y-Richtung (Sim-Orientierung):
    pic_y &= ~(MAP_SECTION_SIZE_ * 2 - 1);
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

    //qDebug() << "MapImageItem::update_map(): min: " << min_ << " max: " << max_;
}

void MapImageItem::commit() {
    needs_update_ = true;
}
