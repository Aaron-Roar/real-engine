#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include "graphics.h"
#include "console.h"
#include <stdio.h>

int main() {
    console_write(APP, "%s", console_legend);
    graphics_start();
    Entity rock = add_entity();
    Entity ball = add_entity();

    set_position(rock, (Position){.x = 0, .y = 0});
    set_position(ball, (Position){.x = 0, .y = 0});
    set_mass(rock, 6);
    //set_mass(ball, 4);
    set_force(rock, (Force){.x = 1, .y = -1});
    set_force(ball, (Force){.x = 1, .y = -1});

    graphics_end();

    //Physics Loop
    while (1) {
        console_write(APP, "Hello from inside the game!!\n");
        system_clear_accelerations();

        system_apply_forces();
        system_update_velocities();
        system_update_positions();
        delay(1000);

        print_entity_movement(ball);
        //print_entity_movement(rock);
    }
}
