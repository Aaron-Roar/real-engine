#include "entity_components.h"
#include <stdbool.h>
#include <stdio.h>
#include "tools.h"




bool entity_alive[MAX_ENTITIES] = {0}; //What entities are active
uint32_t entity_mask[MAX_ENTITIES] = {0}; //Bit map of the components each entity has

//Disasters
DisasterType disaster_type[MAX_ENTITIES] = {0};
DisasterSeverity disaster_serverity[MAX_ENTITIES] = {0};

//Moveable Objects
Position positions[MAX_ENTITIES] = {0};
Velocity velocities[MAX_ENTITIES] = {0};
Acceleration accelerations[MAX_ENTITIES] = {0};
float mass[MAX_ENTITIES] = {0};
Entity targets[MAX_ENTITIES] = {0};
Force forces[MAX_ENTITIES] = {0};
TimeWindow time_windows[MAX_ENTITIES] = {0};

uint32_t entity_counter = 1; //Temporary solution. Leaks memory on entity deletion
Entity add_entity() {
    if(entity_counter >= MAX_ENTITIES) {
        print_error(FAILED_ADD_COMPONENTS | ENTITY_RANGE_EXCEEDED);
        return 0; //Unused location
    }

    entity_counter += 1;
    entity_alive[entity_counter] = 1;
    return entity_counter;
}

void delete_entity(Entity e) {
    if(entity_alive[e] == 0) {
        print_error(FAILED_DELETE_COMPONENTS | ENTITY_DOES_NOT_EXIST);
    }
    entity_alive[e] = 0;
    entity_mask[e] = 0;
}

void add_components(Entity e, CMask mask) {
    if(entity_alive[e] == 0) {
        print_error(FAILED_ADD_COMPONENTS | ENTITY_DOES_NOT_EXIST);
    }
    entity_mask[e] |= mask;
}

void delete_components(Entity e, CMask mask) {
    if(entity_alive[e] == 0) {
        print_error(FAILED_DELETE_COMPONENTS | ENTITY_DOES_NOT_EXIST);
    }
    entity_mask[e] &= ~mask;
}

void print_entity_components(Entity e) {
    printf("Entity: %d\n", e);
        if(entity_mask[e] & NONE) {
            printf("NONE\n");
        }
        if(entity_mask[e] & POSITION) {
            printf("POSITION\n");
        }
        if(entity_mask[e] & VELOCITY) {
            printf("VELOCITY\n");
        }
        if(entity_mask[e] & ACCELERATION) {
            printf("ACCELERATION\n");
        }
        if(entity_mask[e] & FORCE) {
            printf("FORCE\n");
        }
        if(entity_mask[e] & MASS) {
            printf("MASS\n");
        }
        if(entity_mask[e] & TIMEWINDOW) {
            printf("TIMEWINDOW\n");
        }
        if(entity_mask[e] & MOVEABLE) {
            printf("MOVEABLE\n");
        }
}

void set_acceleration(Entity e, Acceleration a) {
    entity_mask[e] |= ACCELERATION | VELOCITY | POSITION | MOVEABLE;
    accelerations[e] = a;
}
void set_velocity(Entity e, Velocity v) {
    entity_mask[e] |= VELOCITY | POSITION | MOVEABLE;
    velocities[e] = v;
}
void set_position(Entity e, Position p) {
    entity_mask[e] |= POSITION | MOVEABLE;
    positions[e] = p;
}

void set_mass(Entity e, Mass m) {
    entity_mask[e] |= MASS;
    mass[e] = m;
}

//Will work in future by appling forces to a target like in unreal
void set_force(Entity e, Force f) {
    entity_mask[e] |= ACCELERATION | VELOCITY | POSITION | MOVEABLE;
    forces[e] = f;
}
