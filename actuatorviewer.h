#pragma once

#include <QString>
#include <QHash>
#include <QModelIndex>
#include <QQmlApplicationEngine>

#include "value_model.h"
#include "value_list.h"


class QObject;
class CommandEvaluator;

class ActuatorViewer {
    QQmlApplicationEngine* p_engine_;
    ValueList actuator_list_;
    ValueModel actuator_model_;
    QHash<QString, QModelIndex> actuator_map_;
    QObject* p_lcd_;

public:
    ActuatorViewer(QQmlApplicationEngine* p_engine, CommandEvaluator& command_eval);
};
