#include <QString>
#include <QQmlContext>
#include <QDebug>
#include <iostream>

#include "sensorviewer.h"
#include "commandevaluator.h"
#include "command.h"


SensorViewer::SensorViewer(QQmlApplicationEngine* p_engine, CommandEvaluator& command_eval) {
    qmlRegisterType<ValueModel>("Sensors", 1, 0, "SensorModel");
    qmlRegisterUncreatableType<ValueList>("Sensors", 1, 0, "ValueList", QStringLiteral("Sensors should not be created in QML"));

    sensor_list_.appendItem(QStringLiteral("Wheel enc left"));
    sensor_list_.appendItem(QStringLiteral("Wheel enc right"));
    sensor_list_.appendItem(QStringLiteral("Distance left"));
    sensor_list_.appendItem(QStringLiteral("Distance right"));
    sensor_list_.appendItem(QStringLiteral("Line left"));
    sensor_list_.appendItem(QStringLiteral("Line right"));
    sensor_list_.appendItem(QStringLiteral("Border left"));
    sensor_list_.appendItem(QStringLiteral("Border right"));
    sensor_list_.appendItem(QStringLiteral("Light left"));
    sensor_list_.appendItem(QStringLiteral("Light right"));
    sensor_list_.appendItem(QStringLiteral("Mouse dX"));
    sensor_list_.appendItem(QStringLiteral("Mouse dY"));
    sensor_list_.appendItem(QStringLiteral("Door"));
    sensor_list_.appendItem(QStringLiteral("Transport pocket"));
    sensor_list_.appendItem(QStringLiteral("RC-5"));
    sensor_list_.appendItem(QStringLiteral("BPS"));
    sensor_list_.appendItem(QStringLiteral("Error"));

    sensor_model_.setList(&sensor_list_);
    p_engine->rootContext()->setContextProperty(QStringLiteral("sensorModel"), &sensor_model_);

    for (int i {}; i < sensor_model_.rowCount(); ++i) {
        const auto& e { sensor_model_.data(sensor_model_.index(i, 0), ValueModel::Name) };
        sensor_map_[e.toString()] = sensor_model_.index(i, 0);
    }


    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_IR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_IR received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["Distance left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensor_model_.setData(sensor_map_["Distance right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_ENC, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_ENC received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["Wheel enc left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensor_model_.setData(sensor_map_["Wheel enc right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_BORDER, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_BORDER received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["Border left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensor_model_.setData(sensor_map_["Border right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_LINE, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_LINE received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["Line left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensor_model_.setData(sensor_map_["Line right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_LDR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_LDR received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["Light left"], cmd.get_cmd_data_l(), ValueModel::Value);
        sensor_model_.setData(sensor_map_["Light right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_TRANS, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_TRANS received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["Transport pocket"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_DOOR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_DOOR received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["Door"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_RC5, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_RC5 received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["RC-5"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_BPS, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_BPS received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["BPS"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_SENS_ERROR, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_SENS_ERROR received: " << cmd << "\n";
        sensor_model_.setData(sensor_map_["Error"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });
}
