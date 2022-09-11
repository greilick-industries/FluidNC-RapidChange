#include "RapidChangeConfig.h"

void RapidChangeConfig::group(Configuration::HandlerBase& handler) {
    handler.item("x_mpos", x_mpos_);
    handler.item("y_mpos", y_mpos_);
    handler.item("z_mpos", z_mpos_);
}
