#include "../Logging.h"
#include "../Configuration/Configurable.h"

class RapidChangeConfig : public Configuration::Configurable {
    public:
        void execute() {
            log_info("Rapid Change executed.");
            log_info(x_mpos_);
            log_info(y_mpos_);
            log_info(z_mpos_);
        }
    
        float x_mpos_;
        float y_mpos_;
        float z_mpos_;

        void group(Configuration::HandlerBase& handler) {
            handler.item("x_mpos", x_mpos_);
            handler.item("y_mpos", y_mpos_);
            handler.item("z_mpos", z_mpos_);
        }
};
