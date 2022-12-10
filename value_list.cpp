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
 * @file    value_list.cpp
 * @brief   QML list viewer
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#include "value_list.h"

#include <algorithm>


ValueList::ValueList(QObject* parent) : QObject { parent } {}

const QList<ViewerItem>& ValueList::items() const {
    return items_;
}

bool ValueList::setItemAt(int index, const ViewerItem& item) {
    if (index < 0 || index >= items_.size()) {
        return false;
    }

    const ViewerItem& oldItem = items_.at(index);
    if (item.value == oldItem.value && item.name == oldItem.name) {
        return false;
    }

    items_[index] = item;
    return true;
}

// bool ValueList::updateItemAt(int index, const int value) {
//    if (index < 0 || index >= items_.size()) {
//        return false;
//    }

//    ViewerItem& item = items_[index];
//    item.value = value;

//    emit itemUpdated(index);

//    return true;
//}

void ValueList::appendItem(const QString& name) {
    emit preItemAppended();

    ViewerItem item;
    item.name = name;
    item.value = 0;
    items_.append(item);

    emit postItemAppended();
}

void ValueList::sort() {
    std::sort(items_.begin(), items_.end(), [](const ViewerItem& v1, const ViewerItem& v2) { return v1.value > v2.value; });
}
