#include "ToolChange.h"

const uint8_t TOOL_COUNT = 6;

const float TOP_OF_Z = 127;
const char* tool_xy_gcode[7];

void machine_init() {  
    
    // tool_xy_gcode[1] = "G53 G0 X152.5 y435.06";
    // tool_xy_gcode[2] = "G53 G0 X197.5 y435.06";
    // tool_xy_gcode[3] = "G53 G0 x242.5 y435.06";
    // tool_xy_gcode[4] = "G53 G0 X287.5 y435.06";
    // tool_xy_gcode[5] = "G53 G0 X332.5 y435.06";
    // tool_xy_gcode[6] = "G53 G0 X377.5 y435.06";
}

void user_tool_change(uint8_t new_tool) {
    if (!current_tool_fetched) {
        fetch_current_tool();
    }
    message_start();
    protocol_buffer_synchronize(); 
    record_state();
    set_tool_change_state();
    operate_dust_cover(OPEN_DUST_COVER);
    return_tool();

    restore_state();

    // if existing tool is requested there is nothing to do
    if (new_tool == current_tool) {
        log_info("Existing tool requested.");
        return;
    }

    if (new_tool > config->_rapidChange->get_max_tool_number()) {
        
    }

    

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

void fetch_current_tool() {
    current_tool_fetched = true;
    if (preferences.begin(RC_PREF_NAMESPACE, false)) {
        if (!preferences.isKey(CURRENT_TOOL_KEY) && !preferences.putUChar(CURRENT_TOOL_KEY, 0)) {
            log_error("Tool peristance error: Current tool will not persist beyond shutdown.");
            preferences.end();
            return;
        }
        current_tool = preferences.getUChar(CURRENT_TOOL_KEY);
    } else {
        log_error("Tool peristance error: Current tool will not persist beyond shutdown.");
    }
    preferences.end();
}

void execute_linef(bool sync_after, const char* format, ...) {
    char gc_line[30];
    va_list args;
    va_start(args, format);
    int length = vsprintf(gc_line, format, args); 
    
    // remove
    log_info(gc_line);   

    gc_line[length] = '\n';
    gc_line[length + 1] = '\0';
    va_end(args);
    
    report_status_message(
        execute_line(gc_line, allChannels, WebUI::AuthenticationLevel::LEVEL_GUEST), allChannels);
    
    if (sync_after) {
        protocol_buffer_synchronize();
    }
}

void go_above_tool(uint8_t tool_num) {
    float x_pos = config->_rapidChange->get_tool_pos(X_AXIS, tool_num);
    float y_pos = config->_rapidChange->get_tool_pos(Y_AXIS, tool_num);
    execute_linef(true, "G53 G0 X%5.3f Y%5.3f", x_pos, y_pos);
}

void go_to_safe_clearance() {
    execute_linef(true, "G53 G0 Z%5.3f", config->_rapidChange->safe_clearance_z_);
}

void message_start() {
    char tool_msg[20];
    sprintf(tool_msg, "Current Tool: %d", current_tool);
    log_info(tool_msg);
    sprintf(tool_msg, "Selected Tool: %d", gc_state.tool);
    log_info(tool_msg);
}

void operate_dust_cover(bool open) {
    if (open) {
        config->_rapidChange->dust_cover_pin_.on();
    } else {
        config->_rapidChange->dust_cover_pin_.off();
    }
}

void record_state() {
    stored_state.coolant = gc_state.modal.coolant;
    stored_state.distance = gc_state.modal.distance;
    stored_state.feed_rate_mode = gc_state.modal.feed_rate;
    stored_state.units = gc_state.modal.units;
}

void restore_state() {
    if (stored_state.coolant.Flood == 1) {
        execute_linef(false, "M8");
    } else if (stored_state.coolant.Mist == 1) {
        execute_linef(false, "M7");
    }
    if (stored_state.distance == Distance::Incremental) {
        execute_linef(false, "G91");
    }
    if (stored_state.feed_rate_mode == FeedRate::InverseTime) {
        execute_linef(false, "G93");
    }
    if (stored_state.units == Units::Inches) {
        execute_linef(true, "G20");
    }
}

void set_tool_change_state() {
    execute_linef(false, "M5 M9 G0 G21 G90 G94");
}

void return_tool() {
    go_to_safe_clearance();

    // if we don't have a tool we're done
    if (current_tool == 0) {
        return;
    }

    go_above_tool(current_tool);
    // execute_linef(true, "G53 G0 A20");
    // execute_linef(true, "G53 Z30");
    // uint8_t attempts = 0;
    // bool tool_dropped = false;

    // do {
    //     tool_dropped = drop_tool(tool_num);
    //     attempts++;
    // } 
    // while (!tool_dropped && attempts < 2);
    
    // if (!tool_dropped) {
    //     execute_linef(false, "M5");
    //     execute_linef(false, "M0");
    //     log_info("Tool drop failed. Program is paused.");
    //     log_info("Remove tool manually and restart to continue.");
    // }    
    // current_tool = 0;
    // preferences.putUInt(CURRENT_TOOL_KEY, 0);
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
    preferences.putUInt(CURRENT_TOOL_KEY, tool_num);
    
}

