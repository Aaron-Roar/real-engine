#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include <stdio.h>

int main() {
    Entity rock = add_entity();
    Entity ball = add_entity();

    set_position(rock, (Position){.x = 0, .y = 0});
    set_position(ball, (Position){.x = 0, .y = 0});
    set_mass(rock, 6);
    set_mass(ball, 5);
    set_force(rock, (Force){.x = 1, .y = -1});
    set_force(ball, (Force){.x = 1, .y = -1});


    //Physics Loop
    while (1) {
        system_clear_accelerations();

        system_apply_forces();
        system_update_velocities();
        system_update_positions();
        delay(1000);

        print_entity_movement(ball);
        print_entity_movement(rock);
    }
}
