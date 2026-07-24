#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "entity_components.h"
#include "physics.h"

void system_update_physics(double dt);
void system_clean_entities_past_lifetime();
#endif
