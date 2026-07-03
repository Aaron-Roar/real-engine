#ifndef ENTITY_COMPONENTS_H
#define ENTITY_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>
#include "math2d.h"


//Entities
typedef uint32_t Entity; //An id for an entity
#define MAX_ENTITIES 100
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
typedef struct {
    float start;
    float end;
} TimeWindow;
typedef struct {
    Force force;
    TimeWindow time_window;
} TimedForce;
extern const int component_count;
typedef struct AxisLock {
    Axis axis;
    Position point_on_axis;
} AxisLock;

typedef struct AngleLock {
    Orientation min;
    Orientation max;
} AngleLock;

extern Position positions[MAX_ENTITIES];
extern Velocity velocities[MAX_ENTITIES];
extern Acceleration accelerations[MAX_ENTITIES];
extern float mass[MAX_ENTITIES];
extern Entity targets[MAX_ENTITIES];
extern Force forces[MAX_ENTITIES];
extern Acceleration force_accelerations[MAX_ENTITIES];
extern TimeWindow time_windows[MAX_ENTITIES];
extern Shape hit_boxes[MAX_ENTITIES];
extern Orientation orientations[MAX_ENTITIES];
extern AngularVelocity angular_velocities[MAX_ENTITIES];
extern AngularAcceleration angular_accelerations[MAX_ENTITIES];
extern Torque torques[MAX_ENTITIES];
extern AngularVelocity torque_angular_accelerations[MAX_ENTITIES];
extern Friction frictions[MAX_ENTITIES];
extern Restitution restitutions[MAX_ENTITIES];
extern AngleLock angle_locks[MAX_ENTITIES];
extern AxisLock axis_locks[MAX_ENTITIES];
//Target Capable Effects

Entity add_entity();
void delete_entity(Entity entity);
void add_components(Entity entity, CMask mask);
void delete_components(Entity entity, CMask mask);
void print_entity_components(Entity entity);
void set_acceleration(Entity entity, Acceleration a);
void set_velocity(Entity entity, Velocity v);
void set_position(Entity entity, Position p);
void set_mass(Entity entity, Mass m);
void set_force(Entity entity, Force f);
void set_torque(Entity entity, Torque t);
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

#endif
