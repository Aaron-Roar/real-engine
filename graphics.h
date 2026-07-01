#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL.h>
#include "entity_components.h"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480


typedef enum Fill {
    GRAPHICS_OUTLINE,
    GRAPHICS_FILLED
} Fill;

typedef struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
} Color;

Color creat_color_hex(uint32_t hex);
Color creat_color_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
bool graphics_start(SDL_Renderer **renderer, SDL_Window **window);
void graphics_end(SDL_Renderer *renderer, SDL_Window *window);
bool graphics_poll_events(SDL_Event *event);
void draw_background(SDL_Renderer *renderer, Color color);
void show_graphics(SDL_Renderer *renderer);
bool draw_shape_outline(SDL_Renderer *renderer, Shape shape, Color color);
bool draw_shape_filled(SDL_Renderer *renderer, Shape shape, Color color);
void draw_hit_box(SDL_Renderer *renderer, Entity e, Fill fill);

#endif
