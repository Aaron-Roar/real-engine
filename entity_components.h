#ifndef ENTITY_COMPONENTS_H
#define ENTITY_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>
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
    ANIMATED_SPRITE             = 1 << 17,
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

typedef Entity Parent;
typedef Entity Child;
typedef struct Children {
    Entity entities[MAX_ENTITIES];
} Children;
typedef struct LifeTime {
    Time expirey_time;
    Tick expirey_tick;
} LifeTime;

extern Parent parents[MAX_ENTITIES];
extern Children children[MAX_ENTITIES];
extern LifeTime life_times[MAX_ENTITIES];
extern Entity targets[MAX_ENTITIES];
//Target Capable Effects

Entity entity_add();
void entity_delete(Entity entity);
void entity_add_components(Entity entity, CMask mask);
bool entity_has_components(Entity entity, Component components);
void entity_delete_components(Entity entity, CMask mask);
void entity_print_entity_components(Entity entity);
bool entity_equate_mask(CMask e_mask, CMask filter);
void entity_set_child(Entity parent, Entity child);
void entity_set_parent(Entity child, Entity parent);
void entity_remove_parent(Entity child);
void entity_remove_child(Entity parent, Entity child);
Children entity_get_children(Entity entity);
Parent entity_get_parent(Entity entity);
void entity_set_life_time(Entity entity, Time expirey_time, Tick expirey_tick);
void entity_remove_life_time(Entity entity);
#endif
