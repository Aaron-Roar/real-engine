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

Entity magnetic_sim_init() {
    Entity smash = add_entity();
    set_position(smash, (Position){.x = 200, .y = 100});
    set_orientation(smash, 1);
    set_mass(smash, 5000);
    set_velocity(smash, (Velocity){0, 0});
    //set_angular_velocity(smash, 3);
    set_acceleration(smash, (Acceleration){0, 0});
    set_restitution(smash, 0.7);
    Shape shape4 = create_circle(50, 4);
    set_hitbox(smash, shape4);
    set_friction(smash, 0.4);
    set_dynamic(smash);
    set_axis_lock(smash, (Axis){1,0}, positions[smash]);

    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < 400; i += 1) {
        Entity ball = add_entity();
        set_position(ball, (Position){.x = tools_random_range(100, 400), .y = tools_random_range(0, 300)});
        set_orientation(ball, tools_random_range(0, 2*pi));
        set_mass(ball, 10);
        //set_velocity(ball, (Velocity){.x = 0, .y = -90});
        set_velocity(ball, (Velocity){.x = tools_random_range(-10, 10), .y = tools_random_range(0, 100)});
        set_acceleration(ball, (Acceleration){tools_random_range(0,10), 50});
        set_restitution(ball, 0.1);
        //set_torque(ball, 2000);
        Shape shape3 = create_circle(8, 10);
        set_hitbox(ball, shape3);
        set_friction(ball, 0.4);
        set_dynamic(ball);
        //set_transform_lock(ball, smash, (Vec2D){tools_random_range(100, 400), tools_random_range(100, 400)}, tools_random_range(0, 10), true, true, false);
        //set_joint(ball, smash, JOINT_DISTANCE, (Vec2D){0}, (Vec2D){0}, 10, 0);
        add_components(ball, PARTICLE);
    }
    return smash;
}

void magnetic_sim_tick(Entity smash) {
        if(engine_get_tick() % 1000 == 0) {
            for(int i = 0; i < MAX_ENTITIES; i += 1) {
                if( (entity_mask[i] & JOINT) == JOINT) {
                    if(joints[i].type == JOINT_PIN) {
                        joints[i].type = JOINT_DISTANCE;
                        velocities[smash].x = -50;
                        set_velocity(smash, (Velocity){50,0});
                        set_angular_velocity(smash, 3);
                    } else {
                        joints[i].type = JOINT_PIN;
                        velocities[smash].x = 50;
                        set_velocity(smash, (Velocity){-50, 0});
                        set_angular_velocity(smash, -3);
                    }
                }
            }
        }
}
void water_sim_init() {
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
    set_mass(smash, 500);
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
    for(int i = 0; i < 300; i += 1) {
        Entity ball = add_entity();
        set_position(ball, (Position){.x = tools_random_range(100, 400), .y = tools_random_range(0, 300)});
        set_orientation(ball, tools_random_range(0, 2*pi));
        set_mass(ball, 0.001);
        set_velocity(ball, (Velocity){.x = tools_random_range(-10, 10), .y = tools_random_range(0, 100)});
        set_acceleration(ball, (Acceleration){tools_random_range(0,10), 50});
        set_restitution(ball, 0.1);
        Shape shape3 = create_circle(7, 5);
        set_hitbox(ball, shape3);
        set_friction(ball, 0);
        set_dynamic(ball);
        //set_axis_lock(ball, (Axis){1, 0}, positions[ball]);
    }
}

