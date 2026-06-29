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
    engine_init();
    console_init();
    graphics_start();

    //Game setup
    //
    time_t start_time = time(NULL);
    double prev_time = 0;
    double current_time = 0;

    Entity rock = add_entity();
    set_position(rock, (Position){.x = 0, .y = 0});
    set_mass(rock, 1000);
    //set_force(rock, (Force){.x = .1, .y = .1});
    set_velocity(rock, (Velocity){5,5});
    Shape box = create_square(10, 10);
    set_hitbox(rock, box);
    //
    //

    //float i = 0;
    //Game Loop
    while (1) {
        //console_write(LOG_APP, "Game Tick!\n");

        //Input (Console + IO Devices)
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
            //log_input(console_line);
            //parse console cmd
            //execite console cmd
        }
            //refresh();


        //physics
        prev_time = current_time;
        current_time = tools_get_currenttime(start_time);
        double dt = current_time - prev_time;
        console_write(LOG_APP, "Time:%f\n", tools_get_currenttime(start_time));
        system_clear_accelerations();
        system_apply_forces();
        system_update_velocities(dt);
        system_update_positions(dt);
        //print_entity_movement(rock);

        //render
        graphics_poll_events();
        draw_background();
        draw_rect(hit_boxes[rock], positions[rock]);
        show_graphics();
    }
    graphics_end();
    engine_shutdown();
}

