#include "../GCode.h"
#include "../Machine/MachineConfig.h"
#include "../Protocol.h"
#include "../Serial.h"
#include "../Settings.h"
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

enum SpinState {
    CW = 3,
    CCW = 4,
    STOP = 5
};

uint8_t current_tool = 0;
StoredState stored_state;
RapidChange::RapidChange* rapid_change;

void spin_cw(int speed);
void spin_ccw(int speed);
void spin_stop();
void execute_linef(bool sync_after, const char* format, ...);
void go_to_touch_probe_xy();
void go_to_tool_xy(uint8_t tool_num);
void go_to_z(float position);
void go_to_z(float position, int feedrate);
void message_start();
void operate_dust_cover(bool open);
void operate_dust_cover_axis(bool open);
void get_tool(uint8_t tool_num);
void set_tool(uint8_t tool_num);
void set_tool_infrared();
void set_tool_touch();
void record_state();
void restore_state();
void drop_tool();
void set_rapid_change_state();
bool spindle_has_tool();
