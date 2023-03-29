#pragma once

#include "../Configuration/Configurable.h"

#define NOT_ASSIGNED 100000.0f

namespace RapidChange {
    class Magazine : public Configuration::Configurable {
        private:           

        public:
            Magazine();

            float x_ref_pos_ = NOT_ASSIGNED;
            float y_ref_pos_ = NOT_ASSIGNED;
            float z_ref_pos_ = NOT_ASSIGNED;
            int direction_ = POSITIVE;
            int orientation_ = X_AXIS;
            int model_ = ER0011;            
            float engage_z_ = NOT_ASSIGNED;
            float back_off_pickup_z_ = NOT_ASSIGNED;
            float safe_clearance_z_ = NOT_ASSIGNED;
            float spindle_start_z_ = NOT_ASSIGNED;
            float tool_recognition_z_ = NOT_ASSIGNED;
            

            enum direction {
                NEGATIVE = -1,
                POSITIVE = 1,
            };

            enum orientation {
                X_AXIS = 0,
                Y_AXIS = 1
            };

            enum Model {
                ER0011 = 1,
                ER0016,
                ER0020
            };            

            void group(Configuration::HandlerBase& handler) override;
            void afterParse() override;
            ~Magazine();
    };
}




