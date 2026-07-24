#include "physics.h"
#include "float.h"
#include <math.h>
#include "console.h"

MEMORY_DEFINE_OBJECT_POOL(PositionPool, Position)
MEMORY_DEFINE_OBJECT_POOL(VelocityPool, Velocity)
MEMORY_DEFINE_OBJECT_POOL(AccelerationPool, Acceleration)
MEMORY_DEFINE_OBJECT_POOL(MassPool, float)
MEMORY_DEFINE_OBJECT_POOL(ForcePool, Force)
MEMORY_DEFINE_OBJECT_POOL(ShapePool, Shape)
MEMORY_DEFINE_OBJECT_POOL(CollisionReportPool, CollisionReport)
MEMORY_DEFINE_OBJECT_POOL(OrientationPool, Orientation)
MEMORY_DEFINE_OBJECT_POOL(AngularVelocityPool, AngularVelocity)
MEMORY_DEFINE_OBJECT_POOL(AngularAccelerationPool, AngularAcceleration)
MEMORY_DEFINE_OBJECT_POOL(TorquePool, Torque)
MEMORY_DEFINE_OBJECT_POOL(FrictionPool, Friction)
MEMORY_DEFINE_OBJECT_POOL(RestitutionPool, Restitution)
MEMORY_DEFINE_OBJECT_POOL(AngleLockPool, AngleLock)
MEMORY_DEFINE_OBJECT_POOL(AxisLockPool, AxisLock)
MEMORY_DEFINE_OBJECT_POOL(TransformLockPool, TransformLock)
MEMORY_DEFINE_OBJECT_POOL(JointPool, Joint)

PositionPool positions_pool = {0};
OrientationPool orientations_pool = {0};
VelocityPool velocities_pool = {0};
AccelerationPool accelerations_pool = {0};
AccelerationPool force_accelerations_pool = {0};
MassPool mass_pool = {0};
ForcePool forces_pool = {0};
ShapePool hit_boxes_pool = {0};
ShapePool world_hit_boxes_pool = {0};
CollisionReportPool collision_reports_pool = {0};
AngularVelocityPool angular_velocities_pool = {0};
AngularAccelerationPool angular_accelerations_pool = {0};
AngularVelocityPool torque_angular_accelerations_pool = {0};
TorquePool torques_pool = {0};
FrictionPool frictions_pool = {0};
RestitutionPool restitutions_pool = {0};
AngleLockPool angle_locks_pool = {0};
AxisLockPool axis_locks_pool = {0};
TransformLockPool transform_locks_pool = {0};
JointPool joints_pool = {0};

bool physics_tables_init(void) {
    if(PositionPool_init(&positions_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(OrientationPool_init(&orientations_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(VelocityPool_init(&velocities_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(AccelerationPool_init(&accelerations_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(AccelerationPool_init(&force_accelerations_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(MassPool_init(&mass_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(ForcePool_init(&forces_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(ShapePool_init(&hit_boxes_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(ShapePool_init(&world_hit_boxes_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(CollisionReportPool_init(&collision_reports_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(AngularVelocityPool_init(&angular_velocities_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(AngularAccelerationPool_init(&angular_accelerations_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(AngularVelocityPool_init(&torque_angular_accelerations_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(TorquePool_init(&torques_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(FrictionPool_init(&frictions_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(RestitutionPool_init(&restitutions_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(AngleLockPool_init(&angle_locks_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(AxisLockPool_init(&axis_locks_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(TransformLockPool_init(&transform_locks_pool, 0).kind == RESULT_ERROR) { goto fail; }
    if(JointPool_init(&joints_pool, 0).kind == RESULT_ERROR) { goto fail; }
    return true;

fail:
    physics_tables_destroy();
    return false;
}

bool physics_tables_ensure_capacity(size_t capacity) {
    if(capacity > MAX_ENTITIES) {
        return false;
    }
    if(capacity > positions_pool.capacity && PositionPool_expand(&positions_pool, capacity - positions_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > orientations_pool.capacity && OrientationPool_expand(&orientations_pool, capacity - orientations_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > velocities_pool.capacity && VelocityPool_expand(&velocities_pool, capacity - velocities_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > accelerations_pool.capacity && AccelerationPool_expand(&accelerations_pool, capacity - accelerations_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > force_accelerations_pool.capacity && AccelerationPool_expand(&force_accelerations_pool, capacity - force_accelerations_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > mass_pool.capacity && MassPool_expand(&mass_pool, capacity - mass_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > forces_pool.capacity && ForcePool_expand(&forces_pool, capacity - forces_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > hit_boxes_pool.capacity && ShapePool_expand(&hit_boxes_pool, capacity - hit_boxes_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > world_hit_boxes_pool.capacity && ShapePool_expand(&world_hit_boxes_pool, capacity - world_hit_boxes_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > collision_reports_pool.capacity && CollisionReportPool_expand(&collision_reports_pool, capacity - collision_reports_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > angular_velocities_pool.capacity && AngularVelocityPool_expand(&angular_velocities_pool, capacity - angular_velocities_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > angular_accelerations_pool.capacity && AngularAccelerationPool_expand(&angular_accelerations_pool, capacity - angular_accelerations_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > torque_angular_accelerations_pool.capacity && AngularVelocityPool_expand(&torque_angular_accelerations_pool, capacity - torque_angular_accelerations_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > torques_pool.capacity && TorquePool_expand(&torques_pool, capacity - torques_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > frictions_pool.capacity && FrictionPool_expand(&frictions_pool, capacity - frictions_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > restitutions_pool.capacity && RestitutionPool_expand(&restitutions_pool, capacity - restitutions_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > angle_locks_pool.capacity && AngleLockPool_expand(&angle_locks_pool, capacity - angle_locks_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > axis_locks_pool.capacity && AxisLockPool_expand(&axis_locks_pool, capacity - axis_locks_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > transform_locks_pool.capacity && TransformLockPool_expand(&transform_locks_pool, capacity - transform_locks_pool.capacity).kind == RESULT_ERROR) { return false; }
    if(capacity > joints_pool.capacity && JointPool_expand(&joints_pool, capacity - joints_pool.capacity).kind == RESULT_ERROR) { return false; }
    return true;
}

void physics_tables_destroy(void) {
    (void)PositionPool_destroy(&positions_pool);
    (void)OrientationPool_destroy(&orientations_pool);
    (void)VelocityPool_destroy(&velocities_pool);
    (void)AccelerationPool_destroy(&accelerations_pool);
    (void)AccelerationPool_destroy(&force_accelerations_pool);
    (void)MassPool_destroy(&mass_pool);
    (void)ForcePool_destroy(&forces_pool);
    (void)ShapePool_destroy(&hit_boxes_pool);
    (void)ShapePool_destroy(&world_hit_boxes_pool);
    (void)CollisionReportPool_destroy(&collision_reports_pool);
    (void)AngularVelocityPool_destroy(&angular_velocities_pool);
    (void)AngularAccelerationPool_destroy(&angular_accelerations_pool);
    (void)AngularVelocityPool_destroy(&torque_angular_accelerations_pool);
    (void)TorquePool_destroy(&torques_pool);
    (void)FrictionPool_destroy(&frictions_pool);
    (void)RestitutionPool_destroy(&restitutions_pool);
    (void)AngleLockPool_destroy(&angle_locks_pool);
    (void)AxisLockPool_destroy(&axis_locks_pool);
    (void)TransformLockPool_destroy(&transform_locks_pool);
    (void)JointPool_destroy(&joints_pool);
}

static bool physics_entity_valid(Entity entity) {
    return entity < MAX_ENTITIES;
}

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
float physics_polygon_moment_of_inertia(Shape shape, Mass mass_value) {
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

    float density = mass_value / fabsf(area);
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
Vec1D physics_circle_moment_of_inertia(Shape circle, Mass mass_value) {
  Vec1D radius = math_circle_radius(circle, math_polygon_centroid(circle));
  Vec1D area = PI_F*radius*radius;
  Vec1D density = mass_value/fabsf(area);
  Vec1D area_moment = 0.5f * area * radius * radius;
  return density * area_moment;
}

//Entity
void physics_set_velocity(Entity entity, Velocity v) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    (void)VelocityPool_store_at(&velocities_pool, entity, v);
    console_debug_write(LOG_ENGINE, "Set Entity: %d Velocity: {x: %f, y: %f}\n", entity, v.x, v.y);
}
void physics_set_position(Entity entity, Position p) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    (void)PositionPool_store_at(&positions_pool, entity, p);
    console_debug_write(LOG_ENGINE, "Set Entity: %d Position: {x: %f, y: %f}\n", entity, p.x, p.y);
}

void physics_set_mass(Entity entity, Mass m) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    entity_mask[entity] |= MASS;
    (void)MassPool_store_at(&mass_pool, entity, m);
    console_debug_write(LOG_ENGINE, "Set Entity: %d Mass: %f\n", entity, m);
}
Entity physics_set_force(Entity entity, Force f) {
    if(!physics_entity_valid(entity)) {
        return 0;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return 0;
    }
    Entity force_entity = entity_add();
    if(!physics_entity_valid(force_entity) || !entity_is_alive(force_entity)) {
        return 0;
    }
    (void)ForcePool_store_at(&forces_pool, force_entity, f);
    (void)TargetPool_store_at(&targets_pool, force_entity, entity);
    entity_mask[force_entity] |= TARGETABLE | FORCE;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Force: {x: %f, y: %f}\n", entity, f.x, f.y);
    return force_entity;
}
void physics_set_acceleration(Entity entity, Acceleration a) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    (void)AccelerationPool_store_at(&accelerations_pool, entity, a);
    console_debug_write(LOG_ENGINE, "Set Entity: %d Acceleration: {x: %f, y: %f}\n", entity, a.x, a.y);
}
Entity physics_set_torque(Entity entity, Torque t) {
    if(!physics_entity_valid(entity)) {
        return 0;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return 0;
    }
    Entity torque_entity = entity_add();
    if(!physics_entity_valid(torque_entity) || !entity_is_alive(torque_entity)) {
        return 0;
    }
    (void)TorquePool_store_at(&torques_pool, torque_entity, t);
    (void)TargetPool_store_at(&targets_pool, torque_entity, entity);
    entity_mask[torque_entity] |= TARGETABLE | TORQUE;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Torque: %f\n", entity, t);
    return torque_entity;
}
void physics_set_hitbox(Entity entity, Shape hitbox) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    entity_mask[entity] |= COLLISION | HIT_BOX;
    (void)ShapePool_store_at(&hit_boxes_pool, entity, hitbox);
  console_debug_write(LOG_ENGINE, "Set Entity: %d to have a hit box\n", entity);
}
void physics_set_orientation(Entity entity, Orientation angle) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
  (void)OrientationPool_store_at(&orientations_pool, entity, angle);
  console_debug_write(LOG_ENGINE, "Set Entity: %d Orientation: %f\n", entity, angle);
}
void physics_set_angular_velocity(Entity entity, AngularVelocity v) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    physics_set_dynamic(entity);
    (void)AngularVelocityPool_store_at(&angular_velocities_pool, entity, v);
    console_debug_write(LOG_ENGINE, "Set Entity: %d Angular Velocity: %f\n", entity, v);
}
Shape physics_get_global_hit_box(Entity entity) {
    CMask filter = HIT_BOX;
    if(!physics_entity_valid(entity)) {
        return (Shape){0};
    }
    if(entity_is_alive(entity)) {
        if( (entity_mask[entity] & filter) == filter ) {
            return world_hit_boxes[entity];
        }
    }
    return (Shape){0};
}
 void physics_set_restitution(Entity entity, Restitution restitution) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
     if(restitution < 0) {
        (void)RestitutionPool_store_at(&restitutions_pool, entity, 0);
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, 0);
     }
     else if(restitution > 1) {
        (void)RestitutionPool_store_at(&restitutions_pool, entity, 1);
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, 1);
     }
     else {
        (void)RestitutionPool_store_at(&restitutions_pool, entity, restitution);
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, restitution);
     }
     entity_add_components(entity, COLLISION);
 }
void physics_set_dynamic(Entity entity) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    entity_add_components(entity, DYNAMIC);
    entity_delete_components(entity, STATIC);
    console_debug_write(LOG_ENGINE, "Set Entity: %d to STATIC\n", entity);
}
void physics_set_static(Entity entity) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    entity_add_components(entity, STATIC);
    entity_delete_components(entity, DYNAMIC);
    console_debug_write(LOG_ENGINE, "Set Entity: %d to DYNAMIC\n", entity);
}
void physics_set_angle_lock(Entity entity, Orientation min, Orientation max) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    entity_add_components(entity, ANGLE_LOCK);
    (void)AngleLockPool_store_at(&angle_locks_pool, entity, (AngleLock){
        .min = min,
        .max = max
    });
}
void physics_set_axis_lock(Entity entity, Axis axis, Position axis_point) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    entity_add_components(entity, AXIS_LOCK);
    Axis normalized_axis = math_normalize_vector(axis);
    (void)AxisLockPool_store_at(&axis_locks_pool, entity, (AxisLock){
        .axis = (Axis){
            .x = normalized_axis.x,
            .y = normalized_axis.y
        },
        .point_on_axis = (Position){
            .x = axis_point.x,
            .y = axis_point.y
        }
    });
}
void physics_set_friction(Entity entity, float friction) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    if(friction < 0) {
        (void)FrictionPool_store_at(&frictions_pool, entity, 0);
    }
    else if(friction >= 0) {
        (void)FrictionPool_store_at(&frictions_pool, entity, friction);
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
    if(!physics_entity_valid(driven) || !physics_entity_valid(driver)) {
        return;
    }
    if(!entity_is_alive(driven) || !entity_is_alive(driver)) {
        //Error
        return;
    }
    entity_add_components(driven, TRANSFORM_LOCK);
    (void)TransformLockPool_store_at(&transform_locks_pool, driven, (TransformLock) {
        .driver = driver,
        .local_offset = local_offset,
        .local_angle = local_angle,
        .lock_position = lock_position,
        .lock_orientation = lock_orientation,
        .inherit_velocity = inherit_velocity
    });
}
void physics_remove_transform_lock(Entity entity) {
    if(!physics_entity_valid(entity)) {
        return;
    }
    if(!entity_is_alive(entity)) {
        //Error
        return;
    }
    entity_delete_components(entity, TRANSFORM_LOCK);
    if(transform_locks_pool.used[entity]) {
        (void)TransformLockPool_release_at(&transform_locks_pool, entity);
    }
}
void physics_set_transform_lock_current_transform(
        Entity driven,
        Entity driver,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
        ) {
    if(!physics_entity_valid(driven) || !physics_entity_valid(driver)) {
        return;
    }
    if(!entity_is_alive(driven) || !entity_is_alive(driver)) {
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
    if(!physics_entity_valid(a) || !physics_entity_valid(b)) {
        return 0;
    }
    if(!entity_is_alive(a) || !entity_is_alive(b)) {
        return 0;
    }

    Entity joint = entity_add();
    if(!physics_entity_valid(joint) || !entity_is_alive(joint)) {
        return 0;
    }

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

    (void)JointPool_store_at(&joints_pool, joint, (Joint){
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
    });

    return joint;
}

void physics_set_collision_report(Entity entity, Entity target, bool state) {
    if(!physics_entity_valid(entity) || !physics_entity_valid(target)) {
        return;
    }
    if(collision_reports_pool.used[entity] == 0) {
        (void)CollisionReportPool_store_at(&collision_reports_pool, entity, (CollisionReport){0});
    }
    collision_reports[entity].collisions[target] = state;
}
bool physics_get_collision_report(Entity entity, Entity target) {
    if(!physics_entity_valid(entity) || !physics_entity_valid(target)) {
        return false;
    }
    if(collision_reports[entity].collisions[target] && collision_reports[target].collisions[entity]) {
        return true;
    }
    return false;
}
