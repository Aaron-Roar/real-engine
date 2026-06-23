#include "console.h"
#include <stdio.h>
#include <stdarg.h>

const char console_legend[] = "Console Legend\n[#] Engine\n[!] Error\n[*] Application\n\n";

void console_write(SourceType source, const char *fmt, ...)
{
    char symbol = '0';
    switch(source) {
        case ENGINE:
            symbol = '#';
            break;
        case ERROR:
            symbol = '!';
            break;
        case APP:
            symbol = '*';
            break;
        default:
            symbol = '#';
    }

    va_list args;
    va_start(args, fmt);
    printf("[%c]", symbol);
    vprintf(fmt, args);
    va_end(args);
}
