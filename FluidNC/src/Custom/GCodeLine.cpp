#include "GCodeLine.h"

namespace RapidChange {
    GCodeLineBuilder GCodeLine::Write() {
        return GCodeLineBuilder();
    }

    GCodeLineBuilder& GCodeLineBuilder::Absolute() {
        AddWord(WordGroup::kDistance, Join(Letters::kG, Numbers::kNinety));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::Incremental() {
        AddWord(WordGroup::kDistance, Join(Letters::kG, Numbers::kNinetyOne));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::Dwell(float milliseconds, uint8_t precision) {
        AddWord(WordGroup::kNonModal, Join(Letters::kG, Numbers::kFour));
        AddWord(WordGroup::kPWord, Join(Letters::kP, milliseconds, precision));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::Inches() {
        AddWord(WordGroup::kUnits, Join(Letters::kG, Numbers::kTwenty));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::Millimeters() {
        AddWord(WordGroup::kUnits, Join(Letters::kG, Numbers::kTwentyOne));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::MachineCoordinates() {
        AddWord(WordGroup::kNonModal, Join(Letters::kG, Numbers::kFiftyThree));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::SetCoordinateSystem(uint8_t coordinate_system) {
        AddWord(WordGroup::kNonModal, Join(Letters::kG, Numbers::kTen));
        AddWord(WordGroup::kLWord, Join(Letters::kL, Numbers::kTwenty));
        AddWord(WordGroup::kPWord, Join(Letters::kP, coordinate_system));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::MoveRapid() {
        AddWord(WordGroup::kMotion, Join(Letters::kG, Numbers::kZero));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::MoveLinear() {
        AddWord(WordGroup::kMotion, Join(Letters::kG, Numbers::kOne));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::ProbeToward() {
        AddWord(WordGroup::kMotion, Join(Letters::kG, Numbers::kThirtyEightPointTwo, Numbers::kOne));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::SpindleCCW() {
        AddWord(WordGroup::kSpindleControl, Join(Letters::kM, Numbers::kFour));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::SpindleCW() {
        AddWord(WordGroup::kSpindleControl, Join(Letters::kM, Numbers::kThree));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::SpindleStop() {
        AddWord(WordGroup::kSpindleControl, Join(Letters::kM, Numbers::kFive));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::SpindleSpeed(uint16_t value) {
        AddWord(WordGroup::kSpindleSpeed, Join(Letters::kS, value));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::X(float number, uint8_t precision) {
        AddWord(WordGroup::kXAxis, Join(Letters::kX, number, precision));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::Y(float number, uint8_t precision) {
        AddWord(WordGroup::kYAxis, Join(Letters::kY, number, precision));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::Z(float number, uint8_t precision) {
        AddWord(WordGroup::kZAxis, Join(Letters::kZ, number, precision));
        return *this;
    }

    GCodeLineBuilder& GCodeLineBuilder::FeedRate(float value, uint8_t precision) {
        AddWord(WordGroup::kFeedRate, Join(Letters::kF, value));
        return *this;
    }

    void GCodeLineBuilder::AddWord(WordGroup group, string word) {
        WordMapInsertResult result = line_.word_map_.insert(WordMapPair(group, word));
        if (result.second == false) {
            // TODO - Better error handling
            throw "Requested Modal Group already assigned.";
        }
    }

    string GCodeLineBuilder::Join(char letter, uint16_t number) {
        stringstream stream;
        stream << letter << (int)number;
        return stream.str();
    }

    string GCodeLineBuilder::Join(char letter, float number, uint8_t precision) {
        stringstream stream;
        stream << letter << fixed << setprecision(precision) << number;
        return stream.str();
    }
}

