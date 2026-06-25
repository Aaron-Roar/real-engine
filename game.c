#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include "graphics.h"
#include "console.h"
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

void print_shape(Shape shape) {
        console_write(LOG_APP, "Shape {X, Y}:\n");
    for(int i = 0; i < shape.vertex_amount; i++) {
        printf("(%f, %f),", shape.vertices[i].x, shape.vertices[i].y);
    }
}

int main() {
    console_init();
    graphics_start();
    graphics_event_listener_start();



    Entity rock = add_entity();
    set_position(rock, (Position){.x = 0, .y = 0});
    set_mass(rock, 6);
    set_force(rock, (Force){.x = 1, .y = -1});
    set_hitbox(rock, circle(10, 200));

    //Physics Loop
    int i = 0;
    while (1) {
        system_clear_accelerations();
        //console_write(LOG_APP, "Game Tick!\n");

        ConsoleInput console_line = {0};
        if(read_console(console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line);
            //log_input(console_line);
            //parse console cmd
            //execite console cmd
        }
        //refresh();


        system_apply_forces();
        system_update_velocities();
        system_update_positions();
        //print_entity_movement(rock);

        draw_background();
        draw_rect();
        show_graphics();
    }
    graphics_end();
}

