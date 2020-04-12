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
 * @file    command_evaluator.cpp
 * @brief   Command management for ct-Bot commands
 * @author  Timo Sandmann
 * @date    11.04.2020
 */

#include <QDebug>

#include "command_evaluator.h"


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
