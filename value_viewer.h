#pragma once

#include <QString>
#include <QHash>
#include <QModelIndex>
#include <QQmlApplicationEngine>

#include "value_model.h"
#include "value_list.h"


class ValueViewer {
protected:
    QQmlApplicationEngine* p_engine_;
    ValueList list_;
    ValueModel model_;
    QHash<QString, QModelIndex> map_;

    void update_map();
    void register_model(const QString& modelname);

public:
    ValueViewer(QQmlApplicationEngine* p_engine);
};
