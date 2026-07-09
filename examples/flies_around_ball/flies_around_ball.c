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
#define amount_of_entities 20
AnimationAsset animation = {0};
AnimatedSprite sprite = {0};

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

    animation = load_animation(renderer, elderfly_fly_files);
    sprite = create_animated_sprite(animation, 3);

    Entity magnet_smash = add_entity();
    set_position(magnet_smash, (Position){.x = 400, .y = 200});
    set_orientation(magnet_smash, 1);
    set_mass(magnet_smash, 5000);
    set_velocity(magnet_smash, (Velocity){0, 0});
    //set_angular_velocity(water_smash, 3);
    set_acceleration(magnet_smash, (Acceleration){0, 0});
    set_restitution(magnet_smash, 0.7);
    Shape shape4 = create_circle(50, 4);
    set_hitbox(magnet_smash, shape4);
    set_friction(magnet_smash, 0.4);
    set_dynamic(magnet_smash);
    set_axis_lock(magnet_smash, (Axis){1,0}, positions[magnet_smash]);
    //set_angle_lock(magnet_smash, 0, 0);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < amount_of_entities; i += 1) {
        Entity ball = add_entity();
        set_position(ball, (Position){.x = tools_random_range(100, 400), .y = tools_random_range(0, 300)});
        set_orientation(ball, tools_random_range(0, 2*PI_F));
        set_mass(ball, 10);
        set_velocity(ball, (Velocity){.x = tools_random_range(-10, 10), .y = tools_random_range(0, 100)});
        set_acceleration(ball, (Acceleration){tools_random_range(0,10), 50});
        set_restitution(ball, 0.1);
        float size = tools_random_range_float(10, 20);
        Shape shape3 = create_circle(size, 10);
        set_hitbox(ball, shape3);
        set_friction(ball, 0.4);
        set_dynamic(ball);
        //set_transform_lock(ball, water_smash, (Vec2D){tools_random_range(100, 400), tools_random_range(100, 400)}, tools_random_range(0, 10), true, true, false);
        set_joint(ball, magnet_smash, JOINT_DISTANCE, (Vec2D){0}, (Vec2D){0}, 10, 0);
        sprite = create_animated_sprite(animation, size/10);
        sprite.animation.time_per_frame = tools_random_range_float(0.005, 0.5);
        add_animated_sprite(ball, sprite);
        //add_components(ball, PARTICLE);
    }


    //Game Loop
    while (console_is_active()) {
        clean_entities_past_lifetime();
        //Console
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }

        //Game Code
        if(engine_get_tick() % 10000 == 0) {
            for(int i = 0; i < MAX_ENTITIES; i += 1) {
                if( (entity_mask[i] & JOINT) == JOINT) {
                    if(joints[i].type == JOINT_PIN) {
                        joints[i].type = JOINT_DISTANCE;
                        velocities[magnet_smash].x = -50;
                        set_velocity(magnet_smash, (Velocity){50,0});
                        set_angular_velocity(magnet_smash, 3);
                    } else {
                        joints[i].type = JOINT_PIN;
                        velocities[magnet_smash].x = 50;
                        set_velocity(magnet_smash, (Velocity){-50, 0});
                        set_angular_velocity(magnet_smash, -3);
                    }
                }
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
        draw_hit_boxes(renderer);
        update_sprite_frames(engine_get_tick(), engine_get_time());
        draw_animated_sprites(renderer);
        show_graphics(renderer);

    }
    graphics_end(renderer, window);
    engine_shutdown();
}

