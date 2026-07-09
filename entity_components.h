#ifndef ENTITY_COMPONENTS_H
#define ENTITY_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>
#include "math2d.h"
#include <time.h>
#include "engine.h"


//Entities
typedef uint32_t Entity; //An id for an entity
#define MAX_ENTITIES 10000
#define MAX_COMPONENTS 100
extern bool entity_alive[MAX_ENTITIES]; //What entities are active
typedef struct EntityList {
    uint32_t entity_amount;
    Entity concerned_entities[MAX_ENTITIES];
} EntityList;
typedef uint32_t CMask; //The bit mask for an entities components
extern CMask entity_mask[MAX_ENTITIES]; //Bit map of the components each entity has
//Enum for the component mask
typedef enum {
    NONE                    = 0,
    STATIC                  = 1 << 0,
    DYNAMIC                 = 1 << 1,
    COLLISION               = 1 << 2,
    HIT_BOX                 = 1 << 3,
    AXIS_LOCK               = 1 << 4,
    ANGLE_LOCK              = 1 << 5,

    FORCE                   = 1 << 6,
    TORQUE                  = 1 << 7,
    TARGETABLE              = 1 << 8,

    MASS                    = 1 << 9,
    TIMEWINDOW              = 1 << 10,
    HAS_PARENT                  = 1 << 11,
    HAS_CHILDREN                = 1 << 12,
    TRANSFORM_LOCK              = 1 << 13,
    JOINT                       = 1 << 14,
    LIFETIME                    = 1 << 15,
    PARTICLE                    = 1 << 16,
} Component;


static const char* component_names[] = {
    "NONE",
    "POSITION",
    "VELOCITY",
    "ACCELERATION",
    "FORCE",
    "MASS",
    "TIMEWINDOW",
    "MOVEABLE",
    "TARGETABLE",
    "COLLISION",
};
extern const int component_count;
typedef struct AxisLock {
    Axis axis;
    Position point_on_axis;
} AxisLock;

typedef struct AngleLock {
    Orientation min;
    Orientation max;
} AngleLock;

typedef Entity Parent;
typedef Entity Child;
typedef struct Children {
    Entity entities[MAX_ENTITIES];
} Children;
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
typedef struct LifeTime {
    Time expirey_time;
    Tick expirey_tick;
} LifeTime;

extern Position positions[MAX_ENTITIES];
extern Velocity velocities[MAX_ENTITIES];
extern Acceleration accelerations[MAX_ENTITIES];
extern float mass[MAX_ENTITIES];
extern Entity targets[MAX_ENTITIES];
extern Force forces[MAX_ENTITIES];
extern Acceleration force_accelerations[MAX_ENTITIES];
extern Shape hit_boxes[MAX_ENTITIES];
extern Shape world_hit_boxes[MAX_ENTITIES];
extern Orientation orientations[MAX_ENTITIES];
extern AngularVelocity angular_velocities[MAX_ENTITIES];
extern AngularAcceleration angular_accelerations[MAX_ENTITIES];
extern Torque torques[MAX_ENTITIES];
extern AngularVelocity torque_angular_accelerations[MAX_ENTITIES];
extern Friction frictions[MAX_ENTITIES];
extern Restitution restitutions[MAX_ENTITIES];
extern AngleLock angle_locks[MAX_ENTITIES];
extern AxisLock axis_locks[MAX_ENTITIES];
extern Parent parents[MAX_ENTITIES];
extern Children children[MAX_ENTITIES];
extern TransformLock transform_locks[MAX_ENTITIES];
extern Joint joints[MAX_ENTITIES];
extern LifeTime life_times[MAX_ENTITIES];
//Target Capable Effects

Entity add_entity();
void delete_entity(Entity entity);
void add_components(Entity entity, CMask mask);
bool has_components(Entity entity, Component components);
void delete_components(Entity entity, CMask mask);
void print_entity_components(Entity entity);
void set_acceleration(Entity entity, Acceleration a);
void set_velocity(Entity entity, Velocity v);
void set_position(Entity entity, Position p);
void set_mass(Entity entity, Mass m);
Entity set_force(Entity entity, Force f);
Entity set_torque(Entity entity, Torque t);
void print_alive_entities();
bool equate_mask(CMask e_mask, CMask filter);
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
void set_child(Entity parent, Entity child);
void set_parent(Entity child, Entity parent);
void remove_parent(Entity child);
void remove_child(Entity parent, Entity child);
Children get_children(Entity entity);
Parent get_parent(Entity entity);
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
void set_life_time(Entity entity, Time expirey_time, Tick expirey_tick);
void remove_life_time(Entity entity);
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
#endif
