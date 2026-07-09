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
#include "test-assets/elder-fly/elderfly_descriptors.h"
#include "examples.h"

const Color background_color = (Color){255,255,255,255};

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



    water_sim_init(renderer);
    //magnetic_sim_init(renderer);
    //Game Loop
    Time dt = engine_get_dt();
    while (console_is_active()) {
        clean_entities_past_lifetime();
        //Console
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }

        //physics
        engine_update_time();
        engine_update_tick();
        apply_collisions();
        system_update_physics(engine_get_dt());
        SDL_Event event = engine_poll_event();

        if (event.type == SDL_EVENT_KEY_UP && event.key.scancode == SDL_SCANCODE_SPACE) {
            // move up
            //engine_pause();
            engine_set_dt(-dt);
            for(int i = 0; i < MAX_ENTITIES; i += 1) {
                accelerations[i].x = -accelerations[i].x;
                accelerations[i].y = -accelerations[i].y;
            }
        }

        //render
        draw_background(renderer, background_color);
        //draw_hit_boxes(renderer);
        update_sprite_frames(engine_get_tick(), engine_get_time());
        draw_animated_sprites(renderer);
        water_sim_tick(renderer);
        //magnetic_sim_tick(renderer);
        show_graphics(renderer);

    }
    graphics_end(renderer, window);
    engine_shutdown();
}

