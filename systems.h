#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "entity_components.h"
#include "physics.h"


void system_update_positions(double dt);
void system_update_velocities(double dt);
void system_apply_forces();
void system_clear_force_torque_accelerations();
void system_print_entity_movement(Entity entity);
void system_update_orientations(double dt);
void system_update_physics(double dt);
Collision system_get_entity_collision(Entity entity_1, Entity entity_2);
void system_separate_entities(Entity entity_1, Entity entity_2, Collision collision);
void system_resolve_collision(Entity entity_1, Entity entity_2, Collision collision);
void system_apply_collisions();
void system_apply_transform_locks();
void system_clean_entities_past_lifetime();
void system_apply_joints();
#endif
