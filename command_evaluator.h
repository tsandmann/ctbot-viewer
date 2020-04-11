#pragma once

#include <map>
#include <vector>
#include <functional>

#include "command.h"

class CommandEvaluator {
    std::map<ctbot::CommandCodes /*cmd*/, std::vector<std::function<bool(const ctbot::CommandBase&)>> /*functions*/> commands_;

public:
    CommandEvaluator();

    void register_cmd(const ctbot::CommandCodes& cmd, std::function<bool(const ctbot::CommandBase&)>&& func);

    bool evaluate(const ctbot::CommandNoCRC* p_cmd) const;
};
