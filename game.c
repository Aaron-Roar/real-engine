#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include "graphics.h"
#include "console.h"
#include "engine.h"
#include "math2d.h"
#include <stdio.h>
#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>

float pi = 3.14;
const Color background_color = (Color){0,0,255,255};
const Color shape_color = (Color){255,0,0,255};

int main() {
    console_init();
    engine_init();
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    SDL_Event event = {0};
    if (!graphics_start(&renderer, &window)) {
    engine_shutdown();
    return 1;
}

    //Game setup
    //
    time_t start_time = tools_get_time();
    double prev_time = 0;
    double current_time = 0;

    Entity rock = add_entity();
    set_position(rock, (Position){.x = 30, .y = 0});
    set_orientation(rock, 20*(2*pi/360));
    set_mass(rock, 1000);
    set_velocity(rock, (Velocity){.x = 5, .y = 5});
    Shape box = create_square(20, 20);
    set_hitbox(rock, box);

    Entity ball = add_entity();
    set_position(ball, (Position){.x = 200, .y = 200});
    set_orientation(ball, 20*(2*pi/360));
    set_mass(ball, 1000);
    set_velocity(ball, (Velocity){.x = -15, .y = -15});
    set_angular_velocity(ball, 5);
    Shape circle = create_circle(90, 4);
    set_hitbox(ball, circle);

    //Game Loop
    while (console_is_active()) {

        //Console
        //
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);

        }
        //parse console cmd
        //execite console cmd
        //refresh();
        //


        //physics
        //
        prev_time = current_time;
        current_time = tools_get_time() - start_time;
        double dt = current_time - prev_time;
        system_update_physics(dt);

        //render
        //
        graphics_poll_events(&event);
        draw_background(renderer, background_color);
        
        Shape world_shape_rock = shape_world_translate(hit_boxes[rock], positions[rock], orientations[rock]);
        Shape world_shape_ball = shape_world_translate(hit_boxes[ball], positions[ball], orientations[ball]);
        if(shape_overlap(world_shape_ball, world_shape_rock)) {
            draw_shape_filled(renderer, world_shape_rock, shape_color);
            draw_shape_filled(renderer, world_shape_ball, shape_color);
        }
        else {
            draw_shape_outline(renderer, world_shape_rock, shape_color);
            draw_shape_outline(renderer, world_shape_ball, shape_color);
        }
        show_graphics(renderer);
        //

    }
    graphics_end(renderer, window);
    engine_shutdown();
}

