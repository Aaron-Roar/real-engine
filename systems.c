#include "entity_components.h"
#include "systems.h"
#include "error.h"
#include "console.h"
#include <stdio.h>
#include <time.h>


void system_update_positions(double dt) {
    CMask filter = MOVEABLE;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if( (entity_mask[i] & filter) == filter ) {
                positions[i] = (Position){
                    .x = positions[i].x + (velocities[i].x)*dt,
                    .y = positions[i].y + (velocities[i].y)*dt
                };
            }
        }
    }
}

void system_update_orientations(double dt) {
    CMask filter = MOVEABLE;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if( (entity_mask[i] & filter) == filter ) {
                orientations[i] = orientations[i] + angular_velocities[i]*dt;
            }
        }
    }
}


void system_update_angular_velocities(double dt) {
    CMask filter = MOVEABLE;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if( (entity_mask[i] & filter) == filter) {
                //Apply based on torque force
                //angular_velocities[i] = (AngularVelocity)5;
            }
        }
    }
}

void system_update_velocities(double dt) {
    CMask filter = MOVEABLE;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if( (entity_mask[i] & filter) == filter) {
                velocities[i] = (Velocity){
                    .x = velocities[i].x + (accelerations[i].x)*dt,
                    .y = velocities[i].y + (accelerations[i].y)*dt
                };
            }
        }
    }
}

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

void system_apply_torques() {
    //Apply force offset from centroid and torque applied directly
}

void system_clear_accelerations() {
    for(int i = 0; i < MAX_ENTITIES; i++) {
        accelerations[i].x = 0;
        accelerations[i].y = 0;
    }
}

void system_update_physics(double dt) {
    system_clear_accelerations();

    system_apply_forces();
    system_apply_torques();
    system_update_velocities(dt);
    system_update_angular_velocities(dt);
    system_update_orientations(dt);
    system_update_positions(dt);
}


void print_entity_movement(Entity e) {
    console_write(LOG_ENGINE, "---Movement Log---\n");
    console_write(LOG_ENGINE, "Entity: %d\n", e);
    console_write(LOG_ENGINE, "Position: {x: %f, y: %f}\n", positions[e].x, positions[e].y);
    console_write(LOG_ENGINE, "Velocity: {x: %f, y: %f}\n", velocities[e].x, velocities[e].y);
    console_write(LOG_ENGINE, "Acceleration: {x: %f, y: %f}\n", accelerations[e].x, accelerations[e].y);
    console_write(LOG_ENGINE, "---Movement Log---\n");
}
