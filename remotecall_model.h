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
 * @file    remotecall_model.h
 * @brief   QML model for remotecall viewer
 * @author  Timo Sandmann
 * @date    27.01.2020
 */

#pragma once

#include <QAbstractListModel>


class RCList;

class RCModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(RCList* list READ list WRITE setList)

public:
    explicit RCModel(QObject* parent = nullptr);

    enum { Name, Parameter1, Parameter2, Parameter3 };

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    RCList* list() const;

    void setList(RCList* list);

private:
    RCList* list_;
};
