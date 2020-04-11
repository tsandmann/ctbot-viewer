#pragma once

#include "value_viewer.h"


class CommandEvaluator;

class ActuatorViewer : public ValueViewer {
    QObject* p_lcd_;

public:
    ActuatorViewer(QQmlApplicationEngine* p_engine, CommandEvaluator& command_eval);
};
