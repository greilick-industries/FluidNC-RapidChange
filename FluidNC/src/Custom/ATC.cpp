#include "../GCode.h"
#include "../Serial.h"
#include "../Settings.h"
#include "../Uart.h"
#include "../WebUI/InputBuffer.h"
#include <stdarg.h>

void gc_exec_linef(const char* format, ...);

void user_tool_change(uint8_t new_tool) {
    if (new_tool == 1) {
        gc_exec_linef("G20G90");
        gc_exec_linef("G58");
        gc_exec_linef("g1z4f100");
        gc_exec_linef("x0y0");
        gc_exec_linef("Z.1");
        gc_exec_linef("S400M4");
        gc_exec_linef("M5");
        gc_exec_linef("g4p1");
        gc_exec_linef("s3000m3");
        gc_exec_linef("g4p.2");
        gc_exec_linef("z.25f20");
        gc_exec_linef("G0Z3.5");
        gc_exec_linef("G54");
        gc_exec_linef("s8000m3");
        gc_exec_linef("g38.2f15z-1.5");
        gc_exec_linef("g0g91z.15");
        gc_exec_linef("g38.2f1z-.2");
        gc_exec_linef("g10l20p0z2.492");
        gc_exec_linef("M5");
        gc_exec_linef("g0g90z3");
    
        log_info("Tool change completed");
    }
}

// This comment is even newer
void gc_exec_linef(const char* format, ...) {
    Channel& r_channel = WebUI::inputBuffer;
    va_list args;
    char gc_line[20];
    gc_line[strlen(format)] = '\r';
    sprintf(gc_line, format, args);
    Error line_executed = execute_line(gc_line, r_channel, WebUI::AuthenticationLevel::LEVEL_GUEST);
    r_channel << ">" << gc_line << ":";
    report_status_message(line_executed, r_channel);
}
