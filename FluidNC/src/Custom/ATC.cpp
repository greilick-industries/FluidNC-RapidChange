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
const float TOP_OF_Z = 80.0;
// struct Tool {
//     float mpos[MAX_N_AXIS];
// };

// Tool tools[MAX_N_AXIS];

const char* tool_xy_gcode[6];


void convert_sys_mpos_to_array(float array_to_fill[MAX_N_AXIS]);
void gc_exec_linef(bool sync_after, const char* format, ...);
void return_tool(uint8_t tool_num);
void pickup_tool(uint8_t tool_num);

void machine_init() {
    // tools[1].mpos[X_AXIS] = 0.0;
    // tools[1].mpos[Y_AXIS] = 0.0;
    // tools[2].mpos[X_AXIS] = 57.15;
    // tools[2].mpos[Y_AXIS] = 0.0;
    // tools[3].mpos[X_AXIS] = 114.3;
    // tools[3].mpos[Y_AXIS] = 0.0;
    // tools[4].mpos[X_AXIS] = 171.45;
    // tools[4].mpos[Y_AXIS] = 0.0;
    // tools[5].mpos[X_AXIS] = 228.6;
    // tools[5].mpos[Y_AXIS] = 0.0;
    tool_xy_gcode[1] = "G53 G0 X0 Y0";
    tool_xy_gcode[2] = "G53 G0 X57.15 Y0";
    tool_xy_gcode[3] = "G53 G0 X114.3 Y0";
    tool_xy_gcode[4] = "G53 G0 X171.45 Y0";
    tool_xy_gcode[5] = "G53 G0 X228.6 Y0";
}

void user_tool_change(uint8_t new_tool) {
    bool units_were_inches = false;
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

    // Record current gcode parser state and then set to atc state
    if (gc_state.modal.distance == Distance::Incremental) {
        was_incremental = true;
        gc_exec_linef(false, "G90");
    }

    if (gc_state.modal.units == Units::Inches) {
        units_were_inches = true;
        gc_exec_linef(false, "G21");
    }

    return_tool(current_tool);
    pickup_tool(new_tool);

    // Move to previous XY
    gc_exec_linef(false, "G53 X%0.3f Y%0.3f", saved_mpos[X_AXIS], saved_mpos[Y_AXIS]);
    // TODO check for work area and move to previous

    // Reset previous gcode parser state
    if (was_incremental) {
        gc_exec_linef(false, "G91");
    }

    if (units_were_inches) {
        gc_exec_linef(false, "G20");
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
    char gc_line[30];
    gc_line[strlen(format)] = '\r';
    sprintf(gc_line, format, args);
    // Error line_executed = execute_line(gc_line, allChannels, WebUI::AuthenticationLevel::LEVEL_GUEST);
    // report_status_message(line_executed, allChannels);
    WebUI::inputBuffer.push(gc_line);
}

void return_tool(uint8_t tool_num) {
    gc_exec_linef(false, "G53 G1 Z80 F2540");
    
    if (tool_num == 0) {
        return;
    }
    
    gc_exec_linef(false, tool_xy_gcode[tool_num]);
    gc_exec_linef(false, "G53 Z10");
    gc_exec_linef(false, "S800 M4");
    gc_exec_linef(false, "G53 G1 Z.05 F760");
    gc_exec_linef(false, "G4 P.5");
    gc_exec_linef(false, "G53 Z5");
    gc_exec_linef(false, "M5");
    gc_exec_linef(false, "G53 G0 Z80");
    current_tool = 0;
}

void pickup_tool(uint8_t tool_num) {
    if (current_tool != 0) {
        report_status_message(Error::GcodeInvalidTarget, allChannels);
    }

    gc_exec_linef(false, tool_xy_gcode[tool_num]);
    gc_exec_linef(false, "G53 Z0");
    gc_exec_linef(false, "S400 M4");
    gc_exec_linef(false, "M5");
    gc_exec_linef(false, "G4 P1");
    gc_exec_linef(false, "S2000 M3");
    gc_exec_linef(false, "G53 G0 Z13");
    gc_exec_linef(false, "S800 M3");
    gc_exec_linef(false, "G53 G1 Z0 F1270");
    gc_exec_linef(false, "G53 G0 Z60");
    gc_exec_linef(false, "G4 P1");
    gc_exec_linef(false, "S8000 M3");
    gc_exec_linef(false, "G38.2 G91 F100 Z25");
    gc_exec_linef(false, "G21 G10 L20 P0 Z63.2968");
    gc_exec_linef(false, "M5");
    gc_exec_linef(false, "G53 G0 G90 Z80");
    current_tool = tool_num;
}
