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
    // bool spindle_was_on = false;
    // bool was_incremental = false;
    // uint64_t spindle_spin_delay;
    // float saved_mpos[MAX_N_AXIS] = {};

    if (new_tool == current_tool) {
        
        log_info("Existing tool requested.");
        return;
    }

    current_tool = new_tool;

    if (new_tool > TOOL_COUNT) {
        Error tool_number_error = Error::InvalidValue;
        report_status_message(tool_number_error, allChannels);
        log_error("Tool requested is out of range.");
        return;
    }

    // protocol_buffer_synchronize();
    
    // convert_sys_mpos_to_array(saved_mpos);

    // if (gc_state.modal.distance == Distance::Incremental) {
    //     was_incremental = true;
    //     gc_exec_linef(false, "G90");
    // }

    // if (gc_state.modal.spindle != SpindleState::Disable) {
    //     spindle_was_on = true;
    //     gc_exec_linef(false, "M5");


    //     // spindle_spin_delay = esp_timer_get_time() + (spindle->_spindown_ms * 1000);

    //     // uint64_t current_time = esp_timer_get_time();
    //     // if (current_time < spindle_spin_delay) {
    //     //     vTaskDelay(spindle_spin_delay - current_time);
    //     // }
    // }
}

void convert_sys_mpos_to_array(float array_to_fill[MAX_N_AXIS]) {
    float* sys_mpos = get_mpos();
    for (uint8_t i = 0; i < MAX_N_AXIS; i++) {
        array_to_fill[i] = *(sys_mpos + i);
    }
}

// This comment is even newer
void gc_exec_linef(bool sync_after, const char* format, ...) {
    // Channel& r_channel = WebUI::inputBuffer;
    va_list args;
    char gc_line[20];
    gc_line[strlen(format)] = '\r';
    sprintf(gc_line, format, args);
    Error line_executed = execute_line(gc_line, allChannels, WebUI::AuthenticationLevel::LEVEL_GUEST);
    // Uart0 << ">" << gc_line << ":";
    report_status_message(line_executed, allChannels);
}

bool return_tool(uint8_t tool_num) {
    if (tool_num == 0) {
        return false;
    }
    return true;
}
