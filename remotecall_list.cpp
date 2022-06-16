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
 * @file    remotecall_list.cpp
 * @brief   QML list viewer for RemoteCalls
 * @author  Timo Sandmann
 * @date    27.01.2020
 */

#include "remotecall_list.h"


RCList::RCList(QObject* parent) : QObject { parent } {}

const QVector<RCItem>& RCList::items() const {
    return items_;
}

bool RCList::setItemAt(int index, const RCItem& item) {
    if (index < 0 || index >= items_.size()) {
        return false;
    }

    const RCItem& oldItem = items_.at(index);
    if (item.name == oldItem.name) {
        return false;
    }

    items_[index] = item;
    return true;
}

void RCList::appendItem(const QString& name, const QString& parameter_info) {
    emit preItemAppended();

    const QStringList par_list { parameter_info.split(',', Qt::SkipEmptyParts) };

    RCItem item;
    item.name = name;

    if (par_list.size() >= 1) {
        item.parameter1 = par_list[0].trimmed().replace(" ", ": ");
        if (par_list.size() >= 2) {
            item.parameter2 = par_list[1].trimmed().replace(" ", ": ");
            if (par_list.size() >= 3) {
                item.parameter3 = par_list[2].trimmed().replace(" ", ": ");
            } else {
                item.parameter3 = "";
            }
        } else {
            item.parameter2 = "";
        }
    } else {
        item.parameter1 = "";
    }
    items_.append(item);

    emit postItemAppended();
}
