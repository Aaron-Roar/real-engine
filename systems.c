#include "entity_components.h"
#include "systems.h"
void system_update_positions() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        positions[i] = (Vec2D){
            .x = positions[i].x + velocities[i].x,
            .y = positions[i].y + velocities[i].y
        };
    }
}

void system_update_velocities() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        velocities[i] = (Vec2D){
            .x = velocities[i].x + accelerations[i].x,
            .y = velocities[i].y + accelerations[i].y
        };
    }
}

void system_update_accelerations() {
    //Based on force, mass, inertia
}

void system_apply_forces() {

}
