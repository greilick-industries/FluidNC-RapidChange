#include "Magazine.h"
#include "../GCode.h"
#include "../Protocol.h"

namespace RapidChange {
    EnumItem directions[] = {
        { Magazine::POSITIVE, "POSITIVE" },
        { Magazine::NEGATIVE, "NEGATIVE" },
        EnumItem(Magazine::POSITIVE)
    };

    EnumItem orientations[] = {
        { Magazine::X_AXIS, "X_AXIS" },
        { Magazine::Y_AXIS, "Y_AXIS" },
        EnumItem(Magazine::X_AXIS)
    };

    EnumItem models[] = {
        { Magazine::ER0011, "ER0011" },
        { Magazine::ER0016, "ER0016" },
        { Magazine::ER0020, "ER0020" },
        EnumItem(Magazine::ER0011)
    };

    void Magazine::group(Configuration::HandlerBase& handler) {
        handler.item("x_ref_pos", x_ref_pos_);
        handler.item("y_ref_pos_", y_ref_pos_);
        handler.item("z_ref_pos", z_ref_pos_);
        handler.item("engage_z", engage_z_);
        
        handler.item("direction", direction_, directions);
        handler.item("orientation", orientation_, orientations);
        handler.item("model", model_, models);
    }

    void Magazine::afterParse() {

    }
}


