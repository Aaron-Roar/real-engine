#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "entity_components.h"
void system_update_positions();
void system_update_velocities();
void system_update_accelerations();
void system_apply_forces();
void system_clear_accelerations();
void print_entity_movement(Entity e);

//void systems_set_position(Entity e, Position p);
//void system_set_velocity(Entity e, Velocity v);
//void systems_set_acceleration(Entity e, Acceleration a);
//void systems_set_force(Entity e, Force f);
#endif
