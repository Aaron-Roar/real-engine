#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdbool.h>

//Parse commands with JSON
//https://github.com/ibireme/yyjson
//
#define MAX_LOGS 100
#define MAX_LOG_STR 50
#define CONSOLE_TOKEN_LENGTH 3
#define CONSOLE_TIME_LENGTH 10
#define CONSOLE_NULL_CHAR_SIZE 1
#define MAX_LOG (MAX_LOG_STR + CONSOLE_TOKEN_LENGTH + CONSOLE_TIME_LENGTH + CONSOLE_NULL_CHAR_SIZE)
//Log: |ConsoleToken|ConsoleStr|ConsoleTime|ConsoleNull|


#define LOG_ROW_OFFSET 2
#define INPUT_ROW_OFFSET 1
#define INPUT_COL_OFFSET 3
#define CURSOR_HOME_ROW 1
#define CURSOR_HOME_COL 3
#define CONSOLE_SYMBOL_OFFSET 3

extern bool console_active;


typedef enum LogSourceType {
    LOG_ENGINE,
    LOG_APP,
    LOG_ERROR,
    LOG_CONSOLE,
} LogSourceType;


typedef enum ConsoleCmd {
    CONSOLE_NONE,
    CONSOLE_ADD,
    CONSOLE_DELETE,
    CONSOLE_SHOW,
    CONSOLE_HELP,
} ConsoleCmd;

typedef enum ConsoleAdd {
    CONSOLE_ADD_ENTITY,
    CONSOLE_ADD_COMPONENTS,
    CONSOLE_ADD_HITBOX,
    CONSOLE_ADD_ASSET,
    CONSOLE_ADD_FORCE,
} ConsoleAdd;

typedef struct TermWindow {
    int cols;
    int rows;
} TermWindow;

typedef struct ConsoleLogString {
    char string[MAX_LOG_STR];
} ConsoleLogString;

typedef struct ConsoleLog {
    double time;
    LogSourceType source;
    ConsoleLogString log;
} ConsoleLog;

extern ConsoleLog logs[MAX_LOGS];

TermWindow capture_window();
int count_logs();
void print_logs();
void print_input();
void console_init();
void console_backspace();
void console_shutdown();
void clear_input();
void clear_logs();
void clear_row(int row);
void log_input(ConsoleLogString input);
bool read_console(ConsoleLogString *input);
void console_write(LogSourceType source, const char *fmt, ...);
#endif
