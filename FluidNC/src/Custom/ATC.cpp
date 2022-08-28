#include "../GCode.h"
#include "../Protocol.h"
#include "../Serial.h"
#include "../Settings.h"
#include "../Spindles/Spindle.h"
#include "../Uart.h"
#include "../WebUI/InputBuffer.h"
#include <stdarg.h>

const uint8_t TOOL_COUNT = 5;
uint8_t current_tool = 0;

void convert_sys_mpos_to_array(float array_to_fill[MAX_N_AXIS]);
void gc_exec_linef(bool sync_after, const char* format, ...);
bool return_tool(uint8_t tool_num);

void machine_init() {

}

void user_tool_change(uint8_t new_tool) {
    bool was_incremental = false;
    float saved_mpos[MAX_N_AXIS] = {};

    if (new_tool == current_tool) {
        log_info("Existing tool requested.");
        return;
    }

    if (new_tool > TOOL_COUNT) {
        Error tool_number_error = Error::InvalidValue;
        report_status_message(tool_number_error, allChannels);
        log_error("Tool requested is out of range.");
        return;
    }

    protocol_buffer_synchronize();
    convert_sys_mpos_to_array(saved_mpos);

    // Turn off spindle if its on
    gc_exec_linef(false, "M5");

    if (gc_state.modal.distance == Distance::Incremental) {
        was_incremental = true;
        gc_exec_linef(false, "G90");
    }

}

void convert_sys_mpos_to_array(float array_to_fill[MAX_N_AXIS]) {
    float* sys_mpos = get_mpos();
    for (uint8_t i = 0; i < MAX_N_AXIS; i++) {
        array_to_fill[i] = *(sys_mpos + i);
    }
}

void gc_exec_linef(bool sync_after, const char* format, ...) {
    va_list args;
    char gc_line[20];
    gc_line[strlen(format)] = '\r';
    sprintf(gc_line, format, args);
    Error line_executed = execute_line(gc_line, allChannels, WebUI::AuthenticationLevel::LEVEL_GUEST);
    report_status_message(line_executed, allChannels);
}

bool return_tool(uint8_t tool_num) {
    if (tool_num == 0) {
        return false;
    }
    return true;
}
