#ifndef CONSOLE_H
#define CONSOLE_H

#include <ncurses.h>

#define MAX_LOGS 100
#define MAX_LOG 50
#define LOG_ROW_OFFSET 2
#define INPUT_ROW_OFFSET 1
#define INPUT_COL_OFFSET 3
#define CURSOR_HOME_ROW 1
#define CURSOR_HOME_COL 3
#define CONSOLE_SYMBOL_OFFSET 3


typedef enum LogSourceType {
    LOG_ENGINE,
    LOG_APP,
    LOG_ERROR
} LogSourceType;

typedef enum Key {
    KEY_NONE      = ERR,
    KEY_ESC       = 27,
    KEY_ENTER_1   = '\n',
    KEY_ENTER_2   = '\r',
    KEY_BACKSPACE_1    = KEY_BACKSPACE,
    KEY_BACKSPACE_2    = 127,
    KEY_BACKSPACE_3    = 8,
} Key;

typedef enum ConsoleCmd {
    CONSOLE_NONE,
    CONSOLE_ADD,
    CONSOLE_DELETE,
    CONSOLE_SHOW,
    CONSOLE_HELP,
} ConsoleCmd;

typedef struct TermWindow {
    int cols;
    int rows;
} TermWindow;

typedef char ConsoleLog[MAX_LOGS][MAX_LOG];
typedef char ConsoleInput[MAX_LOG];
extern ConsoleLog console_log;

TermWindow capture_window();
int count_logs(ConsoleLog logs);
void clear_row(int row);
void clear_logs(ConsoleLog logs);
void print_logs(ConsoleLog logs);
void print_input(ConsoleInput input);
void log_input(ConsoleInput input);
void clear_input(ConsoleInput input);
void console_init();
void console_backspace();
void console_shutdown();
bool read_console(ConsoleInput console_str);
void console_write(LogSourceType source, const char *fmt, ...);


#endif

