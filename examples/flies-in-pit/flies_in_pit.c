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
#include "examples/test-assets/orm/orm_descriptors.h"

const Color background_color = (Color){255,255,255,255};
AnimationAsset animation_elderfly = {0};
AnimatedSprite sprite_elderfly = {0};
AnimationAsset animation_orm = {0};
AnimatedSprite sprite_orm = {0};
#define amount_of_entities 500

int main() {
    console_init();
    console_set_debug(CONSOLE_DEBUG_OFF);
    engine_init();
    engine_set_dt(1/(float)120);
    level_editor_init();
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    if (!graphics_start(&renderer, &window)) {
        engine_shutdown();
        return 1;
    }

    Entity water_wall_1 = entity_add();
    physics_set_static(water_wall_1);
    physics_set_position(water_wall_1, (Position){0, -80});
    physics_set_orientation(water_wall_1, 120*(PI_F/180));
    physics_set_restitution(water_wall_1, 0.5);
    physics_set_friction(water_wall_1, 1);
    Shape shape_1 = math_create_square(40, 400);
    physics_set_hitbox(water_wall_1, shape_1);

    Entity water_wall_2 = entity_add();
    physics_set_static(water_wall_2);
    physics_set_position(water_wall_2, (Position){-180, -10});
    physics_set_orientation(water_wall_2, 0*(PI_F/180));
    physics_set_restitution(water_wall_2, 0.5);
    physics_set_friction(water_wall_2, 1);
    Shape shape_2 = math_create_square(40, 60);
    physics_set_hitbox(water_wall_2, shape_2);

    Entity water_wall_3 = entity_add();
    physics_set_static(water_wall_3);
    physics_set_position(water_wall_3, (Position){180, -10});
    physics_set_orientation(water_wall_3, 0*(PI_F/180));
    physics_set_restitution(water_wall_3, 0.5);
    physics_set_friction(water_wall_3, 1);
    Shape shape_3 = math_create_square(40, 60);
    physics_set_hitbox(water_wall_3, shape_3);

    Entity water_smash = entity_add();
    physics_set_position(water_smash, (Position){.x = 0, .y = 300});
    physics_set_orientation(water_smash, 0);
    physics_set_mass(water_smash, 50);
    physics_set_velocity(water_smash, (Velocity){0, 0});
    physics_set_acceleration(water_smash, (Acceleration){0, -30});
    physics_set_restitution(water_smash, 0.1);
    Shape shape4 = math_create_square(150, 220);
    physics_set_hitbox(water_smash, shape4);
    physics_set_friction(water_smash, 0.4);
    physics_set_dynamic(water_smash);
    animation_orm = graphics_load_animation(renderer, orm_files);
        sprite_orm = graphics_create_animated_sprite(animation_orm, (Scale){10,10});
        graphics_add_animated_sprite(water_smash, sprite_orm);
    //set_axis_lock(water_smash, (Axis){0,1}, positions[smash]);

    animation_elderfly = graphics_load_animation(renderer, elderfly_fly_files);
    time_t seed = 1003463;
    srand(seed);
    for(int i = 0; i < amount_of_entities - 1; i += 1) {
        Entity ball = entity_add();
        physics_set_position(ball, (Position){.x = tools_random_range(-10, 10), .y = tools_random_range(100, 200)});
        physics_set_orientation(ball, tools_random_range(0, 2*PI_F));
        physics_set_mass(ball, 1);
        physics_set_velocity(ball, (Velocity){0, .y = tools_random_range(0, -10)});
        physics_set_acceleration(ball, (Acceleration){0, -50});
        physics_set_restitution(ball, 0.2);
        float size = tools_random_range_float(3, 5);
        Shape shape3 = math_create_circle(size, 3);
        physics_set_hitbox(ball, shape3);
        physics_set_friction(ball, 0.4);
        physics_set_dynamic(ball);
        sprite_elderfly = graphics_create_animated_sprite(animation_elderfly, (Scale){size/10,size/10});
        sprite_elderfly.animation.time_per_frame = tools_random_range_float(0.005, 0.5);
        graphics_add_animated_sprite(ball, sprite_elderfly);
        entity_add_components(ball, PARTICLE);
    }

    engine_reset_clock();
    Time dt = engine_get_dt();
    //Game Loop
    //graphics_recording_start("examples/flies-in-pit/recording.mp4",60);
    while (console_is_active()) {
        system_clean_entities_past_lifetime();

        //Console
        ConsoleLogString console_line = {0};
        if(console_read(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }
        level_editor_update(renderer);

        //physics
        engine_update_time();
        engine_update_tick();
        system_update_physics(engine_get_dt());

        //render
        graphics_draw_background(renderer, background_color);
        graphics_draw_hit_box(renderer, water_wall_1, GRAPHICS_FILLED);
        graphics_draw_hit_box(renderer, water_wall_2, GRAPHICS_FILLED);
        graphics_draw_hit_box(renderer, water_wall_3, GRAPHICS_FILLED);
        graphics_update_sprite_frames(engine_get_tick(), engine_get_time());
        graphics_draw_animated_sprites(renderer);
        graphics_draw_hit_boxes(renderer);
        graphics_draw_particles(renderer);
        graphics_draw_grid(renderer);
        graphics_show(renderer);

    }
    graphics_end(renderer, window);
    engine_shutdown();
}

