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
 * @file    value_list.h
 * @brief   QML list viewer
 * @author  Timo Sandmann
 * @date    06.01.2020
 */

#pragma once

#include <QObject>
#include <QVector>


struct ViewerItem {
    QString name;
    int value;
};


class ValueList : public QObject {
    Q_OBJECT

public:
    explicit ValueList(QObject* parent = nullptr);

    const QVector<ViewerItem>& items() const;

    bool setItemAt(int index, const ViewerItem& item);
    //    bool updateItemAt(int index, const int value);

signals:
    void preItemAppended();
    void postItemAppended();

public slots:
    void appendItem(const QString& name);

private:
    QVector<ViewerItem> items_;
};
