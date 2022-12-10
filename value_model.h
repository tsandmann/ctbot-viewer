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
 * @file    value_model.h
 * @brief   QML model for list viewer
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#pragma once

#include <QAbstractListModel>

#include "value_list.h"


class ValueModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(ValueList* list READ list WRITE setList NOTIFY listChanged)

public:
    explicit ValueModel(QObject* parent = nullptr);

    enum { Name, Value };

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    ValueList* list() const;

    void setList(ValueList* list);

    void sort();

signals:
    void listChanged();

private:
    ValueList* list_;
};
