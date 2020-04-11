#pragma once

#include <QString>
#include <QHash>
#include <QModelIndex>
#include <QQmlApplicationEngine>

#include "value_model.h"
#include "value_list.h"


class CommandEvaluator;

class SensorViewer {
    ValueList sensor_list_;
    ValueModel sensor_model_;
    QHash<QString, QModelIndex> sensor_map_;

public:
    SensorViewer(QQmlApplicationEngine* p_engine, CommandEvaluator& command_eval);
};
