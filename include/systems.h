#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "entity_components.h"
#include "physics.h"

/**
 * Run the physics pipeline for one frame.
 *
 * The current pipeline applies forces and joints, integrates movement,
 * applies locks, updates hitboxes/AABBs, then resolves collisions.
 *
 * @param dt Delta time in seconds.
 */
void system_update_physics(double dt);

/**
 * Delete entities whose lifetime has expired.
 */
void system_clean_entities_past_lifetime(void);
#endif
