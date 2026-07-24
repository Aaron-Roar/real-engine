#include "rohr.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "examples/test-assets/elder-fly/elderfly_descriptors.h"

const Color background_color = (Color){255,255,255,255};
AnimationAsset animation_elderfly = {0};
AnimatedSprite sprite_elderfly = {0};
const int amount_of_entities = 500;
const Time demo_duration_seconds = 10.0;

#define PRINT_ENGINE_ERROR(engine_result) \
    fprintf(stderr, "%s\n", rohr_error_default_message((engine_result).result.error))

int main(void) {
    EngineResult result;
    EntityResult entity_result;
    AnimationAssetResult animation_result;

    if(rohr_error_check(result = rohr_engine_init())) {
        PRINT_ENGINE_ERROR(result);
        return 1;
    }
    rohr_engine_set_dt(1/(float)120);
    if(rohr_error_check(result = rohr_graphics_start())) {
        PRINT_ENGINE_ERROR(result);
        rohr_engine_shutdown();
        return 1;
    }

    Entity wall_1;
    if(rohr_error_check(entity_result = rohr_entity_add())) {
        PRINT_ENGINE_ERROR(entity_result);
        goto fail;
    }
    wall_1 = entity_result.result.value;
    rohr_physics_set_static(wall_1);
    rohr_physics_set_position(wall_1, (Position){0, -80});
    rohr_physics_set_orientation(wall_1, 120*(PI_F/180));
    rohr_physics_set_restitution(wall_1, 0.5);
    rohr_physics_set_friction(wall_1, 1);
    Shape shape_1 = rohr_math_create_square(40, 400);
    rohr_physics_set_hitbox(wall_1, shape_1);

    Entity wall_2;
    if(rohr_error_check(entity_result = rohr_entity_add())) {
        PRINT_ENGINE_ERROR(entity_result);
        goto fail;
    }
    wall_2 = entity_result.result.value;
    rohr_physics_set_static(wall_2);
    rohr_physics_set_position(wall_2, (Position){-180, -10});
    rohr_physics_set_orientation(wall_2, 0*(PI_F/180));
    rohr_physics_set_restitution(wall_2, 0.5);
    rohr_physics_set_friction(wall_2, 1);
    Shape shape_2 = rohr_math_create_square(40, 60);
    rohr_physics_set_hitbox(wall_2, shape_2);

    Entity wall_3;
    if(rohr_error_check(entity_result = rohr_entity_add())) {
        PRINT_ENGINE_ERROR(entity_result);
        goto fail;
    }
    wall_3 = entity_result.result.value;
    rohr_physics_set_static(wall_3);
    rohr_physics_set_position(wall_3, (Position){180, -10});
    rohr_physics_set_orientation(wall_3, 0*(PI_F/180));
    rohr_physics_set_restitution(wall_3, 0.5);
    rohr_physics_set_friction(wall_3, 1);
    Shape shape_3 = rohr_math_create_square(40, 60);
    rohr_physics_set_hitbox(wall_3, shape_3);

    Entity large_fly;
    if(rohr_error_check(entity_result = rohr_entity_add())) {
        PRINT_ENGINE_ERROR(entity_result);
        goto fail;
    }
    large_fly = entity_result.result.value;
    rohr_physics_set_position(large_fly, (Position){.x = 0, .y = 300});
    rohr_physics_set_orientation(large_fly, 0);
    rohr_physics_set_mass(large_fly, 50);
    rohr_physics_set_velocity(large_fly, (Velocity){0, 0});
    rohr_physics_set_acceleration(large_fly, (Acceleration){0, -30});
    rohr_physics_set_restitution(large_fly, 0.1);
    Shape shape4 = rohr_math_create_square(150, 220);
    rohr_physics_set_hitbox(large_fly, shape4);
    rohr_physics_set_friction(large_fly, 0.4);
    rohr_physics_set_dynamic(large_fly);
    if(rohr_error_check(animation_result = rohr_graphics_load_animation(elderfly_fly_files))) {
        PRINT_ENGINE_ERROR(animation_result);
        goto fail;
    }
    animation_elderfly = animation_result.result.value;
    sprite_elderfly = rohr_graphics_create_animated_sprite(animation_elderfly, (Scale){10,10});
    rohr_graphics_add_animated_sprite(large_fly, sprite_elderfly);
    //set_axis_lock(large_fly, (Axis){0,1}, positions[smash]);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < amount_of_entities - 1; i += 1) {
        Entity small_fly;
        if(rohr_error_check(entity_result = rohr_entity_add())) {
            PRINT_ENGINE_ERROR(entity_result);
            goto fail;
        }
        small_fly = entity_result.result.value;
        rohr_physics_set_position(small_fly, (Position){.x = rohr_tools_random_range(-10, 10), .y = rohr_tools_random_range(100, 200)});
        rohr_physics_set_orientation(small_fly, rohr_tools_random_range(0, 2*PI_F));
        rohr_physics_set_mass(small_fly, 1);
        rohr_physics_set_velocity(small_fly, (Velocity){0, .y = rohr_tools_random_range(0, -10)});
        rohr_physics_set_acceleration(small_fly, (Acceleration){0, -50});
        rohr_physics_set_restitution(small_fly, 0.2);
        float size = rohr_tools_random_range_float(3, 5);
        Shape shape3 = rohr_math_create_circle(size, 3);
        rohr_physics_set_hitbox(small_fly, shape3);
        rohr_physics_set_friction(small_fly, 0.4);
        rohr_physics_set_dynamic(small_fly);
        sprite_elderfly = rohr_graphics_create_animated_sprite(animation_elderfly, (Scale){size/10,size/10});
        sprite_elderfly.animation.time_per_frame = rohr_tools_random_range_float(0.005, 0.5);
        rohr_graphics_add_animated_sprite(small_fly, sprite_elderfly);
        rohr_entity_add_components(small_fly, PARTICLE);
    }

    rohr_engine_reset_clock();
    //Game Loop
    rohr_graphics_recording_start("examples/flies-in-pit/recording.mp4",60);
    bool phase_1 = false;
    bool phase_2 = false;
    bool phase_3 = false;
    while (rohr_engine_get_time() < demo_duration_seconds) {
        rohr_system_clean_entities_past_lifetime();
        SDL_Event event = rohr_engine_poll_event();
        if(event.type == SDL_EVENT_QUIT) {
            break;
        }
        if(!phase_1 && rohr_engine_get_time() > 3) {
            phase_1 = true;
        }
        if(!phase_2 && rohr_engine_get_time() > 5) {
            phase_2 = true;
        }
        if(!phase_3 && rohr_engine_get_time() > 7) {
            phase_3 = true;
        }

        //physics
        rohr_engine_update_time();
        rohr_engine_update_tick();
        rohr_system_update_physics(rohr_engine_get_dt());

        //render
        rohr_graphics_draw_background(background_color);
        rohr_graphics_draw_hit_box(wall_1, GRAPHICS_FILLED);
        rohr_graphics_draw_hit_box(wall_2, GRAPHICS_FILLED);
        rohr_graphics_draw_hit_box(wall_3, GRAPHICS_FILLED);
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
        }
        rohr_graphics_draw_local_origins();
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
