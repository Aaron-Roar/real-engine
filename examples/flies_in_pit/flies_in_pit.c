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
#include "examples/test-assets/elder-fly/elderfly_descriptors.h"

const Color background_color = (Color){255,255,255,255};
AnimationAsset animation = {0};
AnimatedSprite sprite = {0};
#define amount_of_entities 20

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

    Entity water_wall_1 = add_entity();
    set_static(water_wall_1);
    set_position(water_wall_1, (Position){80, 390});
    set_orientation(water_wall_1, 0*(PI_F/180));
    set_restitution(water_wall_1, 0.5);
    set_friction(water_wall_1, 0.5);
    Shape shape_1 = create_square(40, 150);
    set_hitbox(water_wall_1, shape_1);

    Entity water_wall_2 = add_entity();
    set_static(water_wall_2);
    set_position(water_wall_2, (Position){520, 390});
    set_orientation(water_wall_2, 0*(PI_F/180));
    set_restitution(water_wall_2, 0.5);
    set_friction(water_wall_2, 0);
    set_hitbox(water_wall_2, shape_1);

    Entity water_wall_3 = add_entity();
    set_static(water_wall_3);
    set_position(water_wall_3, (Position){300, 450});
    set_orientation(water_wall_3, 0*(PI_F/180));
    set_restitution(water_wall_3, 0.5);
    set_friction(water_wall_3, 0.1);
    Shape shape_2 = create_square(400, 40);
    set_hitbox(water_wall_3, shape_2);

    Entity water_smash = add_entity();
    set_position(water_smash, (Position){.x = 200, .y = -300});
    set_orientation(water_smash, 3);
    set_mass(water_smash, 500);
    set_velocity(water_smash, (Velocity){0, 0});
    set_acceleration(water_smash, (Acceleration){0, 30});
    set_restitution(water_smash, 0.7);
    Shape shape4 = create_circle(50, 10);
    set_hitbox(water_smash, shape4);
    set_friction(water_smash, 0.4);
    set_dynamic(water_smash);
    //set_axis_lock(water_smash, (Axis){0,1}, positions[smash]);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < amount_of_entities; i += 1) {
        Entity ball = add_entity();
        set_position(ball, (Position){.x = tools_random_range(100, 400), .y = tools_random_range(0, 300)});
        set_orientation(ball, tools_random_range(0, 2*PI_F));
        set_mass(ball, 0.001);
        set_velocity(ball, (Velocity){.x = tools_random_range(-10, 10), .y = tools_random_range(0, 100)});
        set_acceleration(ball, (Acceleration){tools_random_range(0,10), 50});
        set_restitution(ball, 0.3);
        float size = tools_random_range_float(10, 20);
        Shape shape3 = create_circle(size, 5);
        set_hitbox(ball, shape3);
        set_friction(ball, 0);
        set_dynamic(ball);
        animation = load_animation(renderer, elderfly_fly_files);
        sprite = create_animated_sprite(animation, size/10);
        sprite.animation.time_per_frame = tools_random_range_float(0.005, 0.5);
        add_animated_sprite(ball, sprite);
    }

    Time dt = engine_get_dt();
    //Game Loop
    while (console_is_active()) {
        clean_entities_past_lifetime();

        //Console
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }


        if (event.type == SDL_EVENT_KEY_UP && event.key.scancode == SDL_SCANCODE_SPACE) {
            engine_set_dt(-dt);
            for(int i = 0; i < MAX_ENTITIES; i += 1) {
                accelerations[i].x = -accelerations[i].x;
                accelerations[i].y = -accelerations[i].y;
            }
        }

        //physics
        engine_update_time();
        engine_update_tick();
        apply_collisions();
        system_update_physics(engine_get_dt());
        SDL_Event event = engine_poll_event();

        //render
        draw_background(renderer, background_color);
        draw_hit_box(renderer, water_wall_1, GRAPHICS_FILLED);
        draw_hit_box(renderer, water_wall_2,GRAPHICS_FILLED);
        draw_hit_box(renderer, water_wall_3,GRAPHICS_FILLED);
        draw_hit_box(renderer, water_smash,GRAPHICS_FILLED);
        //draw_hit_boxes(renderer);
        update_sprite_frames(engine_get_tick(), engine_get_time());
        draw_animated_sprites(renderer);
        show_graphics(renderer);

    }
    graphics_end(renderer, window);
    engine_shutdown();
}

