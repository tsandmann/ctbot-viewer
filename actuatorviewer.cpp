#include <QString>
#include <QQmlContext>
#include <QRegularExpression>
#include <QDebug>
#include <iostream>

#include "actuatorviewer.h"
#include "commandevaluator.h"
#include "command.h"


ActuatorViewer::ActuatorViewer(QQmlApplicationEngine* p_engine, CommandEvaluator& command_eval) : p_engine_ { p_engine }, p_lcd_ {} {
    qmlRegisterType<ValueModel>("Actuators", 1, 0, "ActuatorModel");
    qmlRegisterUncreatableType<ValueList>("Actuators", 1, 0, "ValueList", QStringLiteral("Actuators should not be created in QML"));

    actuator_list_.appendItem(QStringLiteral("Motor left"));
    actuator_list_.appendItem(QStringLiteral("Motor right"));
    actuator_list_.appendItem(QStringLiteral("LEDs"));

    actuator_model_.setList(&actuator_list_);
    p_engine_->rootContext()->setContextProperty(QStringLiteral("actuatorModel"), &actuator_model_);

    for (int i {}; i < actuator_model_.rowCount(); ++i) {
        const auto& e { actuator_model_.data(actuator_model_.index(i, 0), ValueModel::Name) };
        actuator_map_[e.toString()] = actuator_model_.index(i, 0);
    }

    command_eval.register_cmd(ctbot::CommandCodes::CMD_AKT_MOT, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_MOT received: " << cmd << "\n";
        actuator_model_.setData(actuator_map_["Motor left"], cmd.get_cmd_data_l(), ValueModel::Value);
        actuator_model_.setData(actuator_map_["Motor right"], cmd.get_cmd_data_r(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_AKT_LED, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_LED received: " << cmd << std::endl;
        actuator_model_.setData(actuator_map_["LEDs"], cmd.get_cmd_data_l(), ValueModel::Value);
        return true;
    });

    command_eval.register_cmd(ctbot::CommandCodes::CMD_AKT_LCD, [this](const ctbot::CommandBase& cmd) {
        // std::cout << "CMD_AKT_LCD received: " << cmd << "\n";

        // FIXME: just a test implementation!
        static char text[4][21];

        if (!p_lcd_) {
            p_lcd_ = p_engine_->rootObjects().first()->findChild<QObject*>("LCD");
            if (!p_lcd_) {
                return false;
            }
        }

        switch (cmd.get_cmd_subcode()) {
            case ctbot::CommandCodes::CMD_SUB_LCD_CLEAR: {
                // qDebug() << "Display CLEAR received.";
                std::memset(&text[0][0], ' ', sizeof(text));
                text[0][20] = 0;
                text[1][20] = 0;
                text[2][20] = 0;
                text[3][20] = 0;

                QString data = text[0];
                data += "\n";
                data += text[1];
                data += "\n";
                data += text[2];
                data += "\n";
                data += text[3];

                p_lcd_->setProperty("text", data);

                return true;
            }

            case ctbot::CommandCodes::CMD_SUB_LCD_DATA: {
                // std::string payload { reinterpret_cast<const char*>(cmd.get_payload().data()), cmd.get_payload_size() };
                // qDebug() << "Display DATA received: col =" << cmd.get_cmd_data_l() << " row = " << cmd.get_cmd_data_r() << " len = " <<
                // cmd.get_payload().size() << " text = " << QString::fromStdString(payload);

                size_t col { static_cast<size_t>(cmd.get_cmd_data_l()) };
                if (col >= 20) {
                    qDebug() << "invalid display command received: col =" << col;
                    //  std::cout << cmd << std::endl;
                    col = 19;
                }
                size_t row { static_cast<size_t>(cmd.get_cmd_data_r()) };
                if (row >= 4) {
                    qDebug() << "invalid display command received: row =" << row;
                    //  std::cout << cmd << std::endl;
                    return false;
                }

                size_t len { cmd.get_payload_size() };
                if (len + col > 20) {
                    // qDebug() << "len updated: len =" << len << " col =" << col;
                    len = 20 - col;
                }
                if (!len) {
                    qDebug() << "invalid display command received: len =" << len << " col =" << col;
                    std::cout << cmd << std::endl;
                    return false;
                }

                std::strncpy(&text[row][col], reinterpret_cast<const char*>(cmd.get_payload().data()), len);
                text[row][col + len] = 0;

                QString data = text[0];
                data += "\n";
                data += text[1];
                data += "\n";
                data += text[2];
                data += "\n";
                data += text[3];
                data.replace(QRegularExpression("[\001-\011]"), ".");
                data.replace(QRegularExpression("[\013-\037]"), ".");
                data.replace(QRegularExpression("[\177-\377]"), "#");
                // qDebug() << "data = " << data;

                p_lcd_->setProperty("text", data);

                return true;
            }

            default: return false;
        }
    });
}
