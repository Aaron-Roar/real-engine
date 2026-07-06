#ifndef TOOLS_H
#define TOOLS_H
#include "stdint.h"
#include "stddef.h"
#include <time.h>

void delay(int seconds);
void binary_to_string(uint32_t value, char* buffer, size_t size);
void tools_append_string(char* src, char* dst, size_t src_size, size_t dst_size);
uint32_t tool_sizeof_string(char* str, char delimiter);
int tools_random_range(int min, int max);
#endif
