#include "rohr.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "examples/test-assets/elder-fly/elderfly_descriptors.h"

const Color background_color = (Color){255,255,255,255};
const int amount_of_entities = 20;
AnimationAsset animation = {0};
AnimatedSprite sprite = {0};

int main() {
    EngineResult result;
    EntityResult entity_result;
    AnimationAssetResult animation_result;

    rohr_console_init();
    rohr_console_set_debug(CONSOLE_DEBUG_OFF);
    if(rohr_error_check(result = rohr_engine_init())) {
        rohr_console_write(LOG_ENGINE, rohr_error_default_message(result.result.error));
        return 1;
    }
    rohr_engine_set_dt(1/(float)120);
    if(rohr_error_check(result = rohr_level_editor_init())) {
        rohr_console_write(LOG_ENGINE, rohr_error_default_message(result.result.error));
        rohr_engine_shutdown();
        return 1;
    }
    SDL_Event event = {0};
    if(rohr_error_check(result = rohr_graphics_start())) {
        rohr_console_write(LOG_ENGINE, rohr_error_default_message(result.result.error));
        rohr_engine_shutdown();
        return 1;
    }

    if(rohr_error_check(animation_result = rohr_graphics_load_animation(elderfly_fly_files))) {
        rohr_console_write(LOG_ENGINE, rohr_error_default_message(animation_result.result.error));
        goto fail;
    }
    animation = animation_result.result.value;
    sprite = rohr_graphics_create_animated_sprite(animation, (Scale){3,3});

    Entity magnet_smash;
    if(rohr_error_check(entity_result = rohr_entity_add())) {
        rohr_console_write(LOG_ENGINE, rohr_error_default_message(entity_result.result.error));
        goto fail;
    }
    magnet_smash = entity_result.result.value;
    rohr_physics_set_position(magnet_smash, (Position){.x = 0, .y = 100});
    rohr_physics_set_orientation(magnet_smash, 1);
    rohr_physics_set_mass(magnet_smash, 5000);
    rohr_physics_set_velocity(magnet_smash, (Velocity){0, 0});
    //set_angular_velocity(water_smash, 3);
    rohr_physics_set_acceleration(magnet_smash, (Acceleration){0, 0});
    rohr_physics_set_restitution(magnet_smash, 0.7);
    Shape shape4 = rohr_math_create_circle(50, 4);
    rohr_physics_set_hitbox(magnet_smash, shape4);
    rohr_physics_set_friction(magnet_smash, 0.4);
    rohr_physics_set_dynamic(magnet_smash);
    EntityIndex magnet_smash_index;
    if(rohr_entity_get_index(magnet_smash, &magnet_smash_index)) {
        rohr_physics_set_axis_lock(magnet_smash, (Axis){1,0}, positions[magnet_smash_index]);
    }
    //set_angle_lock(magnet_smash, 0, 0);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < amount_of_entities; i += 1) {
        Entity ball;
        if(rohr_error_check(entity_result = rohr_entity_add())) {
            rohr_console_write(LOG_ENGINE, rohr_error_default_message(entity_result.result.error));
            goto fail;
        }
        ball = entity_result.result.value;
        rohr_physics_set_position(ball, (Position){.x = rohr_tools_random_range(100, 400), .y = rohr_tools_random_range(0, 300)});
        rohr_physics_set_orientation(ball, rohr_tools_random_range(0, 2*PI_F));
        rohr_physics_set_mass(ball, 10);
        rohr_physics_set_velocity(ball, (Velocity){.x = rohr_tools_random_range(-10, 10), .y = rohr_tools_random_range(0, 100)});
        rohr_physics_set_acceleration(ball, (Acceleration){rohr_tools_random_range(0,10), 50});
        rohr_physics_set_restitution(ball, 0.1);
        float size = rohr_tools_random_range_float(10, 20);
        Shape shape3 = rohr_math_create_circle(size, 5);
        rohr_physics_set_hitbox(ball, shape3);
        rohr_physics_set_friction(ball, 0.4);
        rohr_physics_set_dynamic(ball);
        //set_transform_lock(ball, water_smash, (Vec2D){rohr_tools_random_range(100, 400), rohr_tools_random_range(100, 400)}, rohr_tools_random_range(0, 10), true, true, false);
        if(rohr_error_check(entity_result = rohr_physics_set_joint(ball, magnet_smash, JOINT_DISTANCE, (Vec2D){0}, (Vec2D){0}, 10, 0))) {
            rohr_console_write(LOG_ENGINE, rohr_error_default_message(entity_result.result.error));
            goto fail;
        }
        sprite = rohr_graphics_create_animated_sprite(animation, (Scale){size/10, size/10});
        sprite.animation.time_per_frame = rohr_tools_random_range_float(0.005, 0.5);
        rohr_graphics_add_animated_sprite(ball, sprite);
        rohr_entity_add_components(ball, PARTICLE);
    }


    //Game Loop
    rohr_engine_reset_clock();
    rohr_graphics_recording_start("examples/flies-around-ball/recording.mp4", 60);
    bool phase_1 = false;
    bool phase_2 = false;
    bool phase_3 = false;
    while (rohr_console_is_active()) {
        rohr_system_clean_entities_past_lifetime();
        if(!phase_1 && rohr_engine_get_time() > 3) {
            phase_1 = true;
        }
        if(!phase_2 && rohr_engine_get_time() > 5) {
            phase_2 = true;
        }
        if(!phase_3 && rohr_engine_get_time() > 7) {
            phase_3 = true;
        }

        //Console
        ConsoleLogString console_line = {0};
        if(rohr_console_read(&console_line)) {
            rohr_console_write(LOG_CONSOLE, "%s", console_line.string);
        }
        rohr_level_editor_update();

        //Game Code
        if(rohr_engine_get_tick() % 1000 == 0) {
            for(int i = 0; i < MAX_ENTITIES; i += 1) {
                if(!rohr_entity_index_is_alive(i)) {
                    continue;
                }
                if( rohr_entity_index_has_components(i, JOINT)) {
                    if(joints[i].type == JOINT_PIN) {
                        joints[i].type = JOINT_DISTANCE;
                        rohr_physics_set_acceleration(magnet_smash, (Velocity){2,0});
                        rohr_physics_set_angular_velocity(magnet_smash, 3);
                    } else {
                        joints[i].type = JOINT_PIN;
                        rohr_physics_set_acceleration(magnet_smash, (Velocity){-2, 0});
                        rohr_physics_set_angular_velocity(magnet_smash, -3);
                    }
                }
            }
        }

        //physics
        rohr_engine_update_time();
        rohr_engine_update_tick();
        rohr_system_update_physics(rohr_engine_get_dt());

        //render
        rohr_graphics_draw_background(background_color);
        rohr_graphics_update_sprite_frames(rohr_engine_get_tick(), rohr_engine_get_time());
        rohr_graphics_draw_animated_sprites();
        if(phase_1) {
            rohr_graphics_draw_hit_boxes();
        }
        if(phase_2) {
            rohr_graphics_draw_particles();
        }
        if(phase_3) {
            rohr_graphics_draw_grid();
            rohr_graphics_draw_local_origins();
        }
        rohr_graphics_show();

    }
    rohr_graphics_end();
    rohr_engine_shutdown();
    return 0;

fail:
    rohr_graphics_end();
    rohr_engine_shutdown();
    return 1;
}
