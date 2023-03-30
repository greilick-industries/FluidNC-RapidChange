#include "../GCode.h"
#include "../Machine/MachineConfig.h"
#include "../Protocol.h"
#include "../Serial.h"
#include "../Settings.h"
#include <Preferences.h>
#include <stdarg.h>

struct StoredState {
    CoolantState coolant;
    Distance distance;
    float feed_rate;
    FeedRate feed_rate_mode;
    Motion motion;
    Units units;
};

Preferences preferences;
StoredState stored_state;
CoolantState stored_coolant_state;
bool was_inches;
bool was_incremental; 

void execute_linef(bool sync_after, const char* format, ...);
void record_state();
void restore_state();
void return_tool(uint8_t tool_num);
void set_tool_change_state();
void pickup_tool(uint8_t tool_num);
bool drop_tool(uint8_t tool_num);
