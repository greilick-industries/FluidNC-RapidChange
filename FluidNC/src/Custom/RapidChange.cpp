#include "RapidChange.h"
#include "../GCode.h"
#include "../Protocol.h"

namespace RapidChange {
    EnumItem directions[] = {
        { RapidChange::POSITIVE, "POSITIVE" },
        { RapidChange::NEGATIVE, "NEGATIVE" },
        EnumItem(RapidChange::POSITIVE)
    };
    EnumItem orientations[] = {
        { RapidChange::X_AXIS, "X_AXIS" },
        { RapidChange::Y_AXIS, "Y_AXIS" },
        EnumItem(RapidChange::X_AXIS)
    };
    EnumItem collets[] = {
        { RapidChange::ER11, "ER11" },
        { RapidChange::ER16, "ER16" },
        { RapidChange::ER20, "ER20" },
        EnumItem(RapidChange::ER11)
    };
    EnumItem probes[] = {
        { RapidChange::NONE, "NONE" },
        { RapidChange::INFRARED, "INFRARED" },
        { RapidChange::TOUCH, "TOUCH" },
        EnumItem(RapidChange::NONE)
    };

    float RapidChange::calculate_tool_pos(uint8_t axis, uint8_t tool_num, float ref_value) {
        int multiplier = this->magazine_direction_ == POSITIVE ? 1 : -1;
        if (axis == this->orientation_) {
            return ref_value + (tool_num - 1) * this->pocket_offset_ * multiplier;
        } else {
            return ref_value;
        }
    }

    float RapidChange::get_manual_pos(uint8_t axis) {
        if (axis == X_AXIS) {
            return this->manual_x_pos_;
        } else {
            return this->manual_y_pos_;
        }
    }

    float RapidChange::get_touch_probe_pos(uint8_t axis) {
        if (axis == X_AXIS) {
            return this->touch_probe_x_pos_;
        } else {
            return this->touch_probe_y_pos_;
        }
    }

    float RapidChange::get_tool_pos(uint8_t axis, uint8_t tool_num) {
        if (!tool_has_pocket(tool_num)) {
            return get_manual_pos(axis);
        } else if (axis == X_AXIS) {        
            return calculate_tool_pos(axis, tool_num, this->pocket_one_x_pos_);
        } else {
            return calculate_tool_pos(axis, tool_num, this->pocket_one_y_pos_);
        }
    }

    bool RapidChange::tool_has_pocket(uint8_t tool_num) {
        return tool_num != 0 && tool_num <= this->pockets_;
    }

    // bool RapidChange::is_valid_configuration() {
    //     if (this->pocket_one_x_pos_ == NOT_ASSIGNED
    //         || this->pocket_one_y_pos_ == NOT_ASSIGNED
    //         || this-> manual_x_pos_ == NOT_ASSIGNED
    //         || this-> manual_y_pos_ == NOT_ASSIGNED) {
    //             this->validation_message_ = "XY Coordinates must be configured.";
    //             return false;
    //     }
    //     if (this->engage_z_ == NOT_ASSIGNED
    //         || this->back_off_engage_z_ == NOT_ASSIGNED
    //         || this->spindle_start_z_ == NOT_ASSIGNED
    //         || (this->tool_recognition_z_ == NOT_ASSIGNED && this->disable_tool_recognition_ == false)
    //         || this->safe_clearance_z_ == NOT_ASSIGNED) {
    //             this->validation_message_ = "Z Coordinates must be configured.";
    //             return false;
    //     }
    //     if (this->probe_ == TOUCH && 
    //         (this->touch_probe_x_pos_ == NOT_ASSIGNED 
    //         || this->touch_probe_y_pos_ == NOT_ASSIGNED
    //         || this->go_to_touch_probe_z_ == NOT_ASSIGNED
    //         || this->touch_probe_start_z_ == NOT_ASSIGNED
    //         || this->touch_tool_setter_z_ == NOT_ASSIGNED
    //         || this->touch_probe_max_distance_ == NOT_ASSIGNED)) {
    //             this->validation_message_ = "Touch Probe values must be configured if touch probe is enabled.";
    //             return false;
    //     }
    //     if (this->probe_ == INFRARED &&
    //         (this->infrared_probe_start_z_ == NOT_ASSIGNED
    //         || this->infrared_tool_setter_z_ == NOT_ASSIGNED)) {
    //             this->validation_message_ = "Infrared Probe values must be configured if infrared probe is enabled.";
    //             return false;
    //     }
    //     if (!(this->disable_tool_recognition_) && this->infrared_pin_.name() == "NO_PIN" && this->probe_ != INFRARED) {
    //         this->validation_message_ = "Infrared pin must be configured to enable tool recognition without infrared probing.";
    //         return false;
    //     }

    //     // configuration valid if we reached here
    //     return true;
    // }

    // const char* RapidChange::get_validation_message() {
    //     return this->validation_message_;
    // }

    void RapidChange::group(Configuration::HandlerBase& handler) {
        handler.item("collet", collet_, collets);
        handler.item("pockets", pockets_);
        handler.item("magazine_direction", magazine_direction_, directions);
        handler.item("z_direction", z_direction_, directions);
        handler.item("orientation", orientation_, orientations);
        handler.item("probe", probe_, probes);
        handler.item("disable_tool_recognition", disable_tool_recognition_);

        handler.item("pocket_one_x_pos", pocket_one_x_pos_);
        handler.item("pocket_one_y_pos", pocket_one_y_pos_);
        handler.item("manual_x_pos", manual_x_pos_);
        handler.item("manual_y_pos", manual_y_pos_);
        handler.item("touch_probe_x_pos", touch_probe_x_pos_);
        handler.item("touch_probe_y_pos", touch_probe_y_pos_);

        handler.item("engage_z", engage_z_);
        handler.item("back_off_engage_z", back_off_engage_z_);
        handler.item("spindle_start_z", spindle_start_z_);
        handler.item("tool_recognition_z", tool_recognition_z_);
        handler.item("go_to_touch_probe_z", go_to_touch_probe_z_);
        handler.item("touch_probe_start_z", touch_probe_start_z_);
        handler.item("infrared_probe_start_z", infrared_probe_start_z_);
        handler.item("infrared_tool_setter_z", infrared_tool_setter_z_);
        handler.item("touch_tool_setter_z", touch_tool_setter_z_);
        handler.item("safe_clearance_z", safe_clearance_z_); 
        handler.item("touch_probe_max_distance", touch_probe_max_distance_);     

        handler.item("engage_feedrate", engage_feedrate_);
        handler.item("infrared_probe_feedrate", infrared_probe_feedrate_);
        handler.item("touch_probe_feedrate", touch_probe_feedrate_);

        handler.item("spin_speed_engage_cw", spin_speed_engage_cw_);
        handler.item("spin_speed_engage_ccw", spin_speed_engage_ccw_);
        handler.item("spin_speed_infrared_probe", spin_speed_infrared_probe_);
        
        handler.item("dust_cover_pin", dust_cover_pin_);
        handler.item("infrared_pin", infrared_pin_);
        
    }

    void RapidChange::afterParse() {
        switch (this->collet_) {
            case ER11:
                this->pocket_offset_ = 38;
                break;
            case ER16:
                this->pocket_offset_ = 40;
                break;
            case ER20:
                this->pocket_offset_ = 45;
                break;
            default:
                break;
        }
    }
}


