#include "Magazines.h"
#include "ToolChanger.h"
#include "GCodeLine.h"
#include "../GCode.h"

namespace RapidChange {
    EnumItem directions[] = {
        { ToolChanger::POSITIVE, "POSITIVE" },
        { ToolChanger::NEGATIVE, "NEGATIVE" },
        EnumItem(ToolChanger::POSITIVE)
    };

    EnumItem orientations[] = {
        { ToolChanger::X_AXIS, "X_AXIS" },
        { ToolChanger::Y_AXIS, "Y_AXIS" },
        EnumItem(ToolChanger::X_AXIS)
    };

    EnumItem model_names[] = {
        { ER0011, "ER0011" },
        { ER0016, "ER0016" },
        { ER0020, "ER0020" },
        EnumItem(ER0011)
    };

    ToolChanger::ToolChanger() {
        
        preferences_.begin("RapidChange", false);
        if (!preferences_.isKey("currentTool")) {
            preferences_.putUInt("currentTool", 0);
        }
        currentTool_ = preferences_.getUInt("currentTool");

    }

    void ToolChanger::performToolChange(int selectedTool) {
        if (x_ref_pos_ == NOT_ASSIGNED) {
            log_error("xRefPos must be set.");
        }
        log_info(x_ref_pos_);
        log_info(currentTool_);
    }

    void ToolChanger::group(Configuration::HandlerBase& handler) {
        handler.item("x_mpos", x_ref_pos_);
        handler.item("y_mpos", yRefPos_);
        handler.item("z_mpos", zRefPos_);
        handler.item("direction", direction_, directions);
        handler.item("orientation", orientation_, orientations);
        handler.item("model", magazine_model_, model_names);
    }

    void ToolChanger::afterParse() {

    }

    ToolChanger::~ToolChanger() {
        preferences_.end();
    }
}


