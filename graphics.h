#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL.h>
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

void graphics_start();
void render_end(SDL_Renderer *r);
void window_end(SDL_Window *w);
void graphics_end();
void graphics_event_listener_start();

#endif
