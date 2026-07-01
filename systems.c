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
                angular_velocities[i] = (AngularVelocity)angular_accelerations[i]*dt;
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
  Error error = {0};
  CMask filter = TORQUE | TARGETABLE;
  CMask target_filter = MOVEABLE | MASS;

  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(entity_alive[i]) { //Check if this entity exists
        if( (entity_mask[i] & filter) == filter ) { //Check if this entity is a targetable force
            if( entity_alive[targets[i]] ) { //Check if the target to the force exists
                if( (entity_mask[targets[i]] & target_filter) == target_filter ) { //Check if the target is moveable
                    if(mass[targets[i]] != 0) {
                        angular_accelerations[targets[i]] += torques[i]/polygon_moment_of_inertia(hit_boxes[targets[i]], mass[targets[i]]);
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

void system_update_physics(double dt) {
    system_clear_accelerations();

    system_apply_forces();
    system_apply_torques();
    system_update_velocities(dt);
    system_update_angular_velocities(dt);
    system_update_orientations(dt);
    system_update_positions(dt);
}

Collision system_get_entity_collision(Entity entity_1, Entity entity_2) {
    Shape shape1 = get_global_hit_box(entity_1);
    Shape shape2 = get_global_hit_box(entity_2);
    return sat_collision(shape1, shape2);
}


void print_entity_movement(Entity entity) {
    console_write(LOG_ENGINE, "---Movement Log---\n");
    console_write(LOG_ENGINE, "Entity: %d\n", entity);
    console_write(LOG_ENGINE, "Position: {x: %f, y: %f}\n", positions[entity].x, positions[entity].y);
    console_write(LOG_ENGINE, "Velocity: {x: %f, y: %f}\n", velocities[entity].x, velocities[entity].y);
    console_write(LOG_ENGINE, "Acceleration: {x: %f, y: %f}\n", accelerations[entity].x, accelerations[entity].y);
    console_write(LOG_ENGINE, "---Movement Log---\n");
}
