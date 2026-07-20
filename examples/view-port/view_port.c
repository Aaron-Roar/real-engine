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
#include "controller.h"
#include "examples/test-assets/elder-fly/elderfly_descriptors.h"
#include "examples/test-assets/orm/orm_descriptors.h"

const Color background_color = (Color){255,255,255,255};
AnimationAsset animation_orm = {0};
AnimatedSprite sprite_orm = {0};
int main() {
    console_init();
    console_set_debug(CONSOLE_DEBUG_OFF);
    engine_init();
    KeyboardState keyboard = {0};
    //level_editor_init();
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    if (!graphics_start(&renderer, &window)) {
        engine_shutdown();
        return 1;
    }

    Entity water_smash = entity_add();
    physics_set_position(water_smash, (Position){.x = 0, .y = 0});
    physics_set_orientation(water_smash, 0);
    physics_set_mass(water_smash, 50);
    physics_set_velocity(water_smash, (Velocity){0, 0});
    physics_set_acceleration(water_smash, (Acceleration){0, -30});
    physics_set_restitution(water_smash, 0.1);
    Shape shape4 = math_create_square(150, 220);
    physics_set_hitbox(water_smash, shape4);
    physics_set_friction(water_smash, 0.4);
    physics_set_static(water_smash);
    animation_orm = graphics_load_animation(renderer, orm_files);
        sprite_orm = graphics_create_animated_sprite(animation_orm, (Scale){10,10});
        graphics_add_animated_sprite(water_smash, sprite_orm);

    engine_reset_clock();
    Time dt = engine_get_dt();
    //Game Loop
    //graphics_recording_start("examples/view-port/recording.mp4",60);
    while (console_is_active()) {
        system_clean_entities_past_lifetime();

        //Console
        ConsoleLogString console_line = {0};
        if(console_read(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }
        //level_editor_update(renderer);

        //physics
        engine_update_time();
        engine_update_tick();
        system_update_physics(engine_get_dt());

        //render
        graphics_draw_background(renderer, background_color);
        graphics_update_sprite_frames(engine_get_tick(), engine_get_time());
        graphics_draw_animated_sprites(renderer);
        graphics_show(renderer);

        SDL_Event sdl_event = engine_poll_event();
        KeyboardEvent key_event = capture_keyboard_event(&sdl_event);
        add_key_event(&keyboard, key_event);
        update_key_states(&keyboard);
        print_keyboard_event(key_event);
        
    }
    graphics_end(renderer, window);
    engine_shutdown();
}


