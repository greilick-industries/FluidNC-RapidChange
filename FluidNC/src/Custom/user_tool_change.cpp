#pragma once

#include "GCodeLine.h"
#include "../GCode.h"
#include "../Machine/MachineConfig.h"

void user_tool_change(uint8_t new_tool) {
    if (config->_rapidChange) {
        config->_rapidChange->performToolChange(new_tool);
    } else {
        //gc_execute_line()
        //RapidChange::GCodeLine::Write().PauseExecution();
        log_error("RapidChange was not configured. Check config.yaml for proper configuration");
    }
}
