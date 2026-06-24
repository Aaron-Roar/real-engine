#include "entity_components.h"
#include "systems.h"
#include "error.h"
#include "console.h"
#include <stdio.h>

void system_update_positions() {
    CMask filter = MOVEABLE;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if( (entity_mask[i] & filter) == filter ) {
                positions[i] = (Position){
                    .x = positions[i].x + velocities[i].x,
                    .y = positions[i].y + velocities[i].y
                };
            }
        }
    }
}

void system_update_velocities() {
    CMask filter = MOVEABLE;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if( (entity_mask[i] & filter) == filter) {
                velocities[i] = (Velocity){
                    .x = velocities[i].x + accelerations[i].x,
                    .y = velocities[i].y + accelerations[i].y
                };
            }
        }
    }
}

//void system_update_accelerations() {
//    //Based on force, mass, inertia
//    EntityList failed_entities = {0};
//
//    CMask filter = MOVEABLE | ACCELERATION | MASS;
//    for (int i = 0; i < MAX_ENTITIES; i++) {
//        if(entity_alive[i]) {
//            if( (entity_mask[i] & filter) == filter) {
//                if(mass[i] == 0) {
//                    failed_entities.concerned_entities[failed_entities.entity_amount] = i;
//                    failed_entities.entity_amount += 1;
//                }
//                else {
//                    accelerations[i] = (Acceleration){
//                        .x = forces[i].x/mass[i],
//                        .y = forces[i].y/mass[i]
//                    };
//                }
//            }
//        }
//    }
//
//    if(failed_entities.entity_amount != 0)
//        error_print(FAILED_UPDATE_ACCELERATION | ACCELERATING_MASSLESS_ENTITY, failed_entities);
//}


void system_apply_forces() {
  Error error = {0};
  CMask filter = FORCE | TARGETABLE;
  CMask target_filter = MOVEABLE | MASS;

  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(entity_alive[i]) { //Check if this entity exists
        if( (entity_mask[i] & filter) == filter ) { //Check if this entity is a targetable force
            if( entity_alive[targets[i]] ) { //Check if the target to the force exists
                if( (entity_mask[targets[i]] & target_filter) == target_filter ) { //Check if the target is moveable
                    if(mass[targets[i]] != 0) {
                        accelerations[targets[i]].x += forces[i].x/mass[targets[i]];
                        accelerations[targets[i]].y += forces[i].y/mass[targets[i]];
                    } else {
                        //Force on massless entity
                        error.code |= ACCELERATING_MASSLESS_ENTITY | FAILED_UPDATE_ACCELERATION;
                        error_add_entity(&error, i);
                        error_add_entity(&error, targets[i]);
                    }
                } else {
                    //Force trying to move entity
                    error.code |= INCOMPATABLE_COMPONENTS | FAILED_UPDATE_ACCELERATION;
                        error_add_entity(&error, i);
                        error_add_entity(&error, targets[i]);
                }
            }
            else {
                //Forces exist without targets
            }
        }
    }
  }
  error_print(error);
}

void system_clear_accelerations() {
    for(int i = 0; i < MAX_ENTITIES; i++) {
        accelerations[i].x = 0;
        accelerations[i].y = 0;
    }
}


void print_entity_movement(Entity e) {
    console_write(LOG_ENGINE, "---Movement Log---\n");
    console_write(LOG_ENGINE, "Entity: %d\n", e);
    console_write(LOG_ENGINE, "Position: {x: %f, y: %f}\n", positions[e].x, positions[e].y);
    console_write(LOG_ENGINE, "Velocity: {x: %f, y: %f}\n", velocities[e].x, velocities[e].y);
    console_write(LOG_ENGINE, "Acceleration: {x: %f, y: %f}\n", accelerations[e].x, accelerations[e].y);
    console_write(LOG_ENGINE, "---Movement Log---\n");
}
