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

            // magazine settings
            int collet_ = ER11;
            int magazine_direction_ = POSITIVE;
            int z_direction_ = POSITIVE;
            int orientation_ = X_AXIS;
            bool disable_tool_recognition_ = true;
            int probe_ = NONE;
            int pockets_ = 5;

            // location and feedrate
            float pocket_one_x_pos_ = NOT_ASSIGNED;
            float pocket_one_y_pos_ = NOT_ASSIGNED;
            float manual_x_pos_ = NOT_ASSIGNED;
            float manual_y_pos_ = NOT_ASSIGNED;
            int engage_feedrate_ = 300;
            int spin_speed_engage_cw_ = 1000;
            int spin_speed_engage_ccw_ = 1000;

            // tool change z heights
            float engage_z_ = NOT_ASSIGNED;
            float back_off_engage_z_ = NOT_ASSIGNED;
            float spindle_start_z_ = NOT_ASSIGNED;
            float tool_recognition_z_ = NOT_ASSIGNED;
            float safe_clearance_z_ = NOT_ASSIGNED;
            

            // touch probe
            float touch_probe_x_pos_ = NOT_ASSIGNED;
            float touch_probe_y_pos_ = NOT_ASSIGNED;
            float go_to_touch_probe_z_ = NOT_ASSIGNED;
            float touch_probe_start_z_ = NOT_ASSIGNED;
            float touch_tool_setter_z_ = NOT_ASSIGNED;
            float touch_probe_max_distance_ = NOT_ASSIGNED;
            int touch_probe_feedrate_initial_ = 300;
            int touch_probe_feedrate_ = 100;
            
            // infrared probe
            float infrared_probe_start_z_ = NOT_ASSIGNED;
            float infrared_tool_setter_z_ = NOT_ASSIGNED;
            int infrared_probe_feedrate_ = 300;
            int spin_speed_infrared_probe_ = 3000;
            
            //Pins
            Pin dust_cover_pin_;
            Pin infrared_pin_;

            // Dust cover control using open axis
            bool dust_cover_use_axis_ = false;
            int dust_cover_axis_ = 3;
            float dust_cover_pos_open_ = NOT_ASSIGNED;
            float dust_cover_pos_closed_ = NOT_ASSIGNED;
            int dust_cover_feedrate = 300;

            enum dustCoverAxis {
                A_AXIS = 3,
                B_AXIS = 4,
                C_AXIS = 5
            };

            enum direction {
                NEGATIVE = 0,
                POSITIVE
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
            
            float get_touch_probe_pos(uint8_t axis);
            float get_tool_pos(uint8_t axis, uint8_t tool_num);
            void operate_dust_cover_pin(bool opening);
            bool tool_has_pocket(uint8_t tool_num);
            const char* get_validation_message();
            

            void group(Configuration::HandlerBase& handler) override;
            void afterParse() override;
            ~RapidChange() = default;
    };
}




