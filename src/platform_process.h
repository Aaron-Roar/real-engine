#ifndef PLATFORM_PROCESS_H
#define PLATFORM_PROCESS_H

#include <stdio.h>

FILE *platform_process_open_write(const char *command);
int platform_process_close(FILE *process);

#endif
