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
 * @file    remotecall_list.h
 * @brief   QML list viewer for RemoteCalls
 * @author  Timo Sandmann
 * @date    27.01.2020
 */

#pragma once

#include <QObject>
#include <QVector>


struct RCItem {
    QString name;
    QString parameter1;
    QString parameter2;
    QString parameter3;
};


class RCList : public QObject {
    Q_OBJECT

public:
    explicit RCList(QObject* parent = nullptr);

    const QVector<RCItem>& items() const;

    bool setItemAt(int index, const RCItem& item);

signals:
    void preItemAppended();
    void postItemAppended();

public slots:
    void appendItem(const QString& name, const QString& parameter_info);

private:
    QVector<RCItem> items_;
};
