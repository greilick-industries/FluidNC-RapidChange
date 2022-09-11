#include "../GCode.h"
#include "RapidChangeConfig.h"
#include "../Machine/MachineConfig.h"


void machine_init() {

}

void user_tool_change(uint8_t new_tool) {
    log_info(config->_rapid_change->x_mpos_);
}
