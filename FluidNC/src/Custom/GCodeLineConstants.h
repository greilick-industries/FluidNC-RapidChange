#pragma once

#include <string>

using namespace std;

namespace RapidChange {
    enum class WordGroup : uint8_t {
        kDistance,
        kUnits,
        kNonModal,
        kLWord,
        kPWord,
        kMotion,
        kSpindleControl,
        kSpindleSpeed,
        kXAxis,
        kYAxis,
        kZAxis,
        kFeedRate,
    };

namespace Letters {
    static const char kF = 'F';
    static const char kG = 'G';
    static const char kL = 'L';
    static const char kM = 'M';
    static const char kP = 'P';
    static const char kS = 'S';
    static const char kX = 'X';
    static const char kY = 'Y';
    static const char kZ = 'Z';
}

namespace Numbers {
    static const uint8_t kZero = 0;
    static const uint8_t kOne = 1;
    static const uint8_t kThree = 3;
    static const uint8_t kFour = 4;
    static const uint8_t kFive = 5;
    static const uint8_t kTen = 10;
    static const uint8_t kTwenty = 20;
    static const uint8_t kTwentyOne = 21;
    static const uint8_t kFiftyThree = 53;
    static const uint8_t kNinety = 90;
    static const uint8_t kNinetyOne = 91;
    static const float kThirtyEightPointTwo = 38.2;
}
}
