#include "../GCode.h"
#include "../Machine/MachineConfig.h"
#include "../Protocol.h"
#include "../Serial.h"
#include "../Settings.h"
#include <Preferences.h>
#include <stdarg.h>

#define RC_PREF_NAMESPACE "rapid_change"
#define CURRENT_TOOL_KEY "c_tool"
#define OPEN_DUST_COVER true
#define CLOSE_DUST_COVER false

struct StoredState {
    CoolantState coolant;
    Distance distance;
    FeedRate feed_rate_mode;
    Units units;
};

Preferences preferences;
StoredState stored_state;
uint8_t current_tool = 0;
bool current_tool_fetched = false;

void fetch_current_tool();
void execute_linef(bool sync_after, const char* format, ...);
void go_above_tool(uint8_t tool_num);
void go_to_safe_clearance();
void message_start();
void operate_dust_cover(bool open);
void record_state();
void restore_state();
void return_tool();
void set_tool_change_state();
void pickup_tool(uint8_t tool_num);
bool drop_tool(uint8_t tool_num);
