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

float i = 1;
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
    set_position(rock, (Position){.x = 30, .y = 0});
    set_orientation(rock, 20*(2*pi/360));
    set_mass(rock, 10);
    set_velocity(rock, (Velocity){.x = 5, .y = 5});
    Shape shape1 = create_circle(20, 20);
    set_hitbox(rock, shape1);

    //Initializing entity ball
    Entity ball = add_entity();
    set_position(ball, (Position){.x = 200, .y = 200});
    set_orientation(ball, 20*(2*pi/360));
    set_mass(ball, 1000);
    set_velocity(ball, (Velocity){.x = -15, .y = -15});
    set_torque(ball, 2000);
    Shape shape2 = create_circle(i, 4);
    set_hitbox(ball, shape2);

    //Game Loop
    while (console_is_active()) {
        shape2 = create_circle(i, 4);
        set_hitbox(ball, shape2);
        i += 0.001;

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
        if(system_get_entity_collision(ball, rock).overlap) {
            draw_hit_box(renderer, rock, GRAPHICS_FILLED);
            draw_hit_box(renderer, ball, GRAPHICS_FILLED);
        } else {
            draw_hit_box(renderer, rock, GRAPHICS_OUTLINE);
            draw_hit_box(renderer, ball, GRAPHICS_OUTLINE);
        }

        show_graphics(renderer);
    }
    graphics_end(renderer, window);
    engine_shutdown();
}

