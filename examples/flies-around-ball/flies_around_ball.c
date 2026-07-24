#include "rohr.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "examples/test-assets/elder-fly/elderfly_descriptors.h"

const Color background_color = (Color){255,255,255,255};
const int amount_of_entities = 20;
AnimationAsset animation = {0};
AnimatedSprite sprite = {0};
const Time demo_duration_seconds = 10.0;

#define PRINT_ENGINE_ERROR(engine_result) \
    fprintf(stderr, "%s\n", rohr_error_default_message((engine_result).result.error))

typedef struct ChildAccelerationData {
    Entity parent;
    float magnitude;
    bool away;
} ChildAccelerationData;

static EngineResult accelerate_child(Entity child, void *user_data) {
    ChildAccelerationData *data = user_data;

    if(data == NULL) {
        return rohr_error_result_error(ERROR_MEMORY_POOL_NULL_POINTER);
    }
    if(data->away) {
        return rohr_physics_set_acceleration_away_from_entity(child, data->magnitude, data->parent);
    }
    return rohr_physics_set_acceleration_toward_entity(child, data->magnitude, data->parent);
}

int main(void) {
    EngineResult result;
    EntityResult entity_result;
    ChildrenResult children_result;
    AnimationAssetResult animation_result;
    GroupId children_group = GROUP_INVALID;

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

    if(rohr_error_check(animation_result = rohr_graphics_load_animation(elderfly_fly_files))) {
        PRINT_ENGINE_ERROR(animation_result);
        goto fail;
    }
    animation = animation_result.result.value;
    sprite = rohr_graphics_create_animated_sprite(animation, (Scale){3,3});

    Entity ball;
    if(rohr_error_check(entity_result = rohr_entity_add())) {
        PRINT_ENGINE_ERROR(entity_result);
        goto fail;
    }
    ball = entity_result.result.value;
    rohr_physics_set_position(ball, (Position){.x = 0, .y = 100});
    rohr_physics_set_orientation(ball, 1);
    rohr_physics_set_mass(ball, 5000);
    rohr_physics_set_velocity(ball, (Velocity){0, 0});
    //set_angular_velocity(ball, 3);
    rohr_physics_set_acceleration(ball, (Acceleration){0, 0});
    rohr_physics_set_restitution(ball, 0.7);
    Shape ball_shape = rohr_math_create_circle(50, 4);
    rohr_physics_set_hitbox(ball, ball_shape);
    rohr_physics_set_friction(ball, 0.4);
    rohr_physics_set_static(ball);
    EntityIndex ball_index;
    if(rohr_entity_get_index(ball, &ball_index)) {
        rohr_physics_set_axis_lock(ball, (Axis){1,0}, positions[ball_index]);
    }
    //set_angle_lock(ball, 0, 0);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < amount_of_entities; i += 1) {
        Entity small_fly;
        if(rohr_error_check(entity_result = rohr_entity_add())) {
            PRINT_ENGINE_ERROR(entity_result);
            goto fail;
        }
        small_fly = entity_result.result.value;
        rohr_physics_set_position(small_fly, (Position){.x = rohr_tools_random_range(100, 400), .y = rohr_tools_random_range(0, 300)});
        rohr_physics_set_orientation(small_fly, rohr_tools_random_range(0, 2*PI_F));
        rohr_physics_set_mass(small_fly, 10);
        rohr_physics_set_velocity(small_fly, (Velocity){.x = rohr_tools_random_range(-10, 10), .y = rohr_tools_random_range(0, 100)});
        rohr_physics_set_acceleration(small_fly, (Acceleration){0, 0});
        rohr_physics_set_restitution(small_fly, 0.1);
        float size = rohr_tools_random_range_float(10, 20);
        Shape small_fly_shape = rohr_math_create_circle(size, 5);
        rohr_physics_set_hitbox(small_fly, small_fly_shape);
        rohr_physics_set_friction(small_fly, 0.4);
        rohr_physics_set_dynamic(small_fly);
        if(rohr_error_check(result = rohr_entity_set_parent(small_fly, ball))) {
            PRINT_ENGINE_ERROR(result);
            goto fail;
        }
        sprite = rohr_graphics_create_animated_sprite(animation, (Scale){size/10, size/10});
        sprite.animation.time_per_frame = rohr_tools_random_range_float(0.005, 0.5);
        rohr_graphics_add_animated_sprite(small_fly, sprite);
        rohr_entity_add_components(small_fly, PARTICLE);
    }

    if(rohr_error_check(children_result = rohr_entity_get_children(ball))) {
        PRINT_ENGINE_ERROR(children_result);
        goto fail;
    }
    children_group = children_result.result.value;

    //Game Loop
    rohr_engine_reset_clock();
    rohr_graphics_recording_start("examples/flies-around-ball/recording.mp4", 60);
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

        //Game Code
        Time time = rohr_engine_get_time();
        Time phase_time = time - ((int)(time / 4.0) * 4.0);
        ChildAccelerationData acceleration_data = {
            .parent = ball,
            .magnitude = 20.0f + (float)time * 12.0f,
            .away = phase_time >= 3.0
        };

        if(rohr_error_check(result = rohr_entity_group_for_each(children_group, accelerate_child, &acceleration_data))) {
            PRINT_ENGINE_ERROR(result);
            goto fail;
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
