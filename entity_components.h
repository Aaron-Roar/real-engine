#ifndef ENTITY_COMPONENTS_H
#define ENTITY_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>
#include "math2d.h"


//Entities
typedef uint32_t Entity; //An id for an entity
#define MAX_ENTITIES 30
#define MAX_COMPONENTS 30
extern bool entity_alive[MAX_ENTITIES]; //What entities are active
typedef struct EntityList {
    uint32_t entity_amount;
    Entity concerned_entities[MAX_ENTITIES];
} EntityList;
typedef uint32_t CMask; //The bit mask for an entities components
extern CMask entity_mask[MAX_ENTITIES]; //Bit map of the components each entity has
//Enum for the component mask
typedef enum {
    NONE        = 0,
    POSITION    = 1 << 0,
    VELOCITY    = 1 << 1,
    ACCELERATION= 1 << 2,
    FORCE       = 1 << 3,
    MASS        = 1 << 4,
    TIMEWINDOW  = 1 << 5,
    MOVEABLE    = 1 << 6,
    TARGETABLE  = 1 << 7,
    COLLISION   = 1 << 8,
    TORQUE      = 1 << 9,
    HIT_BOX     = 1 << 10,
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
//Disasters

extern Position positions[MAX_ENTITIES];
extern Velocity velocities[MAX_ENTITIES];
extern Acceleration accelerations[MAX_ENTITIES];
extern float mass[MAX_ENTITIES];
extern Entity targets[MAX_ENTITIES];
extern Force forces[MAX_ENTITIES];
extern TimeWindow time_windows[MAX_ENTITIES];
extern Shape hit_boxes[MAX_ENTITIES];
extern Orientation orientations[MAX_ENTITIES];
extern AngularVelocity angular_velocities[MAX_ENTITIES];
extern AngularAcceleration angular_accelerations[MAX_ENTITIES];
extern Torque torques[MAX_ENTITIES];
extern Friction frictions[MAX_ENTITIES];
extern Restitution restitutions[MAX_ENTITIES];
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
#endif
