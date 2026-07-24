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
#include "level_editor.h"
#include "examples/test-assets/elder-fly/elderfly_descriptors.h"

const Color background_color = (Color){255,255,255,255};
#define amount_of_entities 20
AnimationAsset animation = {0};
AnimatedSprite sprite = {0};

static bool example_add_entity(Entity *entity) {
    EntityResult result = entity_add();

    if(entity == NULL) {
        return false;
    }
    if(result.kind == ERROR_RESULT_ERROR) {
        console_write(LOG_ENGINE, "Error: failed to add entity: %s\n", error_string(result.result.error));
        return false;
    }
    *entity = result.result.value;
    return true;
}

static bool example_load_animation(AnimationDescriptor descriptor, AnimationAsset *asset) {
    AnimationAssetResult result = graphics_load_animation(descriptor);

    if(asset == NULL) {
        return false;
    }
    if(result.kind == ERROR_RESULT_ERROR) {
        console_write(LOG_ENGINE, "Error: failed to load animation: %s\n", error_string(result.result.error));
        return false;
    }
    *asset = result.result.value;
    return true;
}

int main() {
    console_init();
    console_set_debug(CONSOLE_DEBUG_OFF);
    EngineResult engine_result = engine_init();
    if(engine_result.kind == ERROR_RESULT_ERROR) {
        console_write(LOG_ENGINE, "Error: failed to initialize engine: %s\n", error_string(engine_result.result.error));
        return 1;
    }
    EngineResult level_editor_result = level_editor_init();
    if(level_editor_result.kind == ERROR_RESULT_ERROR) {
        console_write(LOG_ENGINE, "Error: failed to initialize level editor: %s\n", error_string(level_editor_result.result.error));
        engine_shutdown();
        return 1;
    }
    SDL_Event event = {0};
    EngineResult graphics_result = graphics_start();
    if(graphics_result.kind == ERROR_RESULT_ERROR) {
        console_write(LOG_ENGINE, "Error: failed to initialize graphics: %s\n", error_string(graphics_result.result.error));
        engine_shutdown();
        return 1;
    }

    if(!example_load_animation(elderfly_fly_files, &animation)) {
        engine_shutdown();
        return 1;
    }
    sprite = graphics_create_animated_sprite(animation, (Scale){3,3});

    Entity magnet_smash;
    if(!example_add_entity(&magnet_smash)) {
        engine_shutdown();
        return 1;
    }
    physics_set_position(magnet_smash, (Position){.x = 0, .y = 100});
    physics_set_orientation(magnet_smash, 1);
    physics_set_mass(magnet_smash, 5000);
    physics_set_velocity(magnet_smash, (Velocity){0, 0});
    //set_angular_velocity(water_smash, 3);
    physics_set_acceleration(magnet_smash, (Acceleration){0, 0});
    physics_set_restitution(magnet_smash, 0.7);
    Shape shape4 = math_create_circle(50, 4);
    physics_set_hitbox(magnet_smash, shape4);
    physics_set_friction(magnet_smash, 0.4);
    physics_set_dynamic(magnet_smash);
    EntityIndex magnet_smash_index;
    if(entity_get_index(magnet_smash, &magnet_smash_index)) {
        physics_set_axis_lock(magnet_smash, (Axis){1,0}, positions[magnet_smash_index]);
    }
    //set_angle_lock(magnet_smash, 0, 0);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < amount_of_entities; i += 1) {
        Entity ball;
        if(!example_add_entity(&ball)) {
            engine_shutdown();
            return 1;
        }
        physics_set_position(ball, (Position){.x = tools_random_range(100, 400), .y = tools_random_range(0, 300)});
        physics_set_orientation(ball, tools_random_range(0, 2*PI_F));
        physics_set_mass(ball, 10);
        physics_set_velocity(ball, (Velocity){.x = tools_random_range(-10, 10), .y = tools_random_range(0, 100)});
        physics_set_acceleration(ball, (Acceleration){tools_random_range(0,10), 50});
        physics_set_restitution(ball, 0.1);
        float size = tools_random_range_float(10, 20);
        Shape shape3 = math_create_circle(size, 5);
        physics_set_hitbox(ball, shape3);
        physics_set_friction(ball, 0.4);
        physics_set_dynamic(ball);
        //set_transform_lock(ball, water_smash, (Vec2D){tools_random_range(100, 400), tools_random_range(100, 400)}, tools_random_range(0, 10), true, true, false);
        EntityResult joint_result = physics_set_joint(ball, magnet_smash, JOINT_DISTANCE, (Vec2D){0}, (Vec2D){0}, 10, 0);
        if(joint_result.kind == ERROR_RESULT_ERROR) {
            console_write(LOG_ENGINE, "Error: failed to create joint: %s\n", error_string(joint_result.result.error));
            engine_shutdown();
            return 1;
        }
        sprite = graphics_create_animated_sprite(animation, (Scale){size/10, size/10});
        sprite.animation.time_per_frame = tools_random_range_float(0.005, 0.5);
        graphics_add_animated_sprite(ball, sprite);
        entity_add_components(ball, PARTICLE);
    }


    //Game Loop
    while (console_is_active()) {
        system_clean_entities_past_lifetime();
        //Console
        ConsoleLogString console_line = {0};
        if(console_read(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }
        level_editor_update();

        //Game Code
        if(engine_get_tick() % 1000 == 0) {
            for(int i = 0; i < MAX_ENTITIES; i += 1) {
                if(!entity_index_is_alive(i)) {
                    continue;
                }
                if( entity_index_has_components(i, JOINT)) {
                    if(joints[i].type == JOINT_PIN) {
                        joints[i].type = JOINT_DISTANCE;
                        physics_set_acceleration(magnet_smash, (Velocity){2,0});
                        physics_set_angular_velocity(magnet_smash, 3);
                    } else {
                        joints[i].type = JOINT_PIN;
                        physics_set_acceleration(magnet_smash, (Velocity){-2, 0});
                        physics_set_angular_velocity(magnet_smash, -3);
                    }
                }
            }
        }

        //physics
        engine_update_time();
        engine_update_tick();
        system_update_physics(engine_get_dt());

        //render
        graphics_draw_background(background_color);
        graphics_draw_hit_boxes();
        graphics_update_sprite_frames(engine_get_tick(), engine_get_time());
        graphics_draw_animated_sprites();
        graphics_show();

    }
    graphics_end();
    engine_shutdown();
}
