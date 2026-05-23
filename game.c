#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include <stdio.h>

int main() {
    Entity rock = add_entity();
    Entity ball = add_entity();
    set_position(rock, (Position){.x = 0, .y = 0});
    set_position(ball, (Position){.x = 0, .y = 0});
    set_mass(rock, 0);
    set_mass(ball, 0);
    set_force(rock, (Force){.x = 1, .y = -1});
    set_force(ball, (Force){.x = 1, .y = -1});


    //Physics Loop
   while (1) {
    printf("Entity: %d\n", rock);
    printf("Entity: %d\n", ball);
       system_apply_forces();
       system_update_accelerations();
       system_update_velocities();
       system_update_positions();
       delay(1000);
   }
}
