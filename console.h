#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdbool.h>

//Parse commands with JSON
//https://github.com/ibireme/yyjson
//
#define MAX_LOG_STR 100
typedef struct ConsoleLogString {
    char string[MAX_LOG_STR];
} ConsoleLogString;

extern bool console_active;

#define CONSOLE_DEBUG_ON true
#define CONSOLE_DEBUG_OFF true
extern bool console_debug;


typedef enum LogSourceType {
    LOG_ENGINE,
    LOG_APP,
    LOG_ERROR,
    LOG_CONSOLE,
} LogSourceType;



void print_logs();
void console_init();
void console_shutdown();
bool read_console(ConsoleLogString *input);
void console_write(LogSourceType source, const char *fmt, ...);
bool console_is_active();
void console_debug_write(LogSourceType source, const char *fmt, ...);
void console_set_debug(bool state);
#endif
