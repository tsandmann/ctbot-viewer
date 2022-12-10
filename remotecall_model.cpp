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
 * @file    remotecall_model.cpp
 * @brief   QML model for remotecall viewer
 * @author  Timo Sandmann
 * @date    27.01.2020
 */

#include <QDebug>

#include "remotecall_model.h"


RCModel::RCModel(QObject* parent) : QAbstractListModel { parent }, list_ {} {}

int RCModel::rowCount(const QModelIndex& parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !list_) {
        return 0;
    }

    return list_->items().size();
}

QVariant RCModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || !list_) {
        return QVariant {};
    }

    const RCItem item { list_->items().at(index.row()) };

    switch (role) {
        case Name: return QVariant(item.name);

        case Parameter1: return QVariant(item.parameter1);
        case Parameter2: return QVariant(item.parameter2);
        case Parameter3: return QVariant(item.parameter3);
    }

    return QVariant {};
}

bool RCModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!list_) {
        return false;
    }

    RCItem item { list_->items().at(index.row()) };
    // qDebug() << "ValueModel::setData(): item={\"" << item.name << "\", " << item.value << "}";

    switch (role) {
        case Name: item.name = value.toString(); break;

        case Parameter1: item.parameter1 = value.toString(); break;
        case Parameter2: item.parameter2 = value.toString(); break;
        case Parameter3: item.parameter3 = value.toString(); break;
    }

    if (list_->setItemAt(index.row(), item)) {
        // qDebug() << "ValueModel::setData(): item={" << item.name << ", " << item.value << "}";
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags RCModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled;
}

QHash<int, QByteArray> RCModel::roleNames() const {
    QHash<int, QByteArray> names;
    names[Name] = "name";
    names[Parameter1] = "parameter1";
    names[Parameter2] = "parameter2";
    names[Parameter3] = "parameter3";
    return names;
}

RCList* RCModel::list() const {
    return list_;
}

void RCModel::setList(RCList* list) {
    beginResetModel();

    if (list_) {
        list_->disconnect(this);
    }

    list_ = list;

    if (list_) {
        connect(list_, &RCList::preItemAppended, this, [=]() {
            const int index = list_->items().size();
            beginInsertRows(QModelIndex(), index, index);
        });

        connect(list_, &RCList::postItemAppended, this, [=]() { endInsertRows(); });
    }

    endResetModel();
}
