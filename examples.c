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
#include "test-assets/elder-fly/elderfly_descriptors.h"
float pi = 3.14;

#define amount_of_entities 20
AnimationAsset animation = {0};
AnimatedSprite sprite = {0};


Entity magnet_smash = 0;
void magnetic_sim_init(SDL_Renderer *renderer) {
    animation = load_animation(renderer, elderfly_fly_files);
    sprite = create_animated_sprite(animation, 3);

    magnet_smash = add_entity();
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
        //sprites[i] = create_animated_sprite(&animation, 3);
        Entity ball = add_entity();
        set_position(ball, (Position){.x = tools_random_range(100, 400), .y = tools_random_range(0, 300)});
        set_orientation(ball, tools_random_range(0, 2*pi));
        set_mass(ball, 10);
        //set_velocity(ball, (Velocity){.x = 0, .y = -90});
        set_velocity(ball, (Velocity){.x = tools_random_range(-10, 10), .y = tools_random_range(0, 100)});
        set_acceleration(ball, (Acceleration){tools_random_range(0,10), 50});
        set_restitution(ball, 0.1);
        //set_torque(ball, 2000);
        Shape shape3 = create_circle(20, 10);
        set_hitbox(ball, shape3);
        set_friction(ball, 0.4);
        set_dynamic(ball);
        //set_transform_lock(ball, water_smash, (Vec2D){tools_random_range(100, 400), tools_random_range(100, 400)}, tools_random_range(0, 10), true, true, false);
        set_joint(ball, magnet_smash, JOINT_DISTANCE, (Vec2D){0}, (Vec2D){0}, 10, 0);
        add_animated_sprite(ball, sprite);
        //add_components(ball, PARTICLE);
    }
}

void magnetic_sim_tick(SDL_Renderer *renderer) {
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
}

Entity water_wall_1 = 0;
Entity water_wall_2 = 0;
Entity water_wall_3 = 0;
Entity water_smash = 0;
void water_sim_init(SDL_Renderer *renderer) {

    water_wall_1 = add_entity();
    set_static(water_wall_1);
    set_position(water_wall_1, (Position){80, 390});
    set_orientation(water_wall_1, 0*(pi/180));
    set_restitution(water_wall_1, 0.5);
    set_friction(water_wall_1, 0.5);
    Shape shape_1 = create_square(40, 150);
    set_hitbox(water_wall_1, shape_1);

    water_wall_2 = add_entity();
    set_static(water_wall_2);
    set_position(water_wall_2, (Position){520, 390});
    set_orientation(water_wall_2, 0*(pi/180));
    set_restitution(water_wall_2, 0.5);
    set_friction(water_wall_2, 0);
    set_hitbox(water_wall_2, shape_1);

    water_wall_3 = add_entity();
    set_static(water_wall_3);
    set_position(water_wall_3, (Position){300, 450});
    set_orientation(water_wall_3, 0*(pi/180));
    set_restitution(water_wall_3, 0.5);
    set_friction(water_wall_3, 0.1);
    Shape shape_2 = create_square(400, 40);
    set_hitbox(water_wall_3, shape_2);

    water_smash = add_entity();
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
        set_orientation(ball, tools_random_range(0, 2*pi));
        set_mass(ball, 0.001);
        set_velocity(ball, (Velocity){.x = tools_random_range(-10, 10), .y = tools_random_range(0, 100)});
        set_acceleration(ball, (Acceleration){tools_random_range(0,10), 50});
        set_restitution(ball, 0.3);
        float size = tools_random_range_float(10, 20);
        Shape shape3 = create_circle(20, 5);
        set_hitbox(ball, shape3);
        set_friction(ball, 0);
        set_dynamic(ball);
        animation = load_animation(renderer, elderfly_fly_files);
        sprite = create_animated_sprite(animation, size/10);
        sprite.animation.time_per_frame = tools_random_range_float(0.005, 0.5);
        add_animated_sprite(ball, sprite);
        //set_axis_lock(ball, (Axis){1, 0}, positions[ball]);
    }
}

void water_sim_tick(SDL_Renderer *renderer) {
        draw_hit_box(renderer, water_wall_1, GRAPHICS_FILLED);
        draw_hit_box(renderer, water_wall_2,GRAPHICS_FILLED);
        draw_hit_box(renderer, water_wall_3,GRAPHICS_FILLED);
        draw_hit_box(renderer, water_smash,GRAPHICS_FILLED);
}
