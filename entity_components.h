#ifndef ENTITY_COMPONENTS_H
#define ENTITY_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>

//Entities
typedef uint32_t Entity; //An id for an entity
#define MAX_ENTITIES 1000
extern bool entity_alive[MAX_ENTITIES]; //What entities are active
extern uint32_t entity_mask[MAX_ENTITIES]; //Bit map of the components each entity has
//Enum for the component mask
typedef enum {
    NONE        = 0,
    POSITION    = 1 << 0,
    VELOCITY    = 1 << 1,
    ACCELERATION= 1 << 2,
    FORCE       = 1 << 3,
    MASS        = 1 << 4,
    TIMEWINDOW  = 1 << 5,
} CMask;
static const char* component_names[] = {
    "NONE",
    "POSITION",
    "VELOCITY",
    "ACCELERATION",
    "FORCE",
    "MASS",
    "TIMEWINDOW",
};
extern const int component_count;

//Disasters
typedef enum {FIRE, WATER, GROWTH} DisasterType;
typedef enum {MINOR, MODERATE, URGENT} DisasterSeverity;
extern DisasterType disaster_type[MAX_ENTITIES];
extern DisasterSeverity disaster_serverity[MAX_ENTITIES];

//Moveable Objects
typedef enum {FIXED, MOVEABLE} MotionType;
typedef struct {
    float x;
    float y;
} Vec2D;
typedef Vec2D Position;
typedef Vec2D Velocity;
typedef Vec2D Acceleration;
typedef Vec2D Force;
typedef struct {
    float start;
    float end;
} TimeWindow;
typedef struct {
    Force force;
    TimeWindow time_window;
} TimedForce;
extern MotionType motion_type[MAX_ENTITIES];
extern Position positions[MAX_ENTITIES];
extern Velocity velocities[MAX_ENTITIES];
extern Acceleration accelerations[MAX_ENTITIES];
extern float mass[MAX_ENTITIES];
extern Entity targets[MAX_ENTITIES];
extern Force forces[MAX_ENTITIES];
extern TimeWindow time_windows[MAX_ENTITIES];


Entity add_entity();
void delete_entity(Entity e);

#endif
