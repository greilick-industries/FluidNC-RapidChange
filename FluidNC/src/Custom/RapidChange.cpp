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
        if (axis == this->orientation_) {
            return ref_value;
        } else {
            return ref_value + (tool_num - 1) * this->pocket_offset_ * this->direction_;
        }
    }

    float RapidChange::get_manual_pos(uint8_t axis) {
        if (axis == X_AXIS) {
            return this->manual_x_pos_;
        } else {
            return this->manual_y_pos_;
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

    void RapidChange::group(Configuration::HandlerBase& handler) {
        handler.item("pocket_one_x_pos", pocket_one_x_pos_);
        handler.item("pocket_one_y_pos", pocket_one_y_pos_);
        handler.item("manual_x_pos", manual_x_pos_);
        handler.item("manual_y_pos", manual_y_pos_);

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

        handler.item("engage_feedrate", engage_feedrate_);
        handler.item("infrared_probe_feedrate", infrared_probe_feedrate_);
        handler.item("touch_probe_feedrate", touch_probe_feedrate_);
        handler.item("dust_cover_pin", dust_cover_pin_);
        handler.item("infrared_pin", infrared_pin_);

        handler.item("spin_speed_engage_cw", spin_speed_engage_cw_);
        handler.item("spin_speed_engage_ccw", spin_speed_engage_ccw_);
        handler.item("spin_speed_infrared_probe", spin_speed_infrared_probe_);

        handler.item("collet", collet_, collets);
        handler.item("direction", direction_, directions);
        handler.item("orientation", orientation_, orientations);
        handler.item("probe", probe_, probes);
        handler.item("pockets", pockets_);
        handler.item("disable_tool_recognition", disable_tool_recognition_);
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


