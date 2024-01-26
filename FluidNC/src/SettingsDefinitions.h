#pragma once

#include "Settings.h"

extern StringSetting* config_filename;

extern StringSetting* build_info;

extern IntSetting* status_mask;

extern EnumSetting* message_level;

// Current tool persistance modification
extern IntSetting* current_tool;

// Current tool length offset persistance modification
extern StringSetting* current_tlo;
