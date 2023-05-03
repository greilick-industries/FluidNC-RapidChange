#pragma once

#include "../Configuration/Configurable.h"

#define NOT_ASSIGNED 100000.0f

namespace RapidChange {
    class RapidChange : public Configuration::Configurable {
        private:           
            float pocket_offset_;

            float calculate_tool_pos(uint8_t axis, uint8_t tool_num, float ref_value);
            float get_manual_pos(uint8_t axis);
            
        public:
            RapidChange() = default;

            bool disable_tool_recognition_ = false;

            float pocket_one_x_pos_ = NOT_ASSIGNED;
            float pocket_one_y_pos_ = NOT_ASSIGNED;
            float manual_x_pos_ = NOT_ASSIGNED;
            float manual_y_pos_ = NOT_ASSIGNED;

            float engage_z_ = NOT_ASSIGNED;
            float back_off_engage_z_ = NOT_ASSIGNED;
            float spindle_start_z_ = NOT_ASSIGNED;
            float tool_recognition_z_ = NOT_ASSIGNED;
            float go_to_touch_probe_z_ = NOT_ASSIGNED;
            float infrared_probe_start_z_ = NOT_ASSIGNED;
            float touch_probe_start_z_ = NOT_ASSIGNED;
            float infrared_tool_setter_z_ = NOT_ASSIGNED;
            float touch_tool_setter_z_ = NOT_ASSIGNED;
            float safe_clearance_z_ = NOT_ASSIGNED;

            int collet_ = ER11;
            int direction_ = POSITIVE;
            int orientation_ = X_AXIS;
            int probe_ = 0;
            int pockets_ = 5;

            int engage_feedrate_ = 300;
            int infrared_probe_feedrate_ = 300;
            int touch_probe_feedrate_ = 100;
            int spin_speed_engage_cw_ = 1000;
            int spin_speed_engage_ccw_ = 1000;
            int spin_speed_infrared_probe_ = 3000;

            Pin dust_cover_pin_;
            Pin infrared_pin_;      

            enum direction {
                NEGATIVE = -1,
                POSITIVE = 1,
            };
            enum orientation {
                X_AXIS = 0,
                Y_AXIS = 1
            };
            enum collet {
                ER11 = 1,
                ER16,
                ER20
            };
            enum probe {
                NONE = 0,
                INFRARED,
                TOUCH
            };            
            
            float get_tool_pos(uint8_t axis, uint8_t tool_num);
            bool tool_has_pocket(uint8_t tool_num);

            void group(Configuration::HandlerBase& handler) override;
            void afterParse() override;
            ~RapidChange() = default;
    };
}




