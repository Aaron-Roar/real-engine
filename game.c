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
    time_t start_time = tools_get_time();
    double prev_time = 0;
    double current_time = 0;

    //Initializing entity rock
    Entity rock = add_entity();
    set_position(rock, (Position){.x = 100, .y = 400});
    set_orientation(rock, 0);
    set_mass(rock, 5);
    //set_velocity(rock, (Velocity){.x = 0, .y = 0});
    set_restitution(rock, 1);
    Shape shape1 = create_square(500, 50);
    set_hitbox(rock, shape1);
    set_static(rock);

    //Initializing entity ball
    Entity ball = add_entity();
    set_position(ball, (Position){.x = 100, .y = 200});
    set_orientation(ball, 0);
    set_mass(ball, 100);
    //set_velocity(ball, (Velocity){.x = 0, .y = -90});
    set_velocity(ball, (Velocity){.x = 0, .y = 50});
    set_acceleration(ball, (Acceleration){0, 0});
    set_restitution(ball, 1);
    //set_torque(ball, 2000);
    Shape shape2 = create_circle(30, 7);
    set_hitbox(ball, shape2);

    Entity ball1 = add_entity();
    set_position(ball1, (Position){.x = 100, .y = 100});
    set_orientation(ball1, 0);
    set_mass(ball1, 100);
    //set_velocity(ball, (Velocity){.x = 0, .y = -90});
    set_velocity(ball1, (Velocity){.x = 0, .y = 50});
    set_acceleration(ball1, (Acceleration){0, 0});
    set_restitution(ball1, 1);
    //set_torque(ball, 2000);
    Shape shape3 = create_circle(30, 4);
    set_hitbox(ball1, shape3);

    //Game Loop
    while (console_is_active()) {

        //Console
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }

        //physics
        prev_time = current_time;
        current_time = tools_get_time() - start_time;
        double dt = current_time - prev_time;
        system_update_physics(dt);

        //render
        graphics_poll_events(&event);
        draw_background(renderer, background_color);
        draw_hit_box(renderer, rock, GRAPHICS_OUTLINE);
        draw_hit_box(renderer, ball, GRAPHICS_OUTLINE);
        draw_hit_box(renderer, ball1, GRAPHICS_OUTLINE);
        apply_collisions();

        show_graphics(renderer);
    }
    graphics_end(renderer, window);
    engine_shutdown();
}

