#include "../Logging.h"
#include "../Configuration/Configurable.h"
#include <Preferences.h>

class RapidChange : public Configuration::Configurable {
    private:
        int _currentTool;
        Preferences _preferences;

    public:
        RapidChange();

        float x_mpos_;
        float y_mpos_;
        float z_mpos_;
        int direction_ = POSITIVE;
        int orientation_ = X_AXIS;
        int model_;
        float _engage_z;
        float _back_off_pickup_z;
        float _spindle_start_z;
        

        enum direction {
            NEGATIVE = -1,
            POSITIVE = 1,
        };

        enum orientation {
            X_AXIS = 0,
            Y_AXIS = 1
        };

        enum model {
            ER0011 = 1,
            ER0016,
            ER0020
        };

        void performToolChange(int toolNum);

        void group(Configuration::HandlerBase& handler) override;
        ~RapidChange();
};
