#include "../GCode.h"
#include "../Machine/MachineConfig.h"


void machine_init() {

}

void user_tool_change(uint8_t new_tool) {
    config->_rapidChange->performToolChange(new_tool);
}
