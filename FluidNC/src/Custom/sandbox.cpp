#include "../GCode.h"
#include "RapidChangeConfig.h"
#include "../Machine/MachineConfig.h"


void machine_init() {

}

void user_tool_change(uint8_t new_tool) {
    RapidChangeConfig& rapid_change_config = *(config->_rapid_change);
    rapid_change_config.execute();
}

