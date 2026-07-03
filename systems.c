#include "entity_components.h"
#include "systems.h"
#include "error.h"
#include "console.h"
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <time.h>


void system_update_positions(double dt) {
    CMask filter = DYNAMIC;
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
    CMask filter = DYNAMIC;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if( (entity_mask[i] & filter) == filter ) {
                orientations[i] = orientations[i] + angular_velocities[i]*dt;
            }
        }
    }
}


void system_update_angular_velocities(double dt) {
    CMask filter = DYNAMIC;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            if( (entity_mask[i] & filter) == filter) {
                angular_velocities[i] += (AngularVelocity)angular_accelerations[i]*dt;
            }
        }
    }
}

void system_update_velocities(double dt) {
    CMask filter = DYNAMIC;
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
  CMask target_filter = DYNAMIC | MASS;

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
  CMask target_filter = DYNAMIC | MASS;

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
        angular_accelerations[i] = 0;
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

Position support_point_average(Shape shape, Vec2D direction)
{
    float best_projection = -FLT_MAX;
    Position sum = {0};
    int count = 0;

    const float epsilon = 0.0001f;

    for(int i = 0; i < shape.amount_of_vertices; i += 1) {
        Position vertex = shape.vertices[i];
        float projection = dot_product(vertex, direction);

        if(projection > best_projection + epsilon) {
            best_projection = projection;
            sum = vertex;
            count = 1;
        }
        else if(fabsf(projection - best_projection) <= epsilon) {
            sum.x += vertex.x;
            sum.y += vertex.y;
            count += 1;
        }
    }

    if(count > 0) {
        sum.x /= count;
        sum.y /= count;
    }

    return sum;
}

Position approximate_contact_point_from_normal(Shape shape_1, Shape shape_2, Vec2D normal)
{
    Vec2D opposite_normal = {
        .x = -normal.x,
        .y = -normal.y
    };

    Position point_1 = support_point_average(shape_1, normal);
    Position point_2 = support_point_average(shape_2, opposite_normal);

    return (Position){
        .x = (point_1.x + point_2.x) * 0.5f,
        .y = (point_1.y + point_2.y) * 0.5f
    };
}

void resolve_collision(Entity entity_1, Entity entity_2, Collision collision) {
    // Assume collision.normal points from entity_1 -> entity_2

    // ================================
    // CHANGED: use your MOVABLE bitmask
    // ================================
    bool entity_1_movable =
        ((entity_mask[entity_1] & DYNAMIC) == DYNAMIC);

    bool entity_2_movable =
        ((entity_mask[entity_2] & DYNAMIC) == DYNAMIC);

    // ================================
    // CHANGED: local inverse mass from your mass[] array
    // If not movable, inverse mass = 0.
    // ================================
    float inv_mass_1 = 0.0f;
    float inv_mass_2 = 0.0f;

    if (entity_1_movable && mass[entity_1] > 0.0f) {
        inv_mass_1 = 1.0f / mass[entity_1];
    }

    if (entity_2_movable && mass[entity_2] > 0.0f) {
        inv_mass_2 = 1.0f / mass[entity_2];
    }

    // If neither body can move, no velocity response is needed.
    if (inv_mass_1 + inv_mass_2 <= 0.0f) {
        return;
    }

    //Position contact = approximate_contact_point(positions[entity_1], positions[entity_2]);
    Shape world_shape_1 = get_global_hit_box(entity_1);
    Shape world_shape_2 = get_global_hit_box(entity_2);

    Position contact = approximate_contact_point_from_normal(
        world_shape_1,
        world_shape_2,
        collision.normal
    );

    Vec2D r1 = {
        .x = contact.x - positions[entity_1].x,
        .y = contact.y - positions[entity_1].y
    };

    Vec2D r2 = {
        .x = contact.x - positions[entity_2].x,
        .y = contact.y - positions[entity_2].y
    };

    // ================================
    // CHANGED: static/non-movable objects should not contribute contact velocity
    // for now. Later you may add kinematic objects.
    // ================================
    Vec2D rotational_velocity_1 = {0};
    Vec2D rotational_velocity_2 = {0};

    if (entity_1_movable) {
        rotational_velocity_1 =
            angular_velocity_cross_vec(angular_velocities[entity_1], r1);
    }

    if (entity_2_movable) {
        rotational_velocity_2 =
            angular_velocity_cross_vec(angular_velocities[entity_2], r2);
    }

    Vec2D contact_velocity_1 = {0};
    Vec2D contact_velocity_2 = {0};

    if (entity_1_movable) {
        contact_velocity_1.x = velocities[entity_1].x + rotational_velocity_1.x;
        contact_velocity_1.y = velocities[entity_1].y + rotational_velocity_1.y;
    }

    if (entity_2_movable) {
        contact_velocity_2.x = velocities[entity_2].x + rotational_velocity_2.x;
        contact_velocity_2.y = velocities[entity_2].y + rotational_velocity_2.y;
    }

    Vec2D v_rel = {
        .x = contact_velocity_2.x - contact_velocity_1.x,
        .y = contact_velocity_2.y - contact_velocity_1.y
    };

    float v_normal = dot_product(v_rel, collision.normal);

    if (v_normal > 0.0f) {
        return;
    }

    float restitution = fminf(restitutions[entity_1], restitutions[entity_2]);

    // ================================
    // CHANGED: local inverse inertia
    // Non-movable objects get inverse inertia = 0.
    // ================================
    float inv_inertia_1 = 0.0f;
    float inv_inertia_2 = 0.0f;

    if (entity_1_movable) {
        float inertia_1 =
            polygon_moment_of_inertia(hit_boxes[entity_1], mass[entity_1]);

        if (inertia_1 > 0.0f) {
            inv_inertia_1 = 1.0f / inertia_1;
        }
    }

    if (entity_2_movable) {
        float inertia_2 =
            polygon_moment_of_inertia(hit_boxes[entity_2], mass[entity_2]);

        if (inertia_2 > 0.0f) {
            inv_inertia_2 = 1.0f / inertia_2;
        }
    }

    float r1_cross_n = cross_2d(r1, collision.normal);
    float r2_cross_n = cross_2d(r2, collision.normal);

    // ================================
    // CHANGED: denominator uses inverse mass/inertia
    // ================================
    float denominator =
        inv_mass_1 +
        inv_mass_2 +
        (r1_cross_n * r1_cross_n) * inv_inertia_1 +
        (r2_cross_n * r2_cross_n) * inv_inertia_2;

    if (denominator <= 0.0f) {
        return;
    }

    float impulse_magnitude =
        (-(1.0f + restitution) * v_normal) / denominator;

    Vec2D impulse = {
        .x = collision.normal.x * impulse_magnitude,
        .y = collision.normal.y * impulse_magnitude
    };

    // ================================
    // CHANGED: velocity update uses inverse mass
    // If entity is not movable, inv_mass = 0, so it does not change.
    // ================================
    velocities[entity_1].x -= impulse.x * inv_mass_1;
    velocities[entity_1].y -= impulse.y * inv_mass_1;

    velocities[entity_2].x += impulse.x * inv_mass_2;
    velocities[entity_2].y += impulse.y * inv_mass_2;

    // ================================
    // CHANGED: angular velocity update uses inverse inertia
    // If entity is not movable, inv_inertia = 0, so it does not rotate.
    // ================================
    angular_velocities[entity_1] -= cross_2d(r1, impulse) * inv_inertia_1;
    angular_velocities[entity_2] += cross_2d(r2, impulse) * inv_inertia_2;
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

        for(int j = i + 1; j < MAX_ENTITIES; j += 1) {
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
                separate_entities(i, j, collision);
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
