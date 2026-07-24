#include "entity_components.h"
#include "systems.h"
#include "console.h"
#include "grid.h"
#include <math2d.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <time.h>
#include "grid.h"

void system_generate_global_hitboxes() {
    CMask filter = HIT_BOX;

    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(entity_is_alive(i)) {
            if( (entity_mask[i] & filter) == filter ) {
                Position pos = positions[i];
                Orientation ort = orientations[i];
                Shape hit_box = hit_boxes[i];

                world_hit_boxes[i] = physics_shape_world_translate(hit_box, pos, ort);
            }
        }

    }
}

Shape system_generate_global_hitbox(Entity entity) {
    CMask filter = HIT_BOX;
        if(entity_is_alive(entity)) {
            if( (entity_mask[entity] & filter) == filter ) {
                Position pos = positions[entity];
                Orientation ort = orientations[entity];
                Shape hit_box = hit_boxes[entity];
                world_hit_boxes[entity] = physics_shape_world_translate(hit_box, pos, ort);
                return world_hit_boxes[entity];
            }
        }
        return (Shape){0};
}

void system_update_positions(double dt) {
    CMask filter = DYNAMIC;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_is_alive(i)) {
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
        if(entity_is_alive(i)) {
            if( (entity_mask[i] & filter) == filter ) {
                orientations[i] = orientations[i] + angular_velocities[i]*dt;
            }
        }
    }
}


void system_update_angular_velocities(double dt) {
    CMask filter = DYNAMIC;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_is_alive(i)) {
            if( (entity_mask[i] & filter) == filter) {
                angular_velocities[i] += (angular_accelerations[i] + torque_angular_accelerations[i]) * dt;
            }
        }
    }
}

void system_update_velocities(double dt) {
    CMask filter = DYNAMIC;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_is_alive(i)) {
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
  CMask filter = FORCE | TARGETABLE;
  CMask target_filter = DYNAMIC | MASS;

  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(entity_is_alive(i)) { //Check if this entity exists
        if( (entity_mask[i] & filter) == filter ) { //Check if this entity is a targetable force
            if(entity_is_alive(targets[i])) { //Check if the target to the force exists
                if( (entity_mask[targets[i]] & target_filter) == target_filter ) { //Check if the target is moveable
                    if(mass[targets[i]] != 0) {
                        force_accelerations[targets[i]].x += forces[i].x/mass[targets[i]];
                        force_accelerations[targets[i]].y += forces[i].y/mass[targets[i]];
                    } else {
                        //Force on massless entity
                        console_write(
                            LOG_ENGINE,
                            "Error: failed to update acceleration, force entity %d targets massless entity %u\n",
                            i,
                            targets[i]
                        );
                    }
                }
            }
            else {
                //Forces exist without targets
            }
        }
    }
  }
}

void system_apply_torques() {
    //Apply force offset from centroid and torque applied directly
  CMask filter = TORQUE | TARGETABLE;
  CMask target_filter = DYNAMIC | MASS;

  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(entity_is_alive(i)) { //Check if this entity exists
        if( (entity_mask[i] & filter) == filter ) { //Check if this entity is a targetable force
            if(entity_is_alive(targets[i])) { //Check if the target to the force exists
                if( (entity_mask[targets[i]] & target_filter) == target_filter ) { //Check if the target is moveable
                    if(mass[targets[i]] != 0) {
                        torque_angular_accelerations[targets[i]] += torques[i]/physics_polygon_moment_of_inertia(hit_boxes[targets[i]], mass[targets[i]]);
                    } else {
                        //Force on massless entity
                        console_write(
                            LOG_ENGINE,
                            "Error: failed to update angular acceleration, torque entity %d targets massless entity %u\n",
                            i,
                            targets[i]
                        );
                    }
                }
            }
            else {
                //Forces exist without targets
            }
        }
    }
  }
}

void system_clear_force_torque_accelerations() {
    for(int i = 0; i < MAX_ENTITIES; i++) {
        if(!entity_is_alive(i)) {
            continue;
        }
        force_accelerations[i].x = 0;
        force_accelerations[i].y = 0;
        torque_angular_accelerations[i] = 0;
    }
}

Collision system_get_entity_collision(Entity entity_1, Entity entity_2) {
    Shape shape1 = physics_get_global_hit_box(entity_1);
    Shape shape2 = physics_get_global_hit_box(entity_2);
    if(entity_has_components(entity_1, PARTICLE) && entity_has_components(entity_2, PARTICLE)) {
        return physics_particle_collision(shape1, shape2);
    }
    return physics_sat_collision(shape1, shape2);
}

void system_separate_entities_tuned(
    Entity entity_1,
    Entity entity_2,
    Collision collision
) {
    bool dynamic_1 = entity_has_components(entity_1, DYNAMIC);
    bool dynamic_2 = entity_has_components(entity_2, DYNAMIC);

    float inv_mass_1 =
        dynamic_1 && mass[entity_1] > 0.0f
        ? 1.0f / mass[entity_1]
        : 0.0f;

    float inv_mass_2 =
        dynamic_2 && mass[entity_2] > 0.0f
        ? 1.0f / mass[entity_2]
        : 0.0f;

    float inv_mass_sum = inv_mass_1 + inv_mass_2;

    if(inv_mass_sum <= 0.0f) {
        return;
    }

    Vec2D correction = {
        .x = collision.normal.x * collision.depth,
        .y = collision.normal.y * collision.depth
    };

    float share_1 = inv_mass_1 / inv_mass_sum;
    float share_2 = inv_mass_2 / inv_mass_sum;

    positions[entity_1].x -= correction.x * share_1;
    positions[entity_1].y -= correction.y * share_1;

    positions[entity_2].x += correction.x * share_2;
    positions[entity_2].y += correction.y * share_2;
}

void system_separate_entities(Entity entity_1, Entity entity_2, Collision collision)
{
    bool entity_1_dynamic = (entity_mask[entity_1] & DYNAMIC) == DYNAMIC;
    bool entity_2_dynamic = (entity_mask[entity_2] & DYNAMIC) == DYNAMIC;

    Vec2D correction = {
        .x = collision.normal.x * collision.depth,
        .y = collision.normal.y * collision.depth
    };
    Mass mass_1 = mass[entity_1];
    Mass mass_2 = mass[entity_2];
    Mass mass_sum = mass_1 + mass_2;

    if(entity_1_dynamic && entity_2_dynamic) {
        positions[entity_1].x -= ( (correction.x)*(mass_2/(mass_sum)) );
        positions[entity_1].y -= ( (correction.y)*(mass_2/(mass_sum)) );
        positions[entity_2].x += ( (correction.x)*(mass_1/(mass_sum)) );
        positions[entity_2].y += ( (correction.y)*(mass_1/(mass_sum)) );

    }

    else if(entity_1_dynamic && !entity_2_dynamic) {
        positions[entity_1].x -= (correction.x);
        positions[entity_1].y -= (correction.y);
    }
    else if(!entity_1_dynamic && entity_2_dynamic) {
        positions[entity_2].x += (correction.x);
        positions[entity_2].y += (correction.y);
    }
}

Position system_support_point_average(Shape shape, Vec2D direction)
{
    float best_projection = -FLT_MAX;
    Position sum = {0};
    int count = 0;

    const float epsilon = 0.0001f;

    for(int i = 0; i < shape.amount_of_vertices; i += 1) {
        Position vertex = shape.vertices[i];
        float projection = math_dot_product(vertex, direction);

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

Position system_get_particle_edge(Entity entity, Vec2D normal, Vec1D radius) {
    return (Position) {
        .x = positions[entity].x + normal.x*radius,
        .y = positions[entity].y + normal.y*radius,
    };
}

Position system_collision_contact_point(Entity entity_1, Entity entity_2, Collision collision)
{
    Shape shape_1 = physics_get_global_hit_box(entity_1);
    Shape shape_2 = physics_get_global_hit_box(entity_2);

    bool entity_1_dynamic = (entity_mask[entity_1] & DYNAMIC) == DYNAMIC;
    bool entity_2_dynamic = (entity_mask[entity_2] & DYNAMIC) == DYNAMIC;

    Vec2D normal = collision.normal;

    Vec2D opposite_normal = {
        .x = -normal.x,
        .y = -normal.y
    };

    Position point_1 = {0};
    Position point_2 = {0};
    //PARTICLE
    if(entity_has_components(entity_1, PARTICLE) && entity_has_components(entity_2, PARTICLE)){
        Vec1D r1 = math_circle_radius(shape_1, math_polygon_centroid(shape_1));
        Vec1D r2 = math_circle_radius(shape_2, math_polygon_centroid(shape_2));
        point_1 = system_get_particle_edge(entity_1, normal, r1);
        point_2 = system_get_particle_edge(entity_2, opposite_normal, r2);
    }
    else {
        point_1 = system_support_point_average(shape_1, normal);
        point_2 = system_support_point_average(shape_2, opposite_normal);

    }


    //Use dynamic entities contact point
    if(entity_1_dynamic && !entity_2_dynamic) {
        return point_1;
    }

    if(!entity_1_dynamic && entity_2_dynamic) {
        return point_2;
    }

    //Midpoint is good enough for now
    return (Position){
        .x = (point_1.x + point_2.x) * 0.5f,
        .y = (point_1.y + point_2.y) * 0.5f
    };
}

void system_apply_friction_impulse(
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
    Vec2D angular_v1 = math_angular_velocity_cross_vec(angular_velocities[entity_1], r1);
    Vec2D angular_v2 = math_angular_velocity_cross_vec(angular_velocities[entity_2], r2);

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

    float rel_v_along_normal = math_dot_product(rel_v, collision.normal);

    Vec2D tangent = {
        .x = rel_v.x - collision.normal.x * rel_v_along_normal,
        .y = rel_v.y - collision.normal.y * rel_v_along_normal
    };

    float tangent_mag = sqrtf(tangent.x * tangent.x + tangent.y * tangent.y);

    if(tangent_mag <= 0) {
        return;
    }

    tangent.x /= tangent_mag;
    tangent.y /= tangent_mag;

    float r1_cross_t = math_cross_2d(r1, tangent);
    float r2_cross_t = math_cross_2d(r2, tangent);

    float denominator =
        inv_mass_1 +
        inv_mass_2 +
        (r1_cross_t * r1_cross_t) * inv_inertia_1 +
        (r2_cross_t * r2_cross_t) * inv_inertia_2;

    if(denominator <= 0) {
        return;
    }

    float jt = -math_dot_product(rel_v, tangent) / denominator;

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

    angular_velocities[entity_1] -= math_cross_2d(r1, friction_impulse) * inv_inertia_1;
    angular_velocities[entity_2] += math_cross_2d(r2, friction_impulse) * inv_inertia_2;
}

void system_resolve_collision(Entity entity_1, Entity entity_2, Collision collision) {
    //Assume collision.normal points from entity_1 -> entity_2
    bool entity_1_movable =
        ((entity_mask[entity_1] & DYNAMIC) == DYNAMIC);

    bool entity_2_movable =
        ((entity_mask[entity_2] & DYNAMIC) == DYNAMIC);

    float inv_mass_1 = 0.0f;
    float inv_mass_2 = 0.0f;

    if (entity_1_movable && mass[entity_1] > 0.0f) {
        inv_mass_1 = 1.0f / mass[entity_1];
    }

    if (entity_2_movable && mass[entity_2] > 0.0f) {
        inv_mass_2 = 1.0f / mass[entity_2];
    }

    //If neither body can move, no velocity response is needed
    if (inv_mass_1 + inv_mass_2 <= 0.0f) {
        return;
    }

    //Position contact = approximate_contact_point(positions[entity_1], positions[entity_2]);
    Position contact = system_collision_contact_point(entity_1, entity_2, collision);

    Vec2D r1 = {
        .x = contact.x - positions[entity_1].x,
        .y = contact.y - positions[entity_1].y
    };

    Vec2D r2 = {
        .x = contact.x - positions[entity_2].x,
        .y = contact.y - positions[entity_2].y
    };

    Vec2D rotational_velocity_1 = {0};
    Vec2D rotational_velocity_2 = {0};

    if (entity_1_movable) {
        rotational_velocity_1 =
            math_angular_velocity_cross_vec(angular_velocities[entity_1], r1);
    }

    if (entity_2_movable) {
        rotational_velocity_2 =
            math_angular_velocity_cross_vec(angular_velocities[entity_2], r2);
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

    float v_normal = math_dot_product(v_rel, collision.normal);

    if (v_normal > 0.0f) {
        return;
    }

    float restitution = fminf(restitutions[entity_1], restitutions[entity_2]);

    if(fabsf(v_normal) < 1.0f) {
      restitution = 0.0f;
    }

    float inv_inertia_1 = 0.0f;
    float inv_inertia_2 = 0.0f;

    if (entity_1_movable) {
        float inertia_1 = 0;
        if(entity_has_components(entity_1, PARTICLE)) {
            inertia_1 = physics_circle_moment_of_inertia(hit_boxes[entity_1], mass[entity_1]);
        } else {
            inertia_1 =
            physics_polygon_moment_of_inertia(hit_boxes[entity_1], mass[entity_1]);

        }
        if (inertia_1 > 0.0f) {
            inv_inertia_1 = 1.0f / inertia_1;
        }
    }

    if (entity_2_movable) {
        float inertia_2 = 0.0f;

        if(entity_has_components(entity_2, PARTICLE)) {
            inertia_2 = physics_circle_moment_of_inertia(
                hit_boxes[entity_2],
                mass[entity_2]
            );
        } else {
            inertia_2 = physics_polygon_moment_of_inertia(
                hit_boxes[entity_2],
                mass[entity_2]
            );
        }

        if(inertia_2 > 0.0f) {
            inv_inertia_2 = 1.0f / inertia_2;
        }
    }

    float r1_cross_n = math_cross_2d(r1, collision.normal);
    float r2_cross_n = math_cross_2d(r2, collision.normal);

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

    velocities[entity_1].x -= impulse.x * inv_mass_1;
    velocities[entity_1].y -= impulse.y * inv_mass_1;

    velocities[entity_2].x += impulse.x * inv_mass_2;
    velocities[entity_2].y += impulse.y * inv_mass_2;

    angular_velocities[entity_1] -= math_cross_2d(r1, impulse) * inv_inertia_1;
    angular_velocities[entity_2] += math_cross_2d(r2, impulse) * inv_inertia_2;

        system_apply_friction_impulse(
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

void system_add_entities_to_grid() {
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(!entity_is_alive(i)) {
            continue;
        }
        if( (entity_mask[i] & HIT_BOX) == HIT_BOX) {
            add_entity_to_grids(i);
        }
    }
}

void system_apply_collisions_tuned() {
    for(int row = 0; row < GRID_ROWS; row += 1) {
        for(int col = 0; col < GRID_COLS; col += 1) {

            for(int i = 0; i < MAX_ENTITIES; i += 1) {
                if(grid.cells[row][col].entity_present[i]) {
                    for(int j = 0; j < MAX_ENTITIES; j +=1 ) {
                        if(i == j) {
                            continue;
                        }
                        if(grid.cells[row][col].entity_present[j]) {
                            Entity entity_1 = grid.cells[row][col].entities[i];
                            Entity entity_2 = grid.cells[row][col].entities[j];
                            if(checked_pair(entity_1,entity_2)) {
                                continue;
                            }
                            add_pair(entity_1,entity_2);
                            Collision collision = system_get_entity_collision(entity_1, entity_2);
                            if(collision.overlap == true) {
                                physics_set_collision_report(entity_1, entity_2, true);
                                physics_set_collision_report(entity_2, entity_1, true);
                                if((entity_mask[entity_1] & entity_mask[entity_2] & COLLISION) == COLLISION) {
                                    system_resolve_collision(entity_1, entity_2, collision);
                                    system_separate_entities(entity_1,entity_2, collision);

                                    system_generate_global_hitbox(entity_1);
                                    system_generate_global_hitbox(entity_2);
                                    grid_update_aabb(entity_1);
                                    grid_update_aabb(entity_2);
                                }

                            } else {
                                physics_set_collision_report(entity_1, entity_2, false);
                                physics_set_collision_report(entity_2, entity_1, false);

                            }

                        }
                    }
                }

            }

        }
    }
}

void system_apply_collisions() {
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(!entity_is_alive(i)) {
            continue;
        }

        for(int j = i + 1; j < MAX_ENTITIES; j += 1) {
            if(!entity_is_alive(j)) {
                continue;
            }
            if(i == j) {
                continue;
            }

            const CMask filter = HIT_BOX;
            if((entity_mask[i] & entity_mask[j] & HIT_BOX) != HIT_BOX) {
                continue;
            }
            Collision collision = system_get_entity_collision(i, j);


            if(collision.overlap == true) {
                physics_set_collision_report(i, j, true);
                physics_set_collision_report(j, i, true);
                if((entity_mask[i] & entity_mask[j] & COLLISION) == COLLISION) {
                    system_resolve_collision(i, j, collision);
                }

            }
            else {
                physics_set_collision_report(i, j, false);
                physics_set_collision_report(j, i, false);

            }
        }
    }
}

void system_apply_angle_locks() {
    for(Entity entity = 0; entity < MAX_ENTITIES; entity += 1) {
        if(!entity_is_alive(entity)) {
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

        if(fabsf(max - min) <= 0) {
            orientations[entity] = min;
            angular_velocities[entity] = 0.0f;
            angular_accelerations[entity] = 0.0f;
            torque_angular_accelerations[entity] = 0.0f;
            torques[entity] = 0.0f;
            continue;
        }

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
        if(!entity_is_alive(entity)) {
            continue;
        }

        if((entity_mask[entity] & AXIS_LOCK) != AXIS_LOCK) {
            continue;
        }

        Axis axis = axis_locks[entity].axis;

        float mag = math_axis_magnitude(axis);

        if(mag <= 0) {
            continue;
        }

        axis.x /= mag;
        axis.y /= mag;

        Position point_on_axis = axis_locks[entity].point_on_axis;

        Vec2D relative = {
            .x = positions[entity].x - point_on_axis.x,
            .y = positions[entity].y - point_on_axis.y
        };

        float distance_along_axis = math_dot_product(relative, axis);

        positions[entity].x = point_on_axis.x + axis.x * distance_along_axis;
        positions[entity].y = point_on_axis.y + axis.y * distance_along_axis;

        velocities[entity] = math_project_onto_axis(velocities[entity], axis);

        accelerations[entity] = math_project_onto_axis(accelerations[entity], axis);
        force_accelerations[entity] = math_project_onto_axis(force_accelerations[entity], axis);

        forces[entity] = math_project_onto_axis(forces[entity], axis);
    }
}

void system_apply_transform_locks() {
    for(Entity driven = 0; driven < MAX_ENTITIES; driven += 1) {
        if(!entity_is_alive(driven)) {
            continue;
        }

        if((entity_mask[driven] & TRANSFORM_LOCK) != TRANSFORM_LOCK) {
            continue;
        }

        Entity driver = transform_locks[driven].driver;

        if(!entity_is_alive(driver)) {
            physics_remove_transform_lock(driven);
            continue;
        }

        Vec2D world_offset = math_rotate_vector(
            transform_locks[driven].local_offset,
            orientations[driver]
        );

        if(transform_locks[driven].lock_position) {
            positions[driven].x = positions[driver].x + world_offset.x;
            positions[driven].y = positions[driver].y + world_offset.y;
        }

        if(transform_locks[driven].lock_orientation) {
            orientations[driven] =
                orientations[driver] + transform_locks[driven].local_angle;
        }

        if(transform_locks[driven].inherit_velocity) {
            velocities[driven] = velocities[driver];

            Vec2D rotational_velocity = math_angular_velocity_cross_vec(
                angular_velocities[driver],
                world_offset
            );

            velocities[driven].x += rotational_velocity.x;
            velocities[driven].y += rotational_velocity.y;

            if(transform_locks[driven].lock_orientation) {
                angular_velocities[driven] = angular_velocities[driver];
            }
        }
    }
}

void system_clean_entities_past_lifetime() {
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(!entity_is_alive(i)) {
            continue;
        }
        if( (entity_mask[i] & LIFETIME) == LIFETIME ) {

            if( (life_times[i].expirey_time != 0 && life_times[i].expirey_time <= engine_get_time()) ) {
                entity_delete(i);
            }
            else if( (life_times[i].expirey_tick != 0 && life_times[i].expirey_tick <= engine_get_tick()) ) {
                entity_delete(i);
            }
        }
    }
}

static Velocity system_point_velocity(Entity entity, Vec2D world_offset) {
    Vec2D angular_part = math_angular_velocity_cross_vec(
        angular_velocities[entity],
        world_offset
    );

    return (Velocity){
        .x = velocities[entity].x + angular_part.x,
        .y = velocities[entity].y + angular_part.y
    };
}
static void system_add_joint_force_for_one_tick(Entity target, Force force) {
    //Entity force_entity = set_force(target, force);
    force_accelerations[target].x += force.x/mass[target];
    force_accelerations[target].y += force.y/mass[target];

    //if(force_entity == 0) {
    //    return;
    //}

    //set_life_time(
    //    force_entity,
    //    0.0,
    //    engine_get_tick() + 1
    //);
}
static void system_add_joint_torque_for_one_tick(Entity target, Torque torque) {
    torque_angular_accelerations[target] += torque/physics_polygon_moment_of_inertia(hit_boxes[target], mass[target]);
    //Entity torque_entity = set_torque(target, torque);

    //if(torque_entity == 0) {
    //    return;
    //}

    //set_life_time(
    //    torque_entity,
    //    0.0,
    //    engine_get_tick() + 1
    //);
}

static void system_add_joint_force_at_point_for_one_tick(Entity target, Position world_point, Force force) {
    if(!entity_is_alive(target)) {
        return;
    }

    if((entity_mask[target] & DYNAMIC) != DYNAMIC) {
        return;
    }

    Entity force_entity = physics_set_force(target, force);

    if(force_entity != 0) {
        entity_set_life_time(force_entity, 0.0, engine_get_tick() + 1);
    }

    Vec2D r = {
        .x = world_point.x - positions[target].x,
        .y = world_point.y - positions[target].y
    };

    Torque torque = math_cross_2d(r, force);

    Entity torque_entity = physics_set_torque(target, torque);

    if(torque_entity != 0) {
        entity_set_life_time(torque_entity, 0.0, engine_get_tick() + 1);
    }
}

static void system_apply_pin_joint(Entity joint_entity) {
    Joint joint = joints[joint_entity];

    Entity a = joint.a;
    Entity b = joint.b;

    if(!entity_is_alive(a) || !entity_is_alive(b)) {
        entity_delete(joint_entity);
        return;
    }

    Vec2D offset_a = math_rotate_vector(
        joint.local_anchor_a,
        orientations[a]
    );

    Vec2D offset_b = math_rotate_vector(
        joint.local_anchor_b,
        orientations[b]
    );

    Position world_anchor_a = {
        .x = positions[a].x + offset_a.x,
        .y = positions[a].y + offset_a.y
    };

    Position world_anchor_b = {
        .x = positions[b].x + offset_b.x,
        .y = positions[b].y + offset_b.y
    };

    Vec2D error = {
        .x = world_anchor_b.x - world_anchor_a.x,
        .y = world_anchor_b.y - world_anchor_a.y
    };

    Velocity velocity_a = system_point_velocity(a, offset_a);
    Velocity velocity_b = system_point_velocity(b, offset_b);

    Vec2D relative_velocity = {
        .x = velocity_b.x - velocity_a.x,
        .y = velocity_b.y - velocity_a.y
    };

    Force force_on_a = {
        .x = joint.stiffness * error.x + joint.damping * relative_velocity.x,
        .y = joint.stiffness * error.y + joint.damping * relative_velocity.y
    };

    Force force_on_b = {
        .x = -force_on_a.x,
        .y = -force_on_a.y
    };

    system_add_joint_force_at_point_for_one_tick(
        a,
        world_anchor_a,
        force_on_a
    );

    system_add_joint_force_at_point_for_one_tick(
        b,
        world_anchor_b,
        force_on_b
    );
}

void system_apply_distance_joint(Entity joint_entity) {
        Joint joint = joints[joint_entity];
        if(joint.type != JOINT_DISTANCE) {
            return;
        }

        Entity a = joint.a;
        Entity b = joint.b;

        Vec2D offset_a = math_rotate_vector(
            joint.local_anchor_a,
            orientations[a]
        );

        Vec2D offset_b = math_rotate_vector(
            joint.local_anchor_b,
            orientations[b]
        );

        Position world_anchor_a = {
            .x = positions[a].x + offset_a.x,
            .y = positions[a].y + offset_a.y
        };

        Position world_anchor_b = {
            .x = positions[b].x + offset_b.x,
            .y = positions[b].y + offset_b.y
        };

        Vec2D delta = {
            .x = world_anchor_b.x - world_anchor_a.x,
            .y = world_anchor_b.y - world_anchor_a.y
        };

        float length = math_vector_magnitude(delta);

        if(length <= 0.0) {
            return;
        }

        Vec2D normal = {
            .x = delta.x / length,
            .y = delta.y / length
        };

        Velocity velocity_a = system_point_velocity(a, offset_a);
        Velocity velocity_b = system_point_velocity(b, offset_b);

        Vec2D relative_velocity = {
            .x = velocity_b.x - velocity_a.x,
            .y = velocity_b.y - velocity_a.y
        };

        float relative_speed = math_dot_product(relative_velocity, normal);

        float stretch = length - joint.rest_length;

        float force_magnitude =
            joint.stiffness * stretch +
            joint.damping * relative_speed;

        Force force_on_a = {
            .x = normal.x * force_magnitude,
            .y = normal.y * force_magnitude
        };

        Force force_on_b = {
            .x = -force_on_a.x,
            .y = -force_on_a.y
        };

        Vec2D r_a = {
            .x = world_anchor_a.x - positions[a].x,
            .y = world_anchor_a.y - positions[a].y
        };

        Vec2D r_b = {
            .x = world_anchor_b.x - positions[b].x,
            .y = world_anchor_b.y - positions[b].y
        };

        Torque torque_on_a = math_cross_2d(r_a, force_on_a);
        Torque torque_on_b = math_cross_2d(r_b, force_on_b);

        system_add_joint_force_for_one_tick(a, force_on_a);
        system_add_joint_force_for_one_tick(b, force_on_b);

        system_add_joint_torque_for_one_tick(a, torque_on_a);
        system_add_joint_torque_for_one_tick(b, torque_on_b);

}
void system_apply_joints()
{
    for(Entity joint_entity = 1; joint_entity < MAX_ENTITIES; joint_entity += 1) {
        if(!entity_is_alive(joint_entity)) {
            continue;
        }
        Joint joint = joints[joint_entity];
        switch(joint.type) {
            case JOINT_PIN:
                system_apply_pin_joint(joint_entity);
                break;
            case JOINT_DISTANCE:
                system_apply_distance_joint(joint_entity);
                break;
            case JOINT_WELD:
                //Not implemented
                break;
            default:
                //Not implemented
                break;
        }

    }
}

void system_update_aabbs() {
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(!entity_is_alive(i)) {
            continue;
        }
        if( (entity_mask[i] & HIT_BOX) == HIT_BOX) {
            grid_update_aabb(i);
        }
    }
}

void system_update_physics(double dt) {
    system_clear_force_torque_accelerations();
    system_apply_joints();

    system_apply_forces();
    system_apply_torques();
    system_update_velocities(dt);
    system_update_angular_velocities(dt);
    system_update_orientations(dt);
    system_update_positions(dt);
    system_apply_axis_locks();
    system_apply_angle_locks();
    system_apply_transform_locks();

    system_generate_global_hitboxes();
    system_update_aabbs();
    system_add_entities_to_grid();
    system_apply_collisions_tuned();
    clear_grid();
}

void print_entity_movement(Entity entity) {
    console_write(LOG_ENGINE, "---Movement Log---\n");
    console_write(LOG_ENGINE, "Entity: %d\n", entity);
    console_write(LOG_ENGINE, "Position: {x: %f, y: %f}\n", positions[entity].x, positions[entity].y);
    console_write(LOG_ENGINE, "Velocity: {x: %f, y: %f}\n", velocities[entity].x, velocities[entity].y);
    console_write(LOG_ENGINE, "Acceleration: {x: %f, y: %f}\n", accelerations[entity].x, accelerations[entity].y);
    console_write(LOG_ENGINE, "---Movement Log---\n");
}
