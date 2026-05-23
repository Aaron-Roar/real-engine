#ifndef TOOLS_H
#define TOOLS_H
#include "entity_components.h"
#include "stdint.h"
#include "stddef.h"
void delay(int seconds);
void tool_print_coponents(Entity e);
void binary_to_string(uint32_t value, char* buffer, size_t size);
void tools_append_string(char* src, char* dst, size_t src_size, size_t dst_size);
uint32_t tool_sizeof_string(char* str, char delimiter);
#endif
