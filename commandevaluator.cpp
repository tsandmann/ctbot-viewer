#include <QDebug>

#include "commandevaluator.h"


CommandEvaluator::CommandEvaluator() {}

void CommandEvaluator::register_cmd(const ctbot::CommandCodes& cmd, std::function<bool(const ctbot::CommandBase&)>&& func) {
    commands_[cmd].emplace_back(func);
}

bool CommandEvaluator::evaluate(const ctbot::CommandNoCRC* p_cmd) const {
    try {
        bool result { true };
        for (auto& func : commands_.at(p_cmd->get_cmd_code())) {
            result &= func(*p_cmd);
        }
        return result;
    } catch (const std::out_of_range&) {
        qDebug() << "CMD code '" << static_cast<char>(p_cmd->get_cmd_code_uint()) << "' not registered:";
        std::cout << *p_cmd << std::endl;
        return false;
    }
}
