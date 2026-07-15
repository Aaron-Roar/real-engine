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
int main() {
    console_init();
    console_set_debug(CONSOLE_DEBUG_OFF);
    engine_init();
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    if (!graphics_start(&renderer, &window)) {
        engine_shutdown();
        return 1;
    }
    level_editor_init();


    Entity entity_1 = add_entity();
    set_static(entity_1);
    set_position(entity_1, (Vec2D){80, 390});
    set_orientation(entity_1, 0*(PI_F/180));
    Shape shape_1 = math_create_square(40, 150);
    set_hitbox(entity_1, shape_1);

    Entity entity_2 = add_entity();
    set_static(entity_2);
    set_position(entity_2, (Vec2D){100, 390});
    set_orientation(entity_2, 0*(PI_F/180));
    Shape shape_2 = math_create_circle(20, 4);
    set_hitbox(entity_2, shape_2);

    //Game Loop
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
        //apply_collisions();
        system_update_physics(engine_get_dt());

        graphics_draw_background(renderer, background_color);

        graphics_draw_hit_boxes(renderer);
        graphics_update_sprite_frames(engine_get_tick(), engine_get_time());
        graphics_draw_animated_sprites(renderer);
        graphics_show(renderer);

    }
    graphics_end(renderer, window);
    engine_shutdown();
}

