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

void convert_sys_mpos_to_array(float (&array_to_fill)[MAX_N_AXIS]);
void gc_exec_linef(bool sync_after, const char* format, ...);
bool return_tool(uint8_t tool_num);

void machine_init() {

}

void test_array_conversion(float (&array_to_fill)[MAX_N_AXIS]) {
        try {
            char coords[30];
            sprintf(coords, "X: %0.3f Y: %0.3f Z: %0.3f", array_to_fill[X_AXIS], array_to_fill[Y_AXIS], array_to_fill[Z_AXIS]);

            log_info(coords);
        }
        catch(...) {
            log_info("Something went wrong");
        }
    }

void user_tool_change(uint8_t new_tool) {
    bool spindle_was_on = false;
    bool was_incremental = false;
    uint64_t spindle_spin_delay;
    float saved_mpos[MAX_N_AXIS] = {};

    // if (new_tool == current_tool) {
    //     // TODO log
    //     return;
    // }

    // if (new_tool > TOOL_COUNT) {
    //     // TODO log
    //     return;
    // }

    // protocol_buffer_synchronize();
    convert_sys_mpos_to_array(saved_mpos);
    test_array_conversion(saved_mpos);
    

    

    // if (gc_state.modal.distance == Distance::Incremental) {
    //     was_incremental = true;
    //     gc_exec_linef(false, "G90");
    // }

    // if (gc_state.modal.spindle != SpindleState::Disable) {
    //     spindle_was_on = true;
    //     gc_exec_linef(false, "M5");
    //     spindle_spin_delay = esp_timer_get_time() + (spindle->_spindown_ms * 1000);

    //     uint64_t current_time = esp_timer_get_time();
    //     if (current_time < spindle_spin_delay) {
    //         vTaskDelay(spindle_spin_delay - current_time);
    //     }
    // }
    
    
}

void convert_sys_mpos_to_array(float (&array_to_fill)[MAX_N_AXIS]) {
    float* sys_mpos = get_mpos();
    for (uint8_t i = 0; i < sizeof(array_to_fill); i++) {
        array_to_fill[i] = sys_mpos[i];
    }
}

// This comment is even newer
void gc_exec_linef(bool sync_after, const char* format, ...) {
    Channel& r_channel = WebUI::inputBuffer;
    va_list args;
    char gc_line[20];
    gc_line[strlen(format)] = '\r';
    sprintf(gc_line, format, args);
    Error line_executed = execute_line(gc_line, r_channel, WebUI::AuthenticationLevel::LEVEL_GUEST);
    r_channel << ">" << gc_line << ":";
    report_status_message(line_executed, r_channel);
}

bool return_tool(uint8_t tool_num) {
    if (tool_num == 0) {
        return false;
    }
    return true;
}
