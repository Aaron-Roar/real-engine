#include "entity_components.h"
#include "systems.h"
#include "error.h"
#include "console.h"
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <time.h>

int collision_count = 0;

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
                angular_velocities[i] += (angular_accelerations[i] + torque_angular_accelerations[i]) * dt;
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
                    .x = velocities[i].x + (accelerations[i].x + force_accelerations[i].x)*dt,
                    .y = velocities[i].y + (accelerations[i].y + force_accelerations[i].y)*dt
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
                        force_accelerations[targets[i]].x += forces[i].x/mass[targets[i]];
                        force_accelerations[targets[i]].y += forces[i].y/mass[targets[i]];
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
                        torque_angular_accelerations[targets[i]] += torques[i]/polygon_moment_of_inertia(hit_boxes[targets[i]], mass[targets[i]]);
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

void system_clear_force_torque_accelerations() {
    for(int i = 0; i < MAX_ENTITIES; i++) {
        force_accelerations[i].x = 0;
        force_accelerations[i].y = 0;
        torque_angular_accelerations[i] = 0;
    }
}


Collision system_get_entity_collision(Entity entity_1, Entity entity_2) {
    Shape shape1 = get_global_hit_box(entity_1);
    Shape shape2 = get_global_hit_box(entity_2);
    return sat_collision(shape1, shape2);
}

void separate_entities(Entity entity_1, Entity entity_2, Collision collision)
{
    bool entity_1_dynamic = (entity_mask[entity_1] & DYNAMIC) == DYNAMIC;
    bool entity_2_dynamic = (entity_mask[entity_2] & DYNAMIC) == DYNAMIC;

    float inv_mass_1 = 0.0f;
    float inv_mass_2 = 0.0f;

    if(entity_1_dynamic && mass[entity_1] > 0.0f) {
        inv_mass_1 = 1.0f / mass[entity_1];
    }

    if(entity_2_dynamic && mass[entity_2] > 0.0f) {
        inv_mass_2 = 1.0f / mass[entity_2];
    }

    float inv_mass_sum = inv_mass_1 + inv_mass_2;

    if(inv_mass_sum <= 0.0f) {
        return;
    }

    Vec2D correction = {
        .x = collision.normal.x * collision.depth,
        .y = collision.normal.y * collision.depth
    };

    positions[entity_1].x -= correction.x * (inv_mass_1 / inv_mass_sum);
    positions[entity_1].y -= correction.y * (inv_mass_1 / inv_mass_sum);

    positions[entity_2].x += correction.x * (inv_mass_2 / inv_mass_sum);
    positions[entity_2].y += correction.y * (inv_mass_2 / inv_mass_sum);
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

Position collision_contact_point(Entity entity_1, Entity entity_2, Collision collision)
{
    Shape shape_1 = get_global_hit_box(entity_1);
    Shape shape_2 = get_global_hit_box(entity_2);

    bool entity_1_dynamic = (entity_mask[entity_1] & DYNAMIC) == DYNAMIC;
    bool entity_2_dynamic = (entity_mask[entity_2] & DYNAMIC) == DYNAMIC;

    Vec2D normal = collision.normal;

    Vec2D opposite_normal = {
        .x = -normal.x,
        .y = -normal.y
    };

    Position point_1 = support_point_average(shape_1, normal);
    Position point_2 = support_point_average(shape_2, opposite_normal);

    /*
        If one object is static and one is dynamic,
        use the dynamic object's contact point.

        This prevents a huge static floor from producing
        a fake contact point at the middle of the entire floor.
    */
    if(entity_1_dynamic && !entity_2_dynamic) {
        return point_1;
    }

    if(!entity_1_dynamic && entity_2_dynamic) {
        return point_2;
    }

    /*
        Both dynamic, or both static.
        Midpoint is okay as a temporary approximation.
    */
    return (Position){
        .x = (point_1.x + point_2.x) * 0.5f,
        .y = (point_1.y + point_2.y) * 0.5f
    };
}

void apply_friction_impulse(
    Entity entity_1,
    Entity entity_2,
    Collision collision,
    Vec2D r1,
    Vec2D r2,
    float normal_impulse_magnitude,
    float inv_mass_1,
    float inv_mass_2,
    float inv_inertia_1,
    float inv_inertia_2
) {
    Vec2D angular_v1 = angular_velocity_cross_vec(angular_velocities[entity_1], r1);
    Vec2D angular_v2 = angular_velocity_cross_vec(angular_velocities[entity_2], r2);

    Vec2D contact_v1 = {
        .x = velocities[entity_1].x + angular_v1.x,
        .y = velocities[entity_1].y + angular_v1.y
    };

    Vec2D contact_v2 = {
        .x = velocities[entity_2].x + angular_v2.x,
        .y = velocities[entity_2].y + angular_v2.y
    };

    Vec2D rel_v = {
        .x = contact_v2.x - contact_v1.x,
        .y = contact_v2.y - contact_v1.y
    };

    float rel_v_along_normal = dot_product(rel_v, collision.normal);

    Vec2D tangent = {
        .x = rel_v.x - collision.normal.x * rel_v_along_normal,
        .y = rel_v.y - collision.normal.y * rel_v_along_normal
    };

    float tangent_mag = sqrtf(tangent.x * tangent.x + tangent.y * tangent.y);

    if(tangent_mag <= 0.00001f) {
        return;
    }

    tangent.x /= tangent_mag;
    tangent.y /= tangent_mag;

    float r1_cross_t = cross_2d(r1, tangent);
    float r2_cross_t = cross_2d(r2, tangent);

    float denominator =
        inv_mass_1 +
        inv_mass_2 +
        (r1_cross_t * r1_cross_t) * inv_inertia_1 +
        (r2_cross_t * r2_cross_t) * inv_inertia_2;

    if(denominator <= 0.00001f) {
        return;
    }

    float jt = -dot_product(rel_v, tangent) / denominator;

    float mu = sqrtf(frictions[entity_1] * frictions[entity_2]);

    float max_friction = fabsf(normal_impulse_magnitude) * mu;

    if(jt > max_friction) {
        jt = max_friction;
    }
    else if(jt < -max_friction) {
        jt = -max_friction;
    }

    Vec2D friction_impulse = {
        .x = tangent.x * jt,
        .y = tangent.y * jt
    };

    velocities[entity_1].x -= friction_impulse.x * inv_mass_1;
    velocities[entity_1].y -= friction_impulse.y * inv_mass_1;

    velocities[entity_2].x += friction_impulse.x * inv_mass_2;
    velocities[entity_2].y += friction_impulse.y * inv_mass_2;

    angular_velocities[entity_1] -= cross_2d(r1, friction_impulse) * inv_inertia_1;
    angular_velocities[entity_2] += cross_2d(r2, friction_impulse) * inv_inertia_2;
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
    Position contact = collision_contact_point(entity_1, entity_2, collision);

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

    if(fabsf(v_normal) < 1.0f) {
      restitution = 0.0f;
    }

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

        apply_friction_impulse(
        entity_1,
        entity_2,
        collision,
        r1,
        r2,
        impulse_magnitude,              // pass normal impulse magnitude
        inv_mass_1,
        inv_mass_2,
        inv_inertia_1,
        inv_inertia_2
    );
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
                resolve_collision(i, j, collision);
                separate_entities(i, j, collision);
                collision_count += 1;
                //console_write(LOG_ENGINE, "Entity %d and Entity %d Colided\n", j, i);
            }
        }
    }
}

void system_apply_angle_locks() {
    for(Entity entity = 0; entity < MAX_ENTITIES; entity += 1) {
        if(!entity_alive[entity]) {
            continue;
        }

        if((entity_mask[entity] & ANGLE_LOCK) != ANGLE_LOCK) {
            continue;
        }

        Orientation min = angle_locks[entity].min;
        Orientation max = angle_locks[entity].max;

        if(min > max) {
            Orientation temp = min;
            min = max;
            max = temp;
        }

        /*
            Locked completely.
        */
        if(fabsf(max - min) <= 0.00001f) {
            orientations[entity] = min;
            angular_velocities[entity] = 0.0f;
            angular_accelerations[entity] = 0.0f;
            torque_angular_accelerations[entity] = 0.0f;
            torques[entity] = 0.0f;
            continue;
        }

        /*
            Hit minimum angle.
        */
        if(orientations[entity] < min) {
            orientations[entity] = min;

            if(angular_velocities[entity] < 0.0f) {
                angular_velocities[entity] = 0.0f;
            }

            if(angular_accelerations[entity] < 0.0f) {
                angular_accelerations[entity] = 0.0f;
            }

            if(torque_angular_accelerations[entity] < 0.0f) {
                torque_angular_accelerations[entity] = 0.0f;
            }

            if(torques[entity] < 0.0f) {
                torques[entity] = 0.0f;
            }
        }

        /*
            Hit maximum angle.
        */
        if(orientations[entity] > max) {
            orientations[entity] = max;

            if(angular_velocities[entity] > 0.0f) {
                angular_velocities[entity] = 0.0f;
            }

            if(angular_accelerations[entity] > 0.0f) {
                angular_accelerations[entity] = 0.0f;
            }

            if(torque_angular_accelerations[entity] > 0.0f) {
                torque_angular_accelerations[entity] = 0.0f;
            }

            if(torques[entity] > 0.0f) {
                torques[entity] = 0.0f;
            }
        }
    }
}

void system_apply_axis_locks() {
    for(Entity entity = 0; entity < MAX_ENTITIES; entity += 1) {
        if(!entity_alive[entity]) {
            continue;
        }

        if((entity_mask[entity] & AXIS_LOCK) != AXIS_LOCK) {
            continue;
        }

        Axis axis = axis_locks[entity].axis;

        float mag = axis_magnitude(axis);

        if(mag <= 0.00001f) {
            continue;
        }

        /*
            Safety normalize.
            Ideally this already happened in set_axis_lock(),
            but this prevents bad input from exploding.
        */
        axis.x /= mag;
        axis.y /= mag;

        Position point_on_axis = axis_locks[entity].point_on_axis;

        /*
            1. Lock position onto the line.

            relative = position - point_on_axis
            distance = dot(relative, axis)
            new_position = point_on_axis + axis * distance
        */
        Vec2D relative = {
            .x = positions[entity].x - point_on_axis.x,
            .y = positions[entity].y - point_on_axis.y
        };

        float distance_along_axis = dot_product(relative, axis);

        positions[entity].x = point_on_axis.x + axis.x * distance_along_axis;
        positions[entity].y = point_on_axis.y + axis.y * distance_along_axis;

        /*
            2. Lock velocity onto the axis.
        */
        velocities[entity] = project_onto_axis(velocities[entity], axis);

        /*
            3. Lock acceleration onto the axis.
        */
        accelerations[entity] = project_onto_axis(accelerations[entity], axis);
        force_accelerations[entity] = project_onto_axis(force_accelerations[entity], axis);

        /*
            4. Optional: lock direct force accumulator too.
            This only matters if forces[entity] is used directly.
        */
        forces[entity] = project_onto_axis(forces[entity], axis);
    }
}

void system_update_physics(double dt) {
    system_clear_force_torque_accelerations();

    system_apply_forces();
    system_apply_torques();
    system_update_velocities(dt);
    system_update_angular_velocities(dt);
    system_update_orientations(dt);
    system_update_positions(dt);
    apply_collisions();
    system_apply_axis_locks();
    system_apply_angle_locks();
}

void print_entity_movement(Entity entity) {
    console_write(LOG_ENGINE, "---Movement Log---\n");
    console_write(LOG_ENGINE, "Entity: %d\n", entity);
    console_write(LOG_ENGINE, "Position: {x: %f, y: %f}\n", positions[entity].x, positions[entity].y);
    console_write(LOG_ENGINE, "Velocity: {x: %f, y: %f}\n", velocities[entity].x, velocities[entity].y);
    console_write(LOG_ENGINE, "Acceleration: {x: %f, y: %f}\n", accelerations[entity].x, accelerations[entity].y);
    console_write(LOG_ENGINE, "---Movement Log---\n");
}
