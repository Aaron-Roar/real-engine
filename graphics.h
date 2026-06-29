#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "entity_components.h"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

bool graphics_start();
void graphics_end();
void graphics_poll_events();
void draw_background();
void draw_rect(Shape rect, Position pos);
void show_graphics();

#endif
