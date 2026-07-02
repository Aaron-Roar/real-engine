#include "entity_components.h"
#include "systems.h"
#include "error.h"
#include "console.h"
#include <math.h>
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


Collision system_get_entity_collision(Entity entity_1, Entity entity_2) {
    Shape shape1 = get_global_hit_box(entity_1);
    Shape shape2 = get_global_hit_box(entity_2);
    return sat_collision(shape1, shape2);
}

void separate_entities(Entity entity_1, Entity entity_2, Collision collision)
{
    if ((mass[entity_1] + mass[entity_2]) <= 0.0f) {
        //Then no mass!
        return;
    }

    Vec2D correction = {
        .x = collision.normal.x * collision.depth,
        .y = collision.normal.y * collision.depth
    };

    positions[entity_1].x -= correction.x * mass[entity_2]/(mass[entity_1] + mass[entity_2]);
    positions[entity_1].y -= correction.y * mass[entity_2]/(mass[entity_1] + mass[entity_2]);

    positions[entity_2].x += correction.x * mass[entity_1]/(mass[entity_1] + mass[entity_2]);
    positions[entity_2].y += correction.y * mass[entity_1]/(mass[entity_1] + mass[entity_2]);
}

void resolve_collision(Entity entity_1, Entity entity_2, Collision collision) {
    separate_entities(entity_1, entity_2, collision);
    //Assume collision points from E1->E2 
    Velocity v_rel = {
        .x = velocities[entity_2].x - velocities[entity_1].x,
        .y = velocities[entity_2].y - velocities[entity_1].y,
    };

    Vec1D v_normal = dot_product(v_rel, collision.normal);

    if(v_normal > 0) {
        //No collision objects moving away from eachother
        return;
    }

    Vec1D restitution = fminf(restitutions[entity_1], restitutions[entity_2]);
    Vec1D impulse_magnitude = (-(1 + restitution)*v_normal) / ((1/mass[entity_1]) + (1/mass[entity_2]));

    Vec2D impulse = {
        .x = collision.normal.x * impulse_magnitude,
        .y = collision.normal.y * impulse_magnitude
    };

    velocities[entity_1].x -= impulse.x / mass[entity_1];
    velocities[entity_1].y -= impulse.y / mass[entity_1];
    velocities[entity_2].x += impulse.x / mass[entity_2];
    velocities[entity_2].y += impulse.y / mass[entity_2];
}

void apply_collisions() {
    CMask filter = COLLISION;
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(!entity_alive[i]) {
            continue;
        }
        if( (entity_mask[i] & filter) != filter) {
            continue;
        }

        for(int j = 0; j < MAX_ENTITIES; j += 1) {
            if(!entity_alive[j]) {
                continue;
            }
            if( (entity_mask[j] & filter) != filter) {
                continue;
            }
            if(i == j) {
                continue;
            }

            Collision collision = system_get_entity_collision(i, j);
            if(collision.overlap == true) {
                resolve_collision(i, j, collision);
            }
        }
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
    apply_collisions();
}

void print_entity_movement(Entity entity) {
    console_write(LOG_ENGINE, "---Movement Log---\n");
    console_write(LOG_ENGINE, "Entity: %d\n", entity);
    console_write(LOG_ENGINE, "Position: {x: %f, y: %f}\n", positions[entity].x, positions[entity].y);
    console_write(LOG_ENGINE, "Velocity: {x: %f, y: %f}\n", velocities[entity].x, velocities[entity].y);
    console_write(LOG_ENGINE, "Acceleration: {x: %f, y: %f}\n", accelerations[entity].x, accelerations[entity].y);
    console_write(LOG_ENGINE, "---Movement Log---\n");
}
