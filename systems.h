#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "entity_components.h"

void system_update_positions(double dt);
void system_update_velocities(double dt);
void system_apply_forces();
void system_clear_accelerations();
void print_entity_movement(Entity e);
void system_update_orientations(double dt);
void system_update_physics(double dt);
Collision system_get_entity_collision(Entity e1, Entity e2);
#endif
