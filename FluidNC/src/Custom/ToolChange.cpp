#include "ToolChange.h"

const uint8_t TOOL_COUNT = 6;
uint8_t current_tool;

const float TOP_OF_Z = 127;
const char* tool_xy_gcode[7];

void machine_init() {   
    tool_xy_gcode[1] = "G53 G0 X152.5 y435.06";
    tool_xy_gcode[2] = "G53 G0 X197.5 y435.06";
    tool_xy_gcode[3] = "G53 G0 x242.5 y435.06";
    tool_xy_gcode[4] = "G53 G0 X287.5 y435.06";
    tool_xy_gcode[5] = "G53 G0 X332.5 y435.06";
    tool_xy_gcode[6] = "G53 G0 X377.5 y435.06";
}

void user_tool_change(uint8_t new_tool) {
    record_state();
    set_tool_change_state();
    // preferences.begin("RapidChange", false);

    // if (!preferences.isKey("currentTool")) {
    //     preferences.putUInt("currentTool", 0);
    // }
    // current_tool = preferences.getUInt("currentTool");
    // log_info("Current tool:");
    // log_info(preferences.getUInt("currentTool"));

    
    // float saved_mpos[MAX_N_AXIS] = {};

    // if (new_tool == current_tool) {
    //     log_info("Existing tool requested.");
    //     return;
    // }

    // if (new_tool > TOOL_COUNT) {
    //     Error tool_number_error = Error::InvalidValue;
    //     report_status_message(tool_number_error, allChannels);
    //     log_error("Tool requested is out of range.");
    //     return;
    // }

    // protocol_buffer_synchronize();

    // // Turn off spindle if its on
    // execute_linef(false, "M5");
    
    // // Record current gcode parser state and then set to atc state
    

    // return_tool(current_tool);
    // pickup_tool(new_tool);

    // if (was_incremental) {
    //     execute_linef(false, "G91");
    // }

    // if (was_inches) {
    //     execute_linef(false, "G20");
    // }
    // log_info("Tool change complete.");
    // log_info("Stored current tool:");
    // log_info(preferences.getUInt("currentTool"));
    // preferences.end();
}

void execute_linef(bool sync_after, const char* format, ...) {
    va_list args;
    char gc_line[30];
    gc_line[strlen(format)] = '\r';
    sprintf(gc_line, format, args);
    Error line_executed = execute_line(gc_line, allChannels, WebUI::AuthenticationLevel::LEVEL_GUEST);
    report_status_message(line_executed, allChannels);
    
    if (sync_after) {
        protocol_buffer_synchronize();
    }
}

void record_state() {
    stored_state.coolant = gc_state.modal.coolant;
    stored_state.distance = gc_state.modal.distance;
    stored_state.feed_rate = gc_state.feed_rate;
    stored_state.feed_rate_mode = gc_state.modal.feed_rate;
    stored_state.motion = gc_state.modal.motion;
    stored_state.units = gc_state.modal.units;
}

void restore_state() {
    if (stored_state.coolant.Flood = 1) {
        execute_linef(false, "M8");
    } else if (stored_state.coolant.Mist = 1) {
        execute_linef(false, "M7");
    }
    if (stored_state.distance == Distance::Incremental) {
        execute_linef(false, "G91");
    }
    if (stored_state.feed_rate_mode == FeedRate::InverseTime) {
        execute_linef(false, "G93");
    }
    if (stored_state.units == Units::Inches) {
        execute_linef(false, "G20");
    }
    execute_linef(true, "F%f", stored_state.feed_rate);
}

void set_tool_change_state() {
    execute_linef(false, "M5 M9 G0 G21 G90 G94");
}

void return_tool(uint8_t tool_num) {
    execute_linef(true, "G53 G1 Z127 F2000");
    if (tool_num == 0) {
        return;
    }

    execute_linef(true, tool_xy_gcode[tool_num]);
    execute_linef(true, "G53 G0 A20");
    execute_linef(true, "G53 Z30");
    uint8_t attempts = 0;
    bool tool_dropped = false;

    do {
        tool_dropped = drop_tool(tool_num);
        attempts++;
    } 
    while (!tool_dropped && attempts < 2);
    
    if (!tool_dropped) {
        execute_linef(false, "M5");
        execute_linef(false, "M0");
        log_info("Tool drop failed. Program is paused.");
        log_info("Remove tool manually and restart to continue.");
    }    
    current_tool = 0;
    preferences.putUInt("currentTool", 0);
}

bool drop_tool(uint8_t tool_num) {
    
    execute_linef(false, "S1200 M4");
    execute_linef(false, "G53 G1 Z9 F1200");
    execute_linef(true, "G53 G1 Z46 F1200");
        
    return config->_probe->get_state();
}



void pickup_tool(uint8_t tool_num) {
    if (current_tool != 0) {
        report_status_message(Error::GcodeInvalidTarget, allChannels);
    }
    execute_linef(true, tool_xy_gcode[tool_num]);
    execute_linef(false, "M5");
    execute_linef(false, "G53 G0 A20");
    execute_linef(true, "G53 G90 Z30");
    execute_linef(false, "S1200 M3");
    execute_linef(false, "G53 G1 Z9 F1200");
    execute_linef(false, "G53 G1 Z15 F1200");
    execute_linef(false, "G53 G1 Z8 F1200");
    execute_linef(true, "G53 G0 Z76");
    execute_linef(false, "S12000");
    execute_linef(true, "G38.2 G91 F300 Z57");
    execute_linef(true, "G0 G91 Z-2");
    execute_linef(true, "G38.2 G91 F25 Z57");
    execute_linef(false, "G10 L20 P0 Z74.5898");
    execute_linef(false, "M5");
    execute_linef(false, "G53 G0 G90 Z127");
    execute_linef(false, "G53 G0 A0");
    current_tool = tool_num;
    preferences.putUInt("currentTool", tool_num);
    
}

