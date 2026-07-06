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

#include "examples.h"


const Color background_color = (Color){0,0,255,255};

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

    //Entity smash = magnetic_sim_init();
    Entity wall_1 = add_entity();
    set_static(wall_1);
    set_position(wall_1, (Position){80, 390});
    set_orientation(wall_1, 0*(pi/180));
    set_restitution(wall_1, 1);
    set_friction(wall_1, 0.5);
    Shape shape_1 = create_square(40, 150);
    set_hitbox(wall_1, shape_1);

    Entity wall_2 = add_entity();
    set_static(wall_2);
    set_position(wall_2, (Position){520, 390});
    set_orientation(wall_2, 0*(pi/180));
    set_restitution(wall_2, 1);
    set_friction(wall_2, 0);
    set_hitbox(wall_2, shape_1);

    Entity wall_3 = add_entity();
    set_static(wall_3);
    set_position(wall_3, (Position){300, 450});
    set_orientation(wall_3, 0*(pi/180));
    set_restitution(wall_3, 1);
    set_friction(wall_3, 0.1);
    Shape shape_2 = create_square(400, 40);
    set_hitbox(wall_3, shape_2);

    Entity smash = add_entity();
    set_position(smash, (Position){.x = 200, .y = -300});
    set_orientation(smash, 0);
    set_mass(smash, 50);
    set_velocity(smash, (Velocity){0, 0});
    set_acceleration(smash, (Acceleration){0, 30});
    set_restitution(smash, 1);
    Shape shape4 = create_circle(50, 10);
    set_hitbox(smash, shape4);
    set_friction(smash, 0.4);
    set_dynamic(smash);
    //set_axis_lock(smash, (Axis){0,1}, positions[smash]);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < 250; i += 1) {
        Entity ball = add_entity();
        set_position(ball, (Position){.x = tools_random_range(100, 400), .y = tools_random_range(0, 300)});
        set_orientation(ball, tools_random_range(0, 2*pi));
        set_mass(ball, 2);
        set_velocity(ball, (Velocity){.x = tools_random_range(-10, 10), .y = tools_random_range(0, 100)});
        set_acceleration(ball, (Acceleration){tools_random_range(0,10), 50});
        set_restitution(ball, 0.1);
        Shape shape3 = create_circle(7, 10);
        set_hitbox(ball, shape3);
        set_friction(ball, 0.1);
        set_dynamic(ball);
    }

    //Game Loop
    while (console_is_active()) {
        clean_entities_past_lifetime();
        //Console
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }

        //magnetic_sim_tick(smash);
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

