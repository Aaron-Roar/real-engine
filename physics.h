#ifndef PHYSICS_H
#define PHYSICS_H
#include "math2d.h"
#include "entity_components.h"
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
extern Position positions[MAX_ENTITIES];
extern Velocity velocities[MAX_ENTITIES];
extern Acceleration accelerations[MAX_ENTITIES];
extern float mass[MAX_ENTITIES];
extern Force forces[MAX_ENTITIES];
extern Acceleration force_accelerations[MAX_ENTITIES];
extern Shape hit_boxes[MAX_ENTITIES];
extern Shape world_hit_boxes[MAX_ENTITIES];
extern CollisionReport collision_reports[MAX_ENTITIES];
extern Orientation orientations[MAX_ENTITIES];
extern AngularVelocity angular_velocities[MAX_ENTITIES];
extern AngularAcceleration angular_accelerations[MAX_ENTITIES];
extern Torque torques[MAX_ENTITIES];
extern AngularVelocity torque_angular_accelerations[MAX_ENTITIES];
extern Friction frictions[MAX_ENTITIES];
extern Restitution restitutions[MAX_ENTITIES];
extern AngleLock angle_locks[MAX_ENTITIES];
extern AxisLock axis_locks[MAX_ENTITIES];
extern TransformLock transform_locks[MAX_ENTITIES];
extern Joint joints[MAX_ENTITIES];
//Math2d
Shape shape_world_translate(Shape shape, Position position, Orientation angle);
float polygon_moment_of_inertia(Shape shape, Mass mass);
Collision sat_collision(Shape shape_1, Shape shape_2);
Position approximate_contact_point(Position p1, Position p2);
Position polygon_centroid(Shape shape);
Vec1D circle_moment_of_inertia(Shape circle, Mass mass);

//Entity

void set_acceleration(Entity entity, Acceleration a);
void set_velocity(Entity entity, Velocity v);
void set_position(Entity entity, Position p);
void set_mass(Entity entity, Mass m);
Entity set_force(Entity entity, Force f);
Entity set_torque(Entity entity, Torque t);
void set_hitbox(Entity entity, Shape hitbox);
void set_orientation(Entity entity, Orientation angle);
void set_angular_velocity(Entity entity, AngularVelocity v);
Shape get_global_hit_box(Entity entity);
void set_restitution(Entity entity, Restitution restitution);
void set_dynamic(Entity entity);
void set_static(Entity entity);
void set_angle_lock(Entity entity, Orientation min, Orientation max);
void set_axis_lock(Entity entity, Axis axis, Position axis_point);
void set_friction(Entity entity, float friction);
void set_transform_lock(
        Entity driven,
        Entity driver, 
        Vec2D local_offset,
        Orientation local_angle,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
);
void remove_transform_lock(Entity entity);
void set_transform_lock_current_transform(
        Entity driven,
        Entity driver,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
);
Entity set_joint(
    Entity a,
    Entity b,
    JointType type,
    Vec2D local_anchor_a,
    Vec2D local_anchor_b,
    float stiffness,
    float damping
);
Collision particle_collision(Shape shape_1, Shape shape_2);
void set_collision_report(Entity entity, Entity target, bool state);
bool get_collision_report(Entity entity, Entity target);
#endif
