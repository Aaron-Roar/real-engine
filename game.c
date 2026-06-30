#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include "graphics.h"
#include "console.h"
#include "engine.h"
#include <stdio.h>
#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>

float pi = 3.14;
int main() {
    console_init();
    engine_init();
    graphics_start();

    //Game setup
    //
    time_t start_time = tools_get_time();
    double prev_time = 0;
    double current_time = 0;

    Entity rock = add_entity();
    set_position(rock, (Position){.x = 0, .y = 0});
    set_mass(rock, 1000);
    //set_force(rock, (Force){.x = .1, .y = .1});
    Shape box = create_square(10, 10);
    set_hitbox(rock, box);
    //
    //

    //float i = 0;
    //Game Loop
    while (console_is_active()) {

        //Console
        //
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);

        }
        //parse console cmd
        //execite console cmd
        //refresh();
        //


        //physics
        //
        prev_time = current_time;
        current_time = tools_get_time() - start_time;
        double dt = current_time - prev_time;
        system_clear_accelerations();
        system_apply_forces();
        system_update_velocities(dt);
        system_update_positions(dt);

        //App
        //
        //console_write(LOG_APP, "{%f, %f}\n", positions[rock].x, positions[rock].y);
        float base_x = 200.0f;
        float amplitude = 50.0f;
        float frequency = 2.0f; // cycles per second
        float speed_down = 80.0f;
        float omega = 2.0f * pi * frequency;
        float x = base_x + amplitude * sinf(omega * current_time);
        float y = positions[rock].y + speed_down * dt;
        set_position(rock,(Position){x,y});
        //

        //render
        //
        graphics_poll_events();
        draw_background();
        draw_rect(hit_boxes[rock], positions[rock]);
        show_graphics();
        //

    }
    graphics_end();
    engine_shutdown();
}

