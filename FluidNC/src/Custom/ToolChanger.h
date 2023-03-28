#pragma once

#include "Magazines.h"
#include "../Logging.h"
#include "../Configuration/Configurable.h"
#include <Preferences.h>

#define NOT_ASSIGNED 100000.0f

namespace RapidChange {
    class ToolChanger : public Configuration::Configurable {
        private:
            int currentTool_;
            Preferences preferences_;

        public:
            ToolChanger();

            float x_ref_pos_ = NOT_ASSIGNED;
            float yRefPos_ = NOT_ASSIGNED;
            float zRefPos_ = NOT_ASSIGNED;
            int direction_ = POSITIVE;
            int orientation_ = X_AXIS;
            int magazine_model_;
            RapidChange::Magazine magazine_;
            float engageZ_ = NOT_ASSIGNED;
            float backOffPickupZ_ = NOT_ASSIGNED;
            float safeClearanceZ_ = NOT_ASSIGNED;
            float spindleStartZ_ = NOT_ASSIGNED;
            float toolRecognitionZ_ = NOT_ASSIGNED;
            

            enum direction {
                NEGATIVE = -1,
                POSITIVE = 1,
            };

            enum orientation {
                X_AXIS = 0,
                Y_AXIS = 1
            };

            // enum ModelName {
            //     ER0011 = 1,
            //     ER0016,
            //     ER0020
            // };

            void performToolChange(int toolNum);

            void group(Configuration::HandlerBase& handler) override;
            void afterParse() override;
            ~ToolChanger();
    };
}




