#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include <stdio.h>

int main() {
    Entity ball = add_entity();
    positions[ball]     = (Vec2D){.x = 0, .y = 0};
    velocities[ball]    = (Vec2D){.x = 0, .y = 0};
    accelerations[ball] = (Vec2D){.x = 0, .y = -9.8};
    printf("Ball: %d\n", ball);

    //Physics Loop
    while (1) {
        apply_forces();
        update_accelerations();
        update_velocities();
        update_positions();
        delay(1000);
        printf("Acceleration: %f\n Velocities: %f\n Positions: %f\n", accelerations[ball].y, velocities[ball].y, positions[ball].y);
    }
}
