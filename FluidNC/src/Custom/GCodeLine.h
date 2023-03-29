#pragma once

#include "GCodeLineConstants.h"
#include <iomanip> 
#include <map>
#include <sstream>

using namespace std;

namespace RapidChange {
    typedef pair<WordGroup, string> WordMapPair;
    typedef std::map<WordGroup, string>::iterator WordMapIterator;
    typedef pair<WordMapIterator, bool> WordMapInsertResult;

    class GCodeLineBuilder;

    class GCodeLine {
        public:
            friend class GCodeLineBuilder;
            static GCodeLineBuilder Write();
            string to_string() const {
                stringstream stream;
                for (auto pair : word_map_) {
                    stream << pair.second;
                }
                stream << "\n";
                return stream.str();
            }

        private:
            GCodeLine() = default;
            std::map<WordGroup, string> word_map_;
    };

    class GCodeLineBuilder {
        public:
            GCodeLineBuilder& Absolute();
            GCodeLineBuilder& Incremental();
            GCodeLineBuilder& Dwell(float milliseconds, uint8_t precision = Numbers::kOne);
            GCodeLineBuilder& Inches();
            GCodeLineBuilder& Millimeters();
            GCodeLineBuilder& MachineCoordinates();
            GCodeLineBuilder& SetCoordinateSystem(uint8_t coordinate_system);
            GCodeLineBuilder& MoveRapid();
            GCodeLineBuilder& MoveLinear();
            GCodeLineBuilder& PauseExecution();
            GCodeLineBuilder& ProbeToward();
            GCodeLineBuilder& SpindleCW();
            GCodeLineBuilder& SpindleCCW();
            GCodeLineBuilder& SpindleStop();
            GCodeLineBuilder& SpindleSpeed(uint16_t value);
            GCodeLineBuilder& X(float value, uint8_t precision = Numbers::kThree);
            GCodeLineBuilder& Y(float value, uint8_t precision = Numbers::kThree);
            GCodeLineBuilder& Z(float value, uint8_t precision = Numbers::kThree);
            GCodeLineBuilder& FeedRate(float value, uint8_t precision = Numbers::kZero);

            operator GCodeLine&&() {
                return move(line_);
            }

        private:
            GCodeLine line_;
            
            void AddWord(WordGroup group, string word);
            string Join(char letter, uint16_t number);
            string Join(char letter, float number, uint8_t precision);

    };
}

