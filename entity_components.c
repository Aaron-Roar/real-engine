#include "entity_components.h"
#include "error.h"
#include "console.h"
#include "tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>


bool entity_alive[MAX_ENTITIES] = {0}; //What entities are active
uint32_t entity_mask[MAX_ENTITIES] = {0}; //Bit map of the components each entity has

//Moveable Objects
Position positions[MAX_ENTITIES] = {0};
Orientation orientations[MAX_ENTITIES] = {0};
Velocity velocities[MAX_ENTITIES] = {0};
Acceleration accelerations[MAX_ENTITIES] = {0};
float mass[MAX_ENTITIES] = {0};
Entity targets[MAX_ENTITIES] = {0};
Force forces[MAX_ENTITIES] = {0};
TimeWindow time_windows[MAX_ENTITIES] = {0};
Shape hit_boxes[MAX_ENTITIES] = {0};
AngularVelocity angular_velocities[MAX_ENTITIES] = {0};
AngularAcceleration angular_accelerations[MAX_ENTITIES] = {0};
Torque torques[MAX_ENTITIES] = {0};
Friction frictions[MAX_ENTITIES] = {0};
Restitution restitutions[MAX_ENTITIES] = {0};

uint32_t entity_counter = 1; //Temporary solution. Leaks memory on entity deletion
Entity add_entity() {
    Error error = {0};

    if(entity_counter >= MAX_ENTITIES) {
        error.code |= FAILED_ADD_ENTITY | ENTITY_RANGE_EXCEEDED;
        error_print(error);
        return 0; //Unused location
    }

    entity_alive[entity_counter] = 1;
    entity_counter += 1;
    return entity_counter - 1;
}

void delete_entity(Entity entity) {
    Error error = {0};

    if(entity_alive[entity] == 0) {
        error.code |= FAILED_DELETE_ENTITY | ENTITY_DOES_NOT_EXIST;
        error_add_entity(&error, entity);
        error_print(error);
    }
    entity_alive[entity] = 0;
    entity_mask[entity] = 0;
}

void add_components(Entity entity, CMask mask) {
    Error error = {0};

    if(entity_alive[entity] == 0) {
        error.code |= FAILED_ADD_COMPONENTS | ENTITY_DOES_NOT_EXIST;
        error_add_entity(&error, entity);
        error_print(error);
    }
    entity_mask[entity] |= mask;
}

void delete_components(Entity entity, CMask mask) {
    Error error = {0};

    if(entity_alive[entity] == 0) {
        error.code |= FAILED_DELETE_COMPONENTS | ENTITY_DOES_NOT_EXIST;
        error_add_entity(&error, entity);
        error_print(error);
    }
    entity_mask[entity] &= ~mask;
}

void print_entity_components(Entity entity) {
    console_write(LOG_ENGINE, "Entity: %d\n", entity);
        if(entity_mask[entity] & NONE) {
            console_write(LOG_ENGINE, "NONE\n");
        }
        if(entity_mask[entity] & POSITION) {
            console_write(LOG_ENGINE, "POSITION\n");
        }
        if(entity_mask[entity] & VELOCITY) {
            console_write(LOG_ENGINE, "VELOCITY\n");
        }
        if(entity_mask[entity] & ACCELERATION) {
            console_write(LOG_ENGINE, "ACCELERATION\n");
        }
        if(entity_mask[entity] & FORCE) {
            console_write(LOG_ENGINE, "FORCE\n");
        }
        if(entity_mask[entity] & MASS) {
            console_write(LOG_ENGINE, "MASS\n");
        }
        if(entity_mask[entity] & TIMEWINDOW) {
            console_write(LOG_ENGINE, "TIMEWINDOW\n");
        }
        if(entity_mask[entity] & MOVEABLE) {
            console_write(LOG_ENGINE, "MOVEABLE\n");
        }
}

void set_acceleration(Entity entity, Acceleration a) {
    entity_mask[entity] |= MOVEABLE;
    accelerations[entity] = a;
}
void set_velocity(Entity entity, Velocity v) {
    entity_mask[entity] |= MOVEABLE;
    velocities[entity] = v;
}
void set_position(Entity entity, Position p) {
    entity_mask[entity] |= POSITION;
    positions[entity] = p;
}

void set_mass(Entity entity, Mass m) {
    entity_mask[entity] |= MASS;
    mass[entity] = m;
}

//Applies a force to a target
void set_force(Entity entity, Force f) {
    Entity force_entity = add_entity();
    forces[force_entity] = f;
    targets[force_entity] = entity;
    entity_mask[force_entity] |= TARGETABLE | FORCE;

    entity_mask[entity] |= MOVEABLE;
}

void set_torque(Entity entity, Torque t) {
    Entity torque_entity = add_entity();
    torques[torque_entity] = t;
    targets[torque_entity] = entity;
    entity_mask[torque_entity] |= TARGETABLE | TORQUE;

    entity_mask[entity] |= MOVEABLE;
}

void set_angular_velocity(Entity entity, AngularVelocity v) {
    entity_mask[entity] |= MOVEABLE;
    angular_velocities[entity] = v;
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

void set_hitbox(Entity entity, Shape hitbox) {
    hit_boxes[entity] = hitbox;
    entity_mask[entity] |= COLLISION | HIT_BOX;
}
void set_orientation(Entity entity, Orientation angle) {
  orientations[entity] = angle;
}

Shape get_global_hit_box(Entity entity) {
    CMask filter = HIT_BOX;
    if(entity_alive[entity]) {
        if( (entity_mask[entity] & filter) == filter ) {
            Position pos = positions[entity];
            Orientation ort = orientations[entity];
            Shape hit_box = hit_boxes[entity];
            return shape_world_translate(hit_box, pos, ort);
        }
    }
    return (Shape){0};
}
 void set_restitution(Entity entity, Restitution restitution) {

 }
