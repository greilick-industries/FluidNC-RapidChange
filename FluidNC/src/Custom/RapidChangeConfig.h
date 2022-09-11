#include "../Logging.h"
#include "../Configuration/Configurable.h"

class RapidChangeConfig : public Configuration::Configurable {
    public:
        RapidChangeConfig();
    
        float x_mpos_;
        float y_mpos_;
        float z_mpos_;

        void group(Configuration::HandlerBase& handler) override;
        ~RapidChangeConfig();
};
