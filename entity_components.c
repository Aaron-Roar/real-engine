#include "entity_components.h"
#include "error.h"
#include "console.h"
#include "tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define PI_F 3.14159265358979323846f



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
Shape hit_boxes[MAX_ENTITIES] = {0};

uint32_t entity_counter = 1; //Temporary solution. Leaks memory on entity deletion
Entity add_entity() {
    Error error = {0};

    if(entity_counter >= MAX_ENTITIES) {
        error.code |= FAILED_ADD_ENTITY | ENTITY_RANGE_EXCEEDED;
        error_print(error);
        return 0; //Unused location
    }

    entity_counter += 1;
    entity_alive[entity_counter] = 1;
    return entity_counter;
}

void delete_entity(Entity e) {
    Error error;

    if(entity_alive[e] == 0) {
        error.code |= FAILED_DELETE_ENTITY | ENTITY_DOES_NOT_EXIST;
        error_add_entity(&error, e);
        error_print(error);
    }
    entity_alive[e] = 0;
    entity_mask[e] = 0;
}

void add_components(Entity e, CMask mask) {
    Error error;

    if(entity_alive[e] == 0) {
        error.code |= FAILED_ADD_COMPONENTS | ENTITY_DOES_NOT_EXIST;
        error_add_entity(&error, e);
        error_print(error);
    }
    entity_mask[e] |= mask;
}

void delete_components(Entity e, CMask mask) {
    Error error;

    if(entity_alive[e] == 0) {
        error.code |= FAILED_DELETE_COMPONENTS | ENTITY_DOES_NOT_EXIST;
        error_add_entity(&error, e);
        error_print(error);
    }
    entity_mask[e] &= ~mask;
}

void print_entity_components(Entity e) {
    console_write(LOG_ENGINE, "Entity: %d\n", e);
        if(entity_mask[e] & NONE) {
            console_write(LOG_ENGINE, "NONE\n");
        }
        if(entity_mask[e] & POSITION) {
            console_write(LOG_ENGINE, "POSITION\n");
        }
        if(entity_mask[e] & VELOCITY) {
            console_write(LOG_ENGINE, "VELOCITY\n");
        }
        if(entity_mask[e] & ACCELERATION) {
            console_write(LOG_ENGINE, "ACCELERATION\n");
        }
        if(entity_mask[e] & FORCE) {
            console_write(LOG_ENGINE, "FORCE\n");
        }
        if(entity_mask[e] & MASS) {
            console_write(LOG_ENGINE, "MASS\n");
        }
        if(entity_mask[e] & TIMEWINDOW) {
            console_write(LOG_ENGINE, "TIMEWINDOW\n");
        }
        if(entity_mask[e] & MOVEABLE) {
            console_write(LOG_ENGINE, "MOVEABLE\n");
        }
}

void set_acceleration(Entity e, Acceleration a) {
    entity_mask[e] |= MOVEABLE;
    accelerations[e] = a;
}
void set_velocity(Entity e, Velocity v) {
    entity_mask[e] |= MOVEABLE;
    velocities[e] = v;
}
void set_position(Entity e, Position p) {
    entity_mask[e] |= POSITION;
    positions[e] = p;
}

void set_mass(Entity e, Mass m) {
    entity_mask[e] |= MASS;
    mass[e] = m;
}

//Applies a force to a target
void set_force(Entity e, Force f) {
    Entity force_entity = add_entity();
    forces[force_entity] = f;
    targets[force_entity] = e;
    entity_mask[force_entity] |= TARGETABLE | FORCE;

    entity_mask[e] |= MOVEABLE;
}

void print_alive_entities() {
    console_write(LOG_ENGINE, "AliveEntities: {");
    for(int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            console_write(LOG_ENGINE,"%d ", i);
        }
    }
    console_write(LOG_ENGINE, "}\n");
}

void set_hitbox(Entity e, Shape hitbox) {
    hit_boxes[e] = hitbox;
    entity_mask[e] |= COLLISION;
}

Shape square(float width, float height) {
    Shape shape = {
        .vertex_amount = 4,
        .vertices = {
            {.x = width/2, .y = height/2},
            {.x = width/2, .y = -height/2},
            {.x = -width/2, .y = -height/2},
            {.x = -width/2, .y = height/2},
        }
    };
    return shape;
}

Shape circle(float radius, uint8_t verticies) {
    Shape shape = {0};
    if(verticies < MIN_VERTICIES) {
        shape.vertex_amount = MIN_VERTICIES;
    }
    else if(verticies > MAX_VERTICIES) {
        shape.vertex_amount = MAX_VERTICIES;
    }
    else {
        shape.vertex_amount = verticies;
    }

    float angle_increment = ((float)2*PI_F)/((float)shape.vertex_amount);
    for(int i = 0; i < shape.vertex_amount; i++) {
        shape.vertices[i].x = cosf(angle_increment*i)*radius;
        shape.vertices[i].y = sinf(angle_increment*i)*radius;
    }
    return shape;
}
