#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "entity_components.h"

void system_update_positions(double dt);
void system_update_velocities(double dt);
void system_apply_forces();
void system_clear_accelerations();
void print_entity_movement(Entity entity);
void system_update_orientations(double dt);
void system_update_physics(double dt);
Collision system_get_entity_collision(Entity entity_1, Entity entity_2);
void separate_entities(Entity entity_1, Entity entity_2, Collision collision);
void resolve_collision(Entity entity_1, Entity entity_2, Collision collision);
void apply_collisions();
#endif
