#pragma once

#include <map>

namespace RapidChange {
    enum MagazineModel {
        ER0011 = 1,
        ER0016,
        ER0020
    };

    struct Magazine {
        MagazineModel model;
        float tool_offset;
        int max_tools;    
    };

    Magazine er0011 {ER0011, 50.0f, 6};

    std::map<MagazineModel, Magazine> magazines_map;
    
    
}


