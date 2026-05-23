#include "entity_components.h"
#include "systems.h"
#include "stdio.h"
void system_update_positions() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if(entity_mask[i] & (MOVEABLE | POSITION) ) {
                positions[i] = (Position){
                    .x = positions[i].x + velocities[i].x,
                    .y = positions[i].y + velocities[i].y
                };
            }
        }
    }
}

void system_update_velocities() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if(entity_mask[i] & (MOVEABLE | VELOCITY)) {
                velocities[i] = (Velocity){
                    .x = velocities[i].x + accelerations[i].x,
                    .y = velocities[i].y + accelerations[i].y
                };
            }
        }
    }
}

void system_update_accelerations() {
    //Based on force, mass, inertia
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            CMask filter = MOVEABLE | ACCELERATION | MASS;
            if( (entity_mask[i] & (filter)) == filter) {
                if(mass[i] == 0) {
                    print_error(FAILED_UPDATE_ACCELERATION | ACCELERATING_MASSLESS_ENTITY);
                }
                accelerations[i] = (Acceleration){
                    .x = forces[i].x/mass[i],
                    .y = forces[i].y/mass[i]
                };
                printf("Applying accelerations to %d\n", i);
            }
        }
    }
}

void system_apply_forces() {
}

void print_entity_movement(Entity e) {
    char string[] =
        "Position {x: %f, y: %f}\n"
        "Velocity: {x: %f, y: %f}\n"
        "Acceleration: {x: %f, y: %f}\n"
        "------------------------------\n";
    printf(string, positions[e].x, positions[e].y, velocities[e].x, velocities[e].y, accelerations[e].x, accelerations[e].y);
}
