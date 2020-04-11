#pragma once

#include "value_viewer.h"


class CommandEvaluator;

class SensorViewer : public ValueViewer {
public:
    SensorViewer(QQmlApplicationEngine* p_engine, CommandEvaluator& command_eval);
};
