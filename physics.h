#ifndef PHYSICS_H
#define PHYSICS_H
#include "math2d.h"
#include "entity_components.h"
ERROR_DECLARE_RESULT_TYPE(ShapeResult, Shape);
typedef struct Collision {
    bool overlap;
    Axis normal;
    Vec1D depth;
} Collision;
typedef struct {
    bool collisions[MAX_ENTITIES];
} CollisionReport;
typedef Vec1D Friction;
typedef Vec1D Restitution;

typedef Vec1D Orientation;
typedef Vec2D Position;
typedef Vec2D Velocity;
typedef Orientation AngularVelocity;
typedef Orientation AngularAcceleration;
typedef Vec2D Acceleration;
typedef Vec2D Force;
typedef float Mass;
typedef Orientation Torque;
typedef struct AxisLock {
    Axis axis;
    Position point_on_axis;
} AxisLock;
typedef struct AngleLock {
    Orientation min;
    Orientation max;
} AngleLock;
typedef struct TransformLock {
    Entity driver;

    Vec2D local_offset;
    Orientation local_angle;

    bool lock_position;
    bool lock_orientation;
    bool inherit_velocity;
} TransformLock;

typedef enum JointType {
    JOINT_DISTANCE,
    JOINT_WELD,
    JOINT_PIN
} JointType;

typedef struct Joint {
    JointType type;

    Entity a;
    Entity b;

    Vec2D local_anchor_a;
    Vec2D local_anchor_b;

    float rest_length;

    float stiffness;
    float damping;

    bool lock_angle;
    Orientation rest_angle;
    float angular_stiffness;
    float angular_damping;
} Joint;
MEMORY_DECLARE_OBJECT_POOL(PositionPool, Position);
MEMORY_DECLARE_OBJECT_POOL(VelocityPool, Velocity);
MEMORY_DECLARE_OBJECT_POOL(AccelerationPool, Acceleration);
MEMORY_DECLARE_OBJECT_POOL(MassPool, float);
MEMORY_DECLARE_OBJECT_POOL(ForcePool, Force);
MEMORY_DECLARE_OBJECT_POOL(ShapePool, Shape);
MEMORY_DECLARE_OBJECT_POOL(CollisionReportPool, CollisionReport);
MEMORY_DECLARE_OBJECT_POOL(OrientationPool, Orientation);
MEMORY_DECLARE_OBJECT_POOL(AngularVelocityPool, AngularVelocity);
MEMORY_DECLARE_OBJECT_POOL(AngularAccelerationPool, AngularAcceleration);
MEMORY_DECLARE_OBJECT_POOL(TorquePool, Torque);
MEMORY_DECLARE_OBJECT_POOL(FrictionPool, Friction);
MEMORY_DECLARE_OBJECT_POOL(RestitutionPool, Restitution);
MEMORY_DECLARE_OBJECT_POOL(AngleLockPool, AngleLock);
MEMORY_DECLARE_OBJECT_POOL(AxisLockPool, AxisLock);
MEMORY_DECLARE_OBJECT_POOL(TransformLockPool, TransformLock);
MEMORY_DECLARE_OBJECT_POOL(JointPool, Joint);
extern PositionPool positions_pool;
extern VelocityPool velocities_pool;
extern AccelerationPool accelerations_pool;
extern MassPool mass_pool;
extern ForcePool forces_pool;
extern AccelerationPool force_accelerations_pool;
extern ShapePool hit_boxes_pool;
extern ShapePool world_hit_boxes_pool;
extern CollisionReportPool collision_reports_pool;
extern OrientationPool orientations_pool;
extern AngularVelocityPool angular_velocities_pool;
extern AngularAccelerationPool angular_accelerations_pool;
extern TorquePool torques_pool;
extern AngularVelocityPool torque_angular_accelerations_pool;
extern FrictionPool frictions_pool;
extern RestitutionPool restitutions_pool;
extern AngleLockPool angle_locks_pool;
extern AxisLockPool axis_locks_pool;
extern TransformLockPool transform_locks_pool;
extern JointPool joints_pool;
#define positions positions_pool.objects
#define velocities velocities_pool.objects
#define accelerations accelerations_pool.objects
#define mass mass_pool.objects
#define forces forces_pool.objects
#define force_accelerations force_accelerations_pool.objects
#define hit_boxes hit_boxes_pool.objects
#define world_hit_boxes world_hit_boxes_pool.objects
#define collision_reports collision_reports_pool.objects
#define orientations orientations_pool.objects
#define angular_velocities angular_velocities_pool.objects
#define angular_accelerations angular_accelerations_pool.objects
#define torques torques_pool.objects
#define torque_angular_accelerations torque_angular_accelerations_pool.objects
#define frictions frictions_pool.objects
#define restitutions restitutions_pool.objects
#define angle_locks angle_locks_pool.objects
#define axis_locks axis_locks_pool.objects
#define transform_locks transform_locks_pool.objects
#define joints joints_pool.objects
Shape physics_shape_world_translate(Shape shape, Position position, Orientation angle);
float physics_polygon_moment_of_inertia(Shape shape, Mass mass_value);
Collision physics_sat_collision(Shape shape_1, Shape shape_2);
Vec1D physics_circle_moment_of_inertia(Shape circle, Mass mass_value);
EngineResult physics_set_acceleration(Entity entity, Acceleration a);
EngineResult physics_set_velocity(Entity entity, Velocity v);
EngineResult physics_set_position(Entity entity, Position p);
EngineResult physics_set_mass(Entity entity, Mass m);
EntityResult physics_set_force(Entity entity, Force f);
EntityResult physics_set_torque(Entity entity, Torque t);
EngineResult physics_set_hitbox(Entity entity, Shape hitbox);
EngineResult physics_set_orientation(Entity entity, Orientation angle);
EngineResult physics_set_angular_velocity(Entity entity, AngularVelocity v);
ShapeResult physics_get_global_hit_box(Entity entity);
EngineResult physics_set_restitution(Entity entity, Restitution restitution);
EngineResult physics_set_dynamic(Entity entity);
EngineResult physics_set_static(Entity entity);
EngineResult physics_set_angle_lock(Entity entity, Orientation min, Orientation max);
EngineResult physics_set_axis_lock(Entity entity, Axis axis, Position axis_point);
EngineResult physics_set_friction(Entity entity, float friction);
EngineResult physics_set_transform_lock(
        Entity driven,
        Entity driver,
        Vec2D local_offset,
        Orientation local_angle,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
);
EngineResult physics_remove_transform_lock(Entity entity);
EngineResult physics_set_transform_lock_current_transform(
        Entity driven,
        Entity driver,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
);
EntityResult physics_set_joint(
    Entity a,
    Entity b,
    JointType type,
    Vec2D local_anchor_a,
    Vec2D local_anchor_b,
    float stiffness,
    float damping
);
Collision physics_particle_collision(Shape shape_1, Shape shape_2);
EngineResult physics_set_collision_report(Entity entity, Entity target, bool state);
bool physics_get_collision_report(Entity entity, Entity target);
#endif
