#include "RapidChange.h"
#include "GCodeLine.h"
#include "../GCode.h"


EnumItem directions[] = {
    { RapidChange::POSITIVE, "POSITIVE" },
    { RapidChange:: NEGATIVE, "NEGATIVE" },
    EnumItem(RapidChange::POSITIVE)
};

EnumItem orientations[] = {
    { RapidChange::X_AXIS, "X_AXIS" },
    { RapidChange:: Y_AXIS, "Y_AXIS" },
    EnumItem(RapidChange::X_AXIS)
};

EnumItem models[] = {
    { RapidChange::ER0011, "ER0011" },
    { RapidChange:: ER0016, "ER0016" },
    { RapidChange:: ER0020, "ER0020" },
    EnumItem(RapidChange::ER0011)
};

RapidChange::RapidChange() {
    
    _preferences.begin("RapidChange", false);
    if (!_preferences.isKey("currentTool")) {
        _preferences.putUInt("currentTool", 0);
    }
    _currentTool = _preferences.getUInt("currentTool");
}

void RapidChange::performToolChange(int selectedTool) {
    // log_info("X ref pos");
    // log_info(this->x_mpos_);
    // log_info("Y ref pos");
    // log_info(this->y_mpos_);
    // log_info("Z ref pos");
    // log_info(this->z_mpos_);
    // log_info("Direction");
    // log_info(this->direction_);
    // log_info("Orientation");
    // log_info(this->orientation_);
    // log_info("Model");
    // log_info(this->model_);
    
    
    log_info(_currentTool);
}

void RapidChange::group(Configuration::HandlerBase& handler) {
    handler.item("x_mpos", x_mpos_);
    handler.item("y_mpos", y_mpos_);
    handler.item("z_mpos", z_mpos_);
    handler.item("direction", direction_, directions);
    handler.item("orientation", orientation_, orientations);
    handler.item("model", model_, models);
}

RapidChange::~RapidChange() {}
