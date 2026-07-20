#include "physics.h"
#include "float.h"
#include <math.h>
#include "console.h"

Position positions[MAX_ENTITIES] = {0};
Orientation orientations[MAX_ENTITIES] = {0};
Velocity velocities[MAX_ENTITIES] = {0};
Acceleration accelerations[MAX_ENTITIES] = {0};
Acceleration force_accelerations[MAX_ENTITIES] = {0};
float mass[MAX_ENTITIES] = {0};
Force forces[MAX_ENTITIES] = {0};
Shape hit_boxes[MAX_ENTITIES] = {0};
Shape world_hit_boxes[MAX_ENTITIES] = {0};
CollisionReport collision_reports[MAX_ENTITIES] = {0};
AngularVelocity angular_velocities[MAX_ENTITIES] = {0};
AngularAcceleration angular_accelerations[MAX_ENTITIES] = {0};
AngularVelocity torque_angular_accelerations[MAX_ENTITIES] = {0};
Torque torques[MAX_ENTITIES] = {0};
Friction frictions[MAX_ENTITIES] = {0};
Restitution restitutions[MAX_ENTITIES] = {0};
AngleLock angle_locks[MAX_ENTITIES] = {0};
AxisLock axis_locks[MAX_ENTITIES] = {0};
TransformLock transform_locks[MAX_ENTITIES] = {0};
Joint joints[MAX_ENTITIES] = {0};
Shape physics_shape_world_translate(Shape shape, Position position, Orientation angle) {
    Shape world_shape = {0};
    world_shape.amount_of_vertices = shape.amount_of_vertices;

    Position center = math_polygon_centroid(shape);

    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        float x = shape.vertices[i].x - center.x;
        float y = shape.vertices[i].y - center.y;

        float rotated_x = x*cos_a - y*sin_a;
        float rotated_y = x*sin_a + y*cos_a;

        world_shape.vertices[i].x = position.x + rotated_x;
        world_shape.vertices[i].y = position.y + rotated_y;
    }

    return world_shape;
}
float physics_polygon_moment_of_inertia(Shape shape, Mass mass) {
    Position c = math_polygon_centroid(shape);

    float area_sum = 0.0f;
    float inertia_sum = 0.0f;

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        int j = (i + 1) % shape.amount_of_vertices;

        float xi = shape.vertices[i].x - c.x;
        float yi = shape.vertices[i].y - c.y;

        float xj = shape.vertices[j].x - c.x;
        float yj = shape.vertices[j].y - c.y;

        float cross = xi * yj - xj * yi;

        float q =
            xi*xi + xi*xj + xj*xj +
            yi*yi + yi*yj + yj*yj;

        area_sum += cross;
        inertia_sum += cross * q;
    }

    float area = 0.5f * area_sum;
    float area_moment = inertia_sum / 12.0f;

    if (fabsf(area) < 1e-8f) { //Very small area no inertia calc needed
        return 0;
    }

    float density = mass / fabsf(area);
    float inertia = density * fabsf(area_moment);

    return inertia;
}
Collision physics_sat_collision_on_axes(Shape shape_1, Shape shape_2, Vec2DList axes, Collision collision) {
    for (int i = 0; i < axes.amount_of_vectors; i += 1) {
        Axis axis = axes.vectors[i];

        Projection p1 = math_project_shape_on_axis(shape_1, axis);
        Projection p2 = math_project_shape_on_axis(shape_2, axis);

        float overlap = math_projection_overlap(p1, p2);

        if (overlap <= 0.0f) {
            return (Collision){ .overlap = false };
        }

        if (overlap < collision.depth) {
            collision.depth = overlap;
            collision.normal = axis;
        }
    }

    return collision;
}
Collision physics_particle_collision(Shape shape_1, Shape shape_2)
{
    Position center_1 = math_polygon_centroid(shape_1);
    Position center_2 = math_polygon_centroid(shape_2);

    float radius_1 = math_circle_radius(shape_1, center_1);
    float radius_2 = math_circle_radius(shape_2, center_2);

    Vec2D delta = {
        .x = center_2.x - center_1.x,
        .y = center_2.y - center_1.y
    };

    float distance_squared =
        delta.x * delta.x +
        delta.y * delta.y;

    float radius_sum = radius_1 + radius_2;
    float radius_sum_squared = radius_sum * radius_sum;

    if(distance_squared >= radius_sum_squared) {
        return (Collision){
            .overlap = false
        };
    }

    float distance = sqrtf(distance_squared);

    Vec2D normal;

    if(distance > 0.00001f) {
        normal = (Vec2D){
            .x = delta.x / distance,
            .y = delta.y / distance
        };
    } else {
        normal = (Vec2D){1.0f, 0.0f};
        distance = 0.0f;
    }

    return (Collision){
        .overlap = true,
        .normal = normal,
        .depth = radius_sum - distance
    };
}

Collision physics_sat_collision(Shape shape_1, Shape shape_2)
{
    Collision collision = {
        .overlap = true,
        .normal = {0},
        .depth = FLT_MAX
    };

    Vec2DList shape1_axes = math_normalize_vectors(math_create_normals(shape_1));
    Vec2DList shape2_axes = math_normalize_vectors(math_create_normals(shape_2));

    collision = physics_sat_collision_on_axes(shape_1, shape_2, shape1_axes, collision);

    if (!collision.overlap) {
        return collision;
    }

    collision = physics_sat_collision_on_axes(shape_1, shape_2, shape2_axes, collision);

    if (!collision.overlap) {
        return collision;
    }

    Position c1 = math_polygon_centroid(shape_1);
    Position c2 = math_polygon_centroid(shape_2);

    Vec2D center_delta = {
        .x = c2.x - c1.x,
        .y = c2.y - c1.y
    };

    if (math_dot_product(center_delta, collision.normal) < 0.0f) {
        collision.normal.x *= -1.0f;
        collision.normal.y *= -1.0f;
    }

    return collision;
}
Position physics_approximate_contact_point(Position p1, Position p2)
{
    return (Position){
        .x = (p1.x + p2.x) * 0.5f,
        .y = (p1.y + p2.y) * 0.5f
    };
}
Vec1D physics_circle_moment_of_inertia(Shape circle, Mass mass) {
  Vec1D radius = math_circle_radius(circle, math_polygon_centroid(circle));
  Vec1D area = PI_F*radius*radius;
  Vec1D density = mass/fabsf(area);
  Vec1D area_moment = 0.5f * area * radius * radius;
  return density * area_moment;
}

//Entity
void physics_set_velocity(Entity entity, Velocity v) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    velocities[entity] = v;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Velocity: {x: %f, y: %f}\n", entity, v.x, v.y);
}
void physics_set_position(Entity entity, Position p) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    positions[entity] = p;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Position: {x: %f, y: %f}\n", entity, p.x, p.y);
}

void physics_set_mass(Entity entity, Mass m) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_mask[entity] |= MASS;
    mass[entity] = m;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Mass: %f\n", entity, m);
}
Entity physics_set_force(Entity entity, Force f) {
    if(!entity_alive[entity]) {
        //Error
        return 0;
    }
    Entity force_entity = entity_add();
    forces[force_entity] = f;
    targets[force_entity] = entity;
    entity_mask[force_entity] |= TARGETABLE | FORCE;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Force: {x: %f, y: %f}\n", entity, f.x, f.y);
    return force_entity;
}
void physics_set_acceleration(Entity entity, Acceleration a) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    accelerations[entity] = a;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Acceleration: {x: %f, y: %f}\n", entity, a.x, a.y);
}
Entity physics_set_torque(Entity entity, Torque t) {
    if(!entity_alive[entity]) {
        //Error
        return 0;
    }
    Entity torque_entity = entity_add();
    torques[torque_entity] = t;
    targets[torque_entity] = entity;
    entity_mask[torque_entity] |= TARGETABLE | TORQUE;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Torque: %f\n", entity, t);
    return torque_entity;
}
void physics_set_hitbox(Entity entity, Shape hitbox) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_mask[entity] |= COLLISION | HIT_BOX;
    hit_boxes[entity] = hitbox;
  console_debug_write(LOG_ENGINE, "Set Entity: %d to have a hit box\n", entity);
}
void physics_set_orientation(Entity entity, Orientation angle) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
  orientations[entity] = angle;
  console_debug_write(LOG_ENGINE, "Set Entity: %d Orientation: %f\n", entity, angle);
}
void physics_set_angular_velocity(Entity entity, AngularVelocity v) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    physics_set_dynamic(entity);
    angular_velocities[entity] = v;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Angular Velocity: %f\n", entity, v);
}
Shape physics_get_global_hit_box(Entity entity) {
    CMask filter = HIT_BOX;
    if(entity_alive[entity]) {
        if( (entity_mask[entity] & filter) == filter ) {
            return world_hit_boxes[entity];
        }
    }
    return (Shape){0};
}
 void physics_set_restitution(Entity entity, Restitution restitution) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
     if(restitution < 0) {
        restitutions[entity] = 0;
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, 0);
     }
     else if(restitution > 1) {
        restitutions[entity] = 1;
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, 1);
     }
     else {
        restitutions[entity] = restitution;
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, restitution);
     }
     entity_add_components(entity, COLLISION);
 }
void physics_set_dynamic(Entity entity) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_add_components(entity, DYNAMIC);
    entity_delete_components(entity, STATIC);
    console_debug_write(LOG_ENGINE, "Set Entity: %d to STATIC\n", entity);
}
void physics_set_static(Entity entity) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_add_components(entity, STATIC);
    entity_delete_components(entity, DYNAMIC);
    console_debug_write(LOG_ENGINE, "Set Entity: %d to DYNAMIC\n", entity);
}
void physics_set_angle_lock(Entity entity, Orientation min, Orientation max) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_add_components(entity, ANGLE_LOCK);
    angle_locks[entity] = (AngleLock){
        .min = min,
        .max = max
    };
}
void physics_set_axis_lock(Entity entity, Axis axis, Position axis_point) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_add_components(entity, AXIS_LOCK);
    Axis normalized_axis = math_normalize_vector(axis);
    axis_locks[entity] = (AxisLock){
        .axis = (Axis){
            .x = normalized_axis.x,
            .y = normalized_axis.y
        },
        .point_on_axis = (Position){
            .x = axis_point.x,
            .y = axis_point.y
        }
    };
}
void physics_set_friction(Entity entity, float friction) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    if(friction < 0) {
        frictions[entity] = 0;
    }
    else if(friction >= 0) {
        frictions[entity] = friction;
    }
}
void physics_set_transform_lock(
        Entity driven,
        Entity driver, 
        Vec2D local_offset,
        Orientation local_angle,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
        ) {
    if(!entity_alive[driven] || !entity_alive[driver]) {
        //Error
        return;
    }
    entity_add_components(driven, TRANSFORM_LOCK);
    transform_locks[driven] = (TransformLock) {
        .driver = driver,
        .local_offset = local_offset,
        .local_angle = local_angle,
        .lock_position = lock_position,
        .lock_orientation = lock_orientation,
        .inherit_velocity = inherit_velocity
    };
}
void physics_remove_transform_lock(Entity entity) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_delete_components(entity, TRANSFORM_LOCK);
    transform_locks[entity] = (TransformLock){0};
}
void physics_set_transform_lock_current_transform(
        Entity driven,
        Entity driver,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
        ) {
    if(!entity_alive[driven] || !entity_alive[driver]) {
        return;
    }

    Vec2D world_offset = {
        .x = positions[driven].x - positions[driver].x,
        .y = positions[driven].y - positions[driver].y
    };

    Vec2D local_offset = math_rotate_vector(
        world_offset,
        -orientations[driver]
    );

    Orientation local_angle =
        orientations[driven] - orientations[driver];

    physics_set_transform_lock(
        driven,
        driver,
        local_offset,
        local_angle,
        lock_position,
        lock_orientation,
        inherit_velocity
    );
}
Entity physics_set_joint(
    Entity a,
    Entity b,
    JointType type,
    Vec2D local_anchor_a,
    Vec2D local_anchor_b,
    float stiffness,
    float damping
) {
    if(!entity_alive[a] || !entity_alive[b]) {
        return 0;
    }

    Entity joint = entity_add();

    entity_add_components(joint, JOINT);

    Vec2D world_anchor_a = {
        .x = positions[a].x + math_rotate_vector(local_anchor_a, orientations[a]).x,
        .y = positions[a].y + math_rotate_vector(local_anchor_a, orientations[a]).y
    };

    Vec2D world_anchor_b = {
        .x = positions[b].x + math_rotate_vector(local_anchor_b, orientations[b]).x,
        .y = positions[b].y + math_rotate_vector(local_anchor_b, orientations[b]).y
    };

    Vec2D delta = {
        .x = world_anchor_b.x - world_anchor_a.x,
        .y = world_anchor_b.y - world_anchor_a.y
    };

    joints[joint] = (Joint){
        .type = type,
        .a = a,
        .b = b,
        .local_anchor_a = local_anchor_a,
        .local_anchor_b = local_anchor_b,
        .rest_length = math_vector_magnitude(delta),
        .stiffness = stiffness,
        .damping = damping,
        .lock_angle = false,
        .rest_angle = orientations[b] - orientations[a],
        .angular_stiffness = 0.0f,
        .angular_damping = 0.0f
    };

    return joint;
}

void physics_set_collision_report(Entity entity, Entity target, bool state) {
    collision_reports[entity].collisions[target] = state;
}
bool physics_get_collision_report(Entity entity, Entity target) {
    if(collision_reports[entity].collisions[target] && collision_reports[target].collisions[entity]) {
        return true;
    }
    return false;
}
