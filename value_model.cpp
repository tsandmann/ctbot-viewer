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
 * @file    value_model.cpp
 * @brief   QML model for list viewer
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#include <QDebug>

#include "value_model.h"


ValueModel::ValueModel(QObject* parent) : QAbstractListModel { parent }, list_ {} {}

int ValueModel::rowCount(const QModelIndex& parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !list_) {
        return 0;
    }

    return list_->items().size();
}

QVariant ValueModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || !list_) {
        return QVariant {};
    }

    const ViewerItem item { list_->items().at(index.row()) };

    switch (role) {
        case Name: return QVariant(item.name);

        case Value: return QVariant(item.value);
    }

    return QVariant {};
}

bool ValueModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!list_) {
        return false;
    }

    ViewerItem item { list_->items().at(index.row()) };
    // qDebug() << "ValueModel::setData(): item={\"" << item.name << "\", " << item.value << "}";

    switch (role) {
        case Name: item.name = value.toString(); break;

        case Value: item.value = value.toInt(); break;
    }

    if (list_->setItemAt(index.row(), item)) {
        // qDebug() << "ValueModel::setData(): item={" << item.name << ", " << item.value << "}";
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags ValueModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled;
}

QHash<int, QByteArray> ValueModel::roleNames() const {
    QHash<int, QByteArray> names;
    names[Name] = "name";
    names[Value] = "value";
    return names;
}

ValueList* ValueModel::list() const {
    return list_;
}

void ValueModel::setList(ValueList* list) {
    beginResetModel();

    if (list_) {
        list_->disconnect(this);
    }

    list_ = list;

    if (list_) {
        connect(list_, &ValueList::preItemAppended, this, [=]() {
            const int index = list_->items().size();
            beginInsertRows(QModelIndex(), index, index);
        });

        connect(list_, &ValueList::postItemAppended, this, [=]() { endInsertRows(); });
    }

    endResetModel();
}
