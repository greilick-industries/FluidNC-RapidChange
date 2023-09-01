#include "ToolChange.h"

void user_select_tool(uint8_t new_tool) {
    current_tool = new_tool;
}

void user_tool_change(uint8_t new_tool) {
    rapid_change = config->_rapidChange;
    char validation_message[256];
    strcpy(validation_message, rapid_change->get_validation_message());
    if (validation_message[0] != 'o') {
        log_info(validation_message);
        rapid_change = nullptr;
        return;
    }
    message_start();
    if (current_tool == new_tool) {
        log_info("TOOL CHANGE BYPASSED. SELECTED TOOL IS THE CURRENT TOOL.");
        rapid_change = nullptr;
        return;
    }
    
    protocol_buffer_synchronize(); 
    record_state();
    set_rapid_change_state();
    drop_tool();
    get_tool(new_tool);
    set_tool(new_tool);
    go_to_z(rapid_change->safe_clearance_z_);
    operate_dust_cover(CLOSE_DUST_COVER);
    restore_state();
        
    rapid_change = nullptr;
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

void go_to_touch_probe_xy() {
    float x_pos = rapid_change->get_touch_probe_pos(X_AXIS);
    float y_pos = rapid_change->get_touch_probe_pos(Y_AXIS);
    execute_linef(true, "G53 G0 X%5.3f Y%5.3f", x_pos, y_pos);
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
    sprintf(tool_msg, "CURRENT TOOL: %d", current_tool);
    log_info(tool_msg);
    sprintf(tool_msg, "SELECTED TOOL: %d", gc_state.tool);
    log_info(tool_msg);
}

void operate_dust_cover(bool state) {
    if (rapid_change->dust_cover_use_axis_) {
        operate_dust_cover_axis(state);
    } else {
        rapid_change->operate_dust_cover_pin(state);
    }
}

void operate_dust_cover_axis(bool isOpening) {
    char letter;
    switch (rapid_change->dust_cover_axis_) {
        case RapidChange::RapidChange::B_AXIS:
            letter = 'B';
            break;
        case RapidChange::RapidChange::C_AXIS:
            letter = 'C';
            break;
        default:
            letter = 'A';
            break;
    }
    float position = isOpening ? rapid_change->dust_cover_pos_open_ : rapid_change->dust_cover_pos_closed_;
    execute_linef(true, "G53 G1 %c%5.3f F%d", letter, position, rapid_change->dust_cover_feedrate);
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
        operate_dust_cover(OPEN_DUST_COVER);
        return;
    }
    // if the tool has a pocket perform automatic drop
    if (rapid_change->tool_has_pocket(current_tool)) {
        uint8_t attempts = 0;
        do {
            go_to_tool_xy(current_tool);
            operate_dust_cover(OPEN_DUST_COVER);
            execute_linef(false, "G4 P0.5");
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
            log_info("TOOL FAILED TO DROP. EXECUTION IS PAUSED.");
            log_info("PLEASE REMOVE THE TOOL MANUALLY AND CYCLE START TO CONTINUE.");
            execute_linef(true, "M0");
        }           
    
    } else {  // if the tool doesn't have a pocket go to manual position and pause
        go_to_tool_xy(current_tool);
        operate_dust_cover(OPEN_DUST_COVER);
        log_info("THIS TOOL REQUIRES MANUAL REMOVAL.");
        log_info("PLEASE REMOVE THE TOOL MANUALLY AND CYCLE START TO CONTINUE.");
        execute_linef(true, "M0");
    }
    // tool has been removed, set current tool to 0
    current_tool = 0;
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
        execute_linef(true, "G4 P0.25");



        if (!rapid_change->disable_tool_recognition_ && !spindle_has_tool()) {
            go_to_z(rapid_change->safe_clearance_z_);
            go_to_tool_xy(0);
            spin_stop();
            log_info("SPINDLE FAILED TO PICK UP THE SELECTED TOOL.");
            log_info("PLEASE ATTACH THE SELECTED TOOL AND CYCLE START TO CONTINUE.");
            execute_linef(true, "M0");
        } else {
            go_to_z(rapid_change->tool_recognition_z_ + 5);
            execute_linef(true, "G4 P1");
            if (!rapid_change->disable_tool_recognition_ && spindle_has_tool()) {
                go_to_z(rapid_change->safe_clearance_z_);
                go_to_tool_xy(0);
                spin_stop();
                log_info("SELECTED TOOL DID NOT THREAD CORRECTLY.");
                log_info("PLEASE REMOVE AND REATTACH THE SELECTED TOOL AND CYCLE START TO CONTINUE.");
                execute_linef(true, "M0");
            } 
        }

    // if selected tool is not in the magazine go to manual position and pause
    } else {
        go_to_z(rapid_change->safe_clearance_z_);
        go_to_tool_xy(tool_num);
        log_info("SELECTED TOOL IS NOT IN THE MAGAZINE.");
        log_info("PLEASE ATTACH THE SELECTED TOOL AND PRESS CYCLE START TO CONTINUE.");
        execute_linef(true, "M0");
    }
    current_tool = tool_num;
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
}

void set_tool_infrared() {
    execute_linef(true, "G38.2 G91 F%d Z%5.3f", rapid_change->infrared_probe_feedrate_, rapid_change->safe_clearance_z_ - rapid_change->tool_recognition_z_);
    execute_linef(true, "G10 L20 P0 Z%5.3f", rapid_change->infrared_tool_setter_z_);
    execute_linef(false, "G90 G0");
    spin_stop();
}

void set_tool_touch() {
    int direction_multiplier = rapid_change->z_direction_ = RapidChange::RapidChange::direction::POSITIVE ? 1 : -1;
    spin_stop();
    go_to_z(rapid_change->go_to_touch_probe_z_);
    go_to_touch_probe_xy();
    go_to_z(rapid_change->touch_probe_start_z_);
    execute_linef(true, "G38.2 G91 F%d Z%5.3f", rapid_change->touch_probe_feedrate_initial_, -1 * direction_multiplier * rapid_change->touch_probe_max_distance_);
    execute_linef(true, "G91 G0 Z%d", direction_multiplier * 2);
    execute_linef(true, "G38.2 G91 F%d Z%5.3f", rapid_change->touch_probe_feedrate_, -1 * direction_multiplier * 2.5);
    execute_linef(true, "G10 L20 P0 Z%5.3f", rapid_change->touch_tool_setter_z_);
    execute_linef(false, "G90 G0");
}

bool spindle_has_tool() {
    if (rapid_change->disable_tool_recognition_) {
        return false;
    } else if (rapid_change->probe_ == RapidChange::RapidChange::probe::INFRARED) {
        return !(config->_probe->get_state()); // If not triggered we have a tool
    } else {
        return (rapid_change->infrared_pin_.read());
    }
}

void user_M30() {
    // TODO: call user_tool_change? this doesn't work
    if (current_tool != 0) {
        execute_linef(true, "M6 T0");
    }
}
