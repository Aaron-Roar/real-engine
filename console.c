#include "console.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ncurses.h>

#define MAX_LOGS 100
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

typedef struct TermWindow {
    int cols;
    int rows;
} TermWindow;
TermWindow capture_window();
typedef enum ConsoleKey {
    Console_KEY_NONE      = ERR,
    Console_KEY_ESC       = 27,
    Console_KEY_ENTER_1   = '\n',
    Console_KEY_ENTER_2   = '\r',
    Console_KEY_BACKSPACE_1    = KEY_BACKSPACE,
    Console_KEY_BACKSPACE_2    = 127,
    Console_KEY_BACKSPACE_3    = 8,
} ConsoleKey;


typedef struct ConsoleLog {
    double time;
    LogSourceType source;
    ConsoleLogString log;
} ConsoleLog;

ConsoleLog logs[MAX_LOGS] = {0};
int log_index = 0;

ConsoleLogString cmd_line_input = {0};
int cmd_line_input_index = 0;

bool console_active = false;
int console_scroll_offset = 0;

bool console_is_active() {
    return console_active;
}
TermWindow capture_window() {
    TermWindow window = {0};
    getmaxyx(stdscr, window.rows, window.cols);
    return window;
}

int count_logs() {
    int log_count = 0;
    for(int i = 0; i < MAX_LOGS; i++) {
        if(logs[i].log.string[0] != 0)
            log_count += 1;
    }
    return log_count;
}

void clear_row(int row) {
    TermWindow window = {0};
    getmaxyx(stdscr, window.rows, window.cols);
    if((row >= window.rows) || (row < 0) ) { //Rows accessing out of range
        //Error
    }
    else {
        move(row, 0);
        clrtoeol();
    }
}

void clear_logs() {
    TermWindow window = capture_window();

    int log_size = count_logs();
    int log_overflow = (log_size > (window.rows - LOG_ROW_OFFSET)) ? log_size - (window.rows - LOG_ROW_OFFSET) : 0; //If log_size > window.rows ? true : false;
    for(int i = 0; i < log_size - log_overflow; i++) {
        move(i, 0);
        clrtoeol();
    }
}

void print_logs() {
    if(console_active) {
        TermWindow window = capture_window();

        clear_logs();
        int log_size = count_logs();
        int log_overflow = (log_size > (window.rows - LOG_ROW_OFFSET)) ? log_size - (window.rows - LOG_ROW_OFFSET) : 0; //If log_size > window.rows

        for(int i = 0; i < log_size - log_overflow; i++) {
            mvprintw(i, 0, "%s", logs[(i + log_overflow) - console_scroll_offset].log.string);
        }
    }
}


void print_input() {
    if(console_active) {
        TermWindow w = capture_window();
        char input_line[w.cols - 1];
        memset(input_line, '-', sizeof(input_line));
        input_line[w.cols - 2] = '\0';

        mvprintw(w.rows - LOG_ROW_OFFSET, 0, "%s", input_line);
        clear_row(w.rows - INPUT_ROW_OFFSET);
        mvprintw(w.rows - INPUT_ROW_OFFSET, 0, ">>%s", cmd_line_input.string);
    }
}

void log_input(ConsoleLogString input) {
    input.string[MAX_LOG_STR - 1] = '\0';

    memcpy(
        logs[log_index].log.string,
        input.string,
        sizeof(logs[log_index].log.string)
    );

    log_index = (log_index + 1) % MAX_LOGS;

    print_logs();
}

void clear_input() {
    TermWindow w = capture_window();

    memset(cmd_line_input.string, 0, sizeof(ConsoleLogString));
    clear_row(w.rows - INPUT_ROW_OFFSET);
}

void console_init() {
    initscr(); //Hands control of the terminal to <ncurses.h>
    cbreak(); //Makes input available imediatley instead of waiting for enter press
    noecho(); //Stops the terminal from auto showing typed characters. We want to do this our own way instead.
    keypad(stdscr, TRUE); //Allows ncurses to detect special keypresses
    nodelay(stdscr, TRUE); //Makes getch return immediatley and not hang. it returns ERR if nothing input
    console_active = true;
}

void console_backspace() {
    if(cmd_line_input_index != 0) {
        cmd_line_input.string[cmd_line_input_index] = 0;
        cmd_line_input.string[cmd_line_input_index - 1] = 0;
        cmd_line_input_index -= 1;
    }
}

void console_shutdown() {
    //Goodbye MSG
    console_write(LOG_CONSOLE, "<ESC-Key> Recived.\nConsole Shutting...\n");
    clear_logs();
    print_logs();

    console_active = false;
    clear_logs();
    clear_input();
    endwin(); //Kills ncurses now terminal is back to normal
}

void console_scroll_logs_up() {

}

void console_scroll_logs_down() {

}

ConsoleLogString input_log_buffer = {0};
int input_log_buffer_index = 0;
//ConsoleStr will be all JSON

bool read_console(ConsoleLogString *console_str) {
    if(console_active) {
        TermWindow w = capture_window();
        move(w.rows - INPUT_ROW_OFFSET, cmd_line_input_index + 2);

            int ch = getch();
            switch(ch) {
                case Console_KEY_NONE:
                    break;
                case Console_KEY_ESC:
                    //Exit ??
                    console_shutdown();
                    break;
                case Console_KEY_ENTER_1:
                    memcpy(console_str->string, cmd_line_input.string, sizeof(ConsoleLogString));
                    clear_input();
                    cmd_line_input_index = 0;
                    return true;
                case Console_KEY_ENTER_2:
                    memcpy(console_str->string, cmd_line_input.string, sizeof(ConsoleLogString));
                    clear_input();
                    cmd_line_input_index = 0;
                    return true;
                case Console_KEY_BACKSPACE_1:
                    console_backspace();
                    break;
                case Console_KEY_BACKSPACE_2:
                    console_backspace();
                    break;
                case Console_KEY_BACKSPACE_3:
                    console_backspace();
                    break;
                default:
                    if(cmd_line_input_index < MAX_LOG_STR) {
                        cmd_line_input.string[cmd_line_input_index] = ch;
                        cmd_line_input_index += 1;
                    }
                    break;
            }
            print_input();
            return false;
    }
    return false;
}

char source_symbol(LogSourceType source) {
    char result = 0;
    switch(source) {
        case LOG_ENGINE:
             result = '#';
             break;
        case LOG_APP:
             result = '*';
             break;
        case LOG_ERROR:
             result = '!';
             break;
        case LOG_CONSOLE:
             result = '>';
             break;
        default:
             result = 'a';
             break;
    }

    return result;
}

void console_write(LogSourceType source, const char *fmt, ...)
{
        ConsoleLogString str_buff = {0};

    str_buff.string[0] = '[';
    str_buff.string[1] = source_symbol(source);
    str_buff.string[2] = ']';

    va_list args;
    va_start(args, fmt);

    vsnprintf(
        &str_buff.string[3],
        sizeof(str_buff.string) - 3,
        fmt,
        args
    );

    va_end(args);

    log_input(str_buff);
}
