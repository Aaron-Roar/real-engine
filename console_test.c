#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define MAX_LOGS 100
#define MAX_LOG 50
#define LOG_ROW_OFFSET 2
#define INPUT_ROW_OFFSET 1
#define INPUT_COL_OFFSET 3
#define CURSOR_HOME_ROW 1
#define CURSOR_HOME_COL 3


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

ConsoleLog console_log = {0};
int log_index = 0;
bool new_log = false;
int input_index = 0;
ConsoleInput input = {0};

TermWindow capture_window() {
    TermWindow window = {0};
    getmaxyx(stdscr, window.rows, window.cols);
    return window;
}

int count_logs(ConsoleLog logs) {
    int log_count = 0;
    for(int i = 0; i < MAX_LOGS; i++) {
        if(logs[i][0] != 0)
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

void clear_logs(ConsoleLog logs) {
    TermWindow window = capture_window();

    int log_size = count_logs(logs);
    int log_overflow = (log_size > (window.rows - LOG_ROW_OFFSET)) ? log_size - (window.rows - LOG_ROW_OFFSET) : 0; //If log_size > window.rows ? true : false;
    for(int i = 0; i < log_size - log_overflow; i++) {
        move(i, 0);
        clrtoeol();
    }
}

void print_logs(ConsoleLog logs) {
    TermWindow window = capture_window();

    clear_logs(logs);
    int log_size = count_logs(logs);
    int log_overflow = (log_size > (window.rows - LOG_ROW_OFFSET)) ? log_size - (window.rows - LOG_ROW_OFFSET) : 0; //If log_size > window.rows ? true : false;
    for(int i = 0; i < log_size - log_overflow; i++) {
        mvprintw(i, 0, "%s", logs[i + log_overflow]);
    }
}


void print_input(ConsoleInput input) {
    TermWindow w = capture_window();
    char input_line[w.cols - 1];
    memset(input_line, '-', sizeof(input_line));
    input_line[w.cols - 2] = '\0';

    mvprintw(w.rows - LOG_ROW_OFFSET, 0, "%s", input_line);
    clear_row(w.rows - INPUT_ROW_OFFSET);
    mvprintw(w.rows - INPUT_ROW_OFFSET, 0, ">>%s", input);
}

void log_input(ConsoleInput input) {
    if(log_index >= MAX_LOGS) {
        //Error
    }
    else {
        input[MAX_LOG - 1] = '\0';
        memcpy(&console_log[log_index], input, sizeof(ConsoleInput));
        log_index += 1;
    }
    print_logs(console_log);
}

void clear_input(ConsoleInput input) {
    TermWindow w = capture_window();

    memset(input, 0, sizeof(ConsoleInput));
    clear_row(w.rows - INPUT_ROW_OFFSET);
}

void console_init() {
    initscr(); //Hands control of the terminal to <ncurses.h>
    cbreak(); //Makes input available imediatley instead of waiting for enter press
    noecho(); //Stops the terminal from auto showing typed characters. We want to do this our own way instead.
    keypad(stdscr, TRUE); //Allows ncurses to detect special keypresses
    nodelay(stdscr, TRUE); //Makes getch return immediatley and not hang. it returns ERR if nothing input
}

void console_backspace() {
    if(input_index != 0) {
        input[input_index] = 0;
        input[input_index - 1] = 0;
        input_index -= 1;
    }
}

void console_shutdown() {
    endwin(); //Kills ncurses now terminal is back to normal
}

bool read_console(ConsoleInput console_str) {
    TermWindow w = capture_window();
    move(w.rows - INPUT_ROW_OFFSET, input_index + 2);

        int ch = getch();
        switch(ch) {
            case KEY_NONE:
                break;
            case KEY_ESC:
                //Exit ??
                console_shutdown();
                break;
            case KEY_ENTER_1:
                memcpy(console_str, input, sizeof(ConsoleInput));
                clear_input(input);
                input_index = 0;
                return true;
            case KEY_ENTER_2:
                memcpy(console_str, input, sizeof(ConsoleInput));
                clear_input(input);
                input_index = 0;
                return true;
            case KEY_BACKSPACE_1:
                console_backspace();
                break;
            case KEY_BACKSPACE_2:
                console_backspace();
                break;
            case KEY_BACKSPACE_3:
                console_backspace();
                break;
            default:
                input[input_index] = ch;
                input_index += 1;
                break;
        }
        print_input(input);
        return false;
}

//ConsoleCmd console_parse_cmd(ConsoleInput cmd_str) {
//
//}

int main() {
    console_init();

    while(true) {
        ConsoleInput console_line = {0};
        if(read_console(console_line)) {
            log_input(console_line);
        }
        refresh();
    }

    //console_shutdown();

}
        //console_cmd = Read console
        //console_log(cmd)
        //run_cmd
