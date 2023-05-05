#include "ToolChange.h"

void user_tool_change(uint8_t new_tool) {
    if (!current_tool_fetched) {
        fetch_current_tool();
    }
    
    message_start();
    if (current_tool == new_tool) {
        log_info("Tool change bypassed. Selected tool is the current tool.");
        return;
    }
    
    rapid_change = config->_rapidChange;
    
    protocol_buffer_synchronize(); 
    record_state();
    set_rapid_change_state();
    operate_dust_cover(OPEN_DUST_COVER);
    drop_tool();
    get_tool(new_tool);
    set_tool(new_tool);
    go_to_z(rapid_change->safe_clearance_z_);
    operate_dust_cover(CLOSE_DUST_COVER);
    restore_state();
        
    rapid_change = nullptr;
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

void go_to_tool_xy(uint8_t tool_num) {
    float x_pos = rapid_change->get_tool_pos(X_AXIS, tool_num);
    float y_pos = rapid_change->get_tool_pos(Y_AXIS, tool_num);
    execute_linef(true, "G53 G0 X%5.3f Y%5.3f", x_pos, y_pos);
}

void go_to_z(float position) {
    execute_linef(true, "G53 G0 Z%5.3f", position);
}

void go_to_z(float position, int feedrate) {
    execute_linef(true, "G53 G1 Z%5.3f F%d", position, feedrate);
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
        rapid_change->dust_cover_pin_.on();
    } else {
        rapid_change->dust_cover_pin_.off();
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

void set_rapid_change_state() {
    execute_linef(true, "M5 M9 G0 G21 G90 G94");
}

void drop_tool() {
    go_to_z(rapid_change->safe_clearance_z_);
    // if we don't have a tool we're done
    if (current_tool == 0) {
        return;
    }
    // if the tool has a pocket perform automatic drop
    if (rapid_change->tool_has_pocket(current_tool)) {
        uint8_t attempts = 0;
        do {
            go_to_tool_xy(current_tool);
            go_to_z(rapid_change->spindle_start_z_);
            spin_ccw(rapid_change->spin_speed_engage_ccw_);
            go_to_z(rapid_change->engage_z_, rapid_change->engage_feedrate_);
            go_to_z(rapid_change->tool_recognition_z_);
            spin_stop();
            attempts++;
        } while (spindle_has_tool() && attempts < 2);
        // if tool didn't drop after 2 attempts go to manual change position and pause
        if (spindle_has_tool()) {
            go_to_z(rapid_change->safe_clearance_z_);
            go_to_tool_xy(0);
            log_info("Tool failed to drop. Execution is paused.");
            log_info("Please remove the tool manually and cycle start to continue");
            execute_linef(true, "M0");
        }           
    
    } else {  // if the tool doesn't have a pocket go to manual position and pause
        go_to_tool_xy(current_tool);
        log_info("This tool requires manual removal.");
        log_info("Please remove the tool manually and cycle start to continue");
        execute_linef(true, "M0");
    }
    // tool has been removed, set current tool to 0
    current_tool = 0;
    preferences.putUInt(CURRENT_TOOL_KEY, 0);
}

void spin_cw(int speed) {
    execute_linef(false, "M3 S%d", speed);
}

void spin_ccw(int speed) {
    execute_linef(false, "M4 S%d", speed);
}

void spin_stop() {
    execute_linef(false, "M5");
}

void get_tool(uint8_t tool_num) {
    // sanity check
    if (current_tool != 0) {
        report_status_message(Error::GcodeInvalidTarget, allChannels);
    }
    // if selected tool is tool 0, set current tool to 0 and we're done
    if (tool_num == 0) {
        current_tool = tool_num;
        preferences.putUInt(CURRENT_TOOL_KEY, tool_num);
        return;
    }

    // if selected tool is in the magazine, perform automatic pick up
    if (rapid_change->tool_has_pocket(tool_num)) {
        go_to_tool_xy(tool_num);
        go_to_z(rapid_change->spindle_start_z_);
        spin_cw(rapid_change->spin_speed_engage_cw_);
        go_to_z(rapid_change->engage_z_, rapid_change->engage_feedrate_);
        go_to_z(rapid_change->back_off_engage_z_);
        go_to_z(rapid_change->engage_z_, rapid_change->engage_feedrate_);
        go_to_z(rapid_change->tool_recognition_z_);
        if (!rapid_change->disable_tool_recognition_ && !spindle_has_tool()) {
            go_to_z(rapid_change->safe_clearance_z_);
            go_to_tool_xy(0);
            execute_linef(true, "M0");
            log_info("Spindle failed to pick up the selected tool.");
            log_info("Please attach the selected tool and press cycle start to continue.");
        }
    
    // if selected tool is not in the magazine go to manual position and pause
    } else {
        go_to_z(rapid_change->safe_clearance_z_);
        go_to_tool_xy(tool_num);
        execute_linef(true, "M0");
        log_info("Selected tool is not in the magazine.");
        log_info("Please attach the selected tool and press cycle start to continue.");
    }
    current_tool = tool_num;
    preferences.putUInt(CURRENT_TOOL_KEY, tool_num);
    
}

void set_tool(uint8_t tool_num) {
    // if selected tool is 0, we're done
    if (tool_num == 0) {
        spin_stop();
        return;
    }
    // switch on probe config
    switch (rapid_change->probe_) {
        case RapidChange::RapidChange::probe::NONE:
            spin_stop();
            return;
        case RapidChange::RapidChange::probe::INFRARED:
            set_tool_infrared();
            return;
        case RapidChange::RapidChange::probe::TOUCH:
            set_tool_touch();
            return;
        default:
            return;
    }

    if (rapid_change->probe_ == RapidChange::RapidChange::probe::INFRARED)
    execute_linef(true, "G38.2 G91 F300 Z57");
    execute_linef(true, "G0 G91 Z-2");
    execute_linef(true, "G38.2 G91 F25 Z57");
    execute_linef(false, "G10 L20 P0 Z74.5898");
}

void set_tool_infrared() {}

void set_tool_touch() {
    spin_stop();
    go_to_z(rapid_change->go_to_touch_probe_z_);
    go_to_tool_xy(0);
    go_to_z(rapid_change->touch_probe_start_z_);
    execute_linef(true, "G38.2 G91 F%d Z%5.3f", rapid_change->touch_probe_feedrate_, -1 * rapid_change->touch_probe_max_distance_);
    execute_linef(false, "G10 L20 P0 Z%5.3f", rapid_change->touch_tool_setter_z_);
}

bool spindle_has_tool() {
    if (rapid_change->disable_tool_recognition_) {
        return false;
    } else {
        return !(rapid_change->infrared_pin_.read());
    }
}
