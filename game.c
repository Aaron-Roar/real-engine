#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include "graphics.h"
#include "console.h"
#include "engine.h"
#include "math2d.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


float pi = 3.14;
const Color background_color = (Color){0,0,255,255};
const Color shape_color = (Color){255,0,0,255};

int random_range(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

int main() {
    console_init();
    console_set_debug(CONSOLE_DEBUG_OFF);
    engine_init();
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    SDL_Event event = {0};
    if (!graphics_start(&renderer, &window)) {
        engine_shutdown();
        return 1;
    }

    //Initializing entity plate
    Entity plate = add_entity();
    set_position(plate, (Position){.x = 100, .y = 500});
    set_orientation(plate, 0);
    set_mass(plate, 5000);
    set_velocity(plate, (Velocity){0, 0});
    set_restitution(plate, 0.9);
    Shape shape1 = create_square(1000, 50);
    set_hitbox(plate, shape1);
    set_friction(plate, 0.5);
    set_static(plate);

    Entity plate2 = add_entity();
    set_position(plate2, (Position){.x = 100, .y = -10});
    set_orientation(plate2, 0);
    set_mass(plate2, 5000);
    set_velocity(plate2, (Velocity){0, 200});
    set_acceleration(plate2, (Acceleration){0, 50});
    set_restitution(plate2, 0.6);
    Shape shape2 = create_circle(50, 10);
    set_hitbox(plate2, shape2);
    set_friction(plate2, 0);
    set_static(plate2);
    set_life_time(plate2,10,0);
    //set_axis_lock(plate2,(Position){0,5}, positions[plate2]);
    //set_angle_lock(plate2,0,0.1);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < 3; i += 1) {
        Entity ball = add_entity();
        set_position(ball, (Position){.x = random_range(0, 400), .y = random_range(0, 300)});
        set_orientation(ball, random_range(0, 2*pi));
        set_mass(ball, 10);
        //set_velocity(ball, (Velocity){.x = 0, .y = -90});
        set_velocity(ball, (Velocity){.x = random_range(-10, 10), .y = random_range(0, 100)});
        set_acceleration(ball, (Acceleration){random_range(0,10), 50});
        set_restitution(ball, (float)rand()/(float)(RAND_MAX/1));
        //set_torque(ball, 2000);
        Shape shape3 = create_circle(30, random_range(3, 10));
        set_hitbox(ball, shape3);
        set_friction(ball, 0.5);
        set_dynamic(ball);
        //set_transform_lock(ball, plate2, (Vec2D){random_range(100, 400), random_range(100, 400)}, random_range(0, 10), true, true, false);
        set_joint(ball, plate2, JOINT_DISTANCE, (Vec2D){0}, (Vec2D){0}, 10, 0);
    }

    //Game Loop
    while (console_is_active()) {
        clean_entities_past_lifetime();
        //Console
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }

        //physics
        engine_update_time();
        engine_update_tick();
        apply_collisions();
        system_update_physics(engine_get_dt());

        //render
        graphics_poll_events(&event);
        draw_background(renderer, background_color);
        draw_hit_boxes(renderer);
        show_graphics(renderer);

        //App 
        console_write(LOG_APP, "Collision Count: %d\n", collision_count);
    }
    graphics_end(renderer, window);
    engine_shutdown();
}

