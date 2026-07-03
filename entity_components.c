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
Acceleration force_accelerations[MAX_ENTITIES] = {0};
float mass[MAX_ENTITIES] = {0};
Entity targets[MAX_ENTITIES] = {0};
Force forces[MAX_ENTITIES] = {0};
TimeWindow time_windows[MAX_ENTITIES] = {0};
Shape hit_boxes[MAX_ENTITIES] = {0};
AngularVelocity angular_velocities[MAX_ENTITIES] = {0};
AngularAcceleration angular_accelerations[MAX_ENTITIES] = {0};
AngularVelocity torque_angular_accelerations[MAX_ENTITIES] = {0};
Torque torques[MAX_ENTITIES] = {0};
Friction frictions[MAX_ENTITIES] = {0};
Restitution restitutions[MAX_ENTITIES] = {0};
AngleLock angle_locks[MAX_ENTITIES] = {0};
AxisLock axis_locks[MAX_ENTITIES] = {0};
Parent parents[MAX_ENTITIES] = {0};
Children children[MAX_ENTITIES] = {0};

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
}

void set_static(Entity entity) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    add_components(entity, STATIC);
    entity_mask[entity] &= ~DYNAMIC;
    console_debug_write(LOG_ENGINE, "Set Entity: %d to DYNAMIC\n", entity);
}

void set_dynamic(Entity entity) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    add_components(entity, DYNAMIC);
    entity_mask[entity] &= ~STATIC;
    console_debug_write(LOG_ENGINE, "Set Entity: %d to STATIC\n", entity);
}

void set_acceleration(Entity entity, Acceleration a) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    accelerations[entity] = a;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Acceleration: {x: %f, y: %f}\n", entity, a.x, a.y);
}
void set_velocity(Entity entity, Velocity v) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    velocities[entity] = v;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Velocity: {x: %f, y: %f}\n", entity, v.x, v.y);
}
void set_position(Entity entity, Position p) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    positions[entity] = p;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Position: {x: %f, y: %f}\n", entity, p.x, p.y);
}

void set_mass(Entity entity, Mass m) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_mask[entity] |= MASS;
    mass[entity] = m;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Mass: %f\n", entity, m);
}


//Applies a force to a target
void set_force(Entity entity, Force f) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    set_dynamic(entity);
    Entity force_entity = add_entity();
    forces[force_entity] = f;
    targets[force_entity] = entity;
    entity_mask[force_entity] |= TARGETABLE | FORCE;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Force: {x: %f, y: %f}\n", entity, f.x, f.y);
}

void set_torque(Entity entity, Torque t) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    set_dynamic(entity);
    Entity torque_entity = add_entity();
    torques[torque_entity] = t;
    targets[torque_entity] = entity;
    entity_mask[torque_entity] |= TARGETABLE | TORQUE;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Torque: %f\n", entity, t);
}

void set_angular_velocity(Entity entity, AngularVelocity v) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    set_dynamic(entity);
    angular_velocities[entity] = v;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Angular Velocity: %f\n", entity, v);
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
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    entity_mask[entity] |= COLLISION | HIT_BOX;
    hit_boxes[entity] = hitbox;
  console_debug_write(LOG_ENGINE, "Set Entity: %d to have a hit box\n", entity);
}
void set_orientation(Entity entity, Orientation angle) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
  orientations[entity] = angle;
  console_debug_write(LOG_ENGINE, "Set Entity: %d Orientation: %f\n", entity, angle);
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
    if(!entity_alive[entity]) {
        //Error
        return;
    }
     if(restitution < 0) {
        restitutions[entity] = 0;
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, 0);
     }
     else if(restitution > 1) {
        restitutions[entity] = 1;
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, 1);
     }
     else {
        restitutions[entity] = restitution;
        console_debug_write(LOG_ENGINE, "Set Entity: %d Restitution: %f\n", entity, restitution);
     }
     add_components(entity, COLLISION);
 }

void set_axis_lock(Entity entity, Axis axis, Position axis_point) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    add_components(entity, AXIS_LOCK);
    Axis normalized_axis = normalize_vector(axis);
    axis_locks[entity] = (AxisLock){
        .axis = (Axis){
            .x = normalized_axis.x,
            .y = normalized_axis.y
        },
        .point_on_axis = (Position){
            .x = axis_point.x,
            .y = axis_point.y
        }
    };
}

void set_angle_lock(Entity entity, Orientation min, Orientation max) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    add_components(entity, ANGLE_LOCK);
    angle_locks[entity] = (AngleLock){
        .min = min,
        .max = max
    };
}

void set_friction(Entity entity, float friction) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    if(friction < 0) {
        frictions[entity] = 0;
    }
    else if(friction >= 0) {
        frictions[entity] = friction;
    }
}

void set_child(Entity parent, Entity child) {
    if(!entity_alive[parent] || !entity_alive[child]) {
        //Error parent or child not alive 
        return;
    }
    add_components(parent, HAS_CHILDREN);
    add_components(child, HAS_PARENT);
    children[parent].entities[child] = child;
    parents[child] = parent;
}

void set_parent(Entity child, Entity parent) {
    if(!entity_alive[parent] || !entity_alive[child]) {
        //Error parent or child not alive 
        return;
    }
    add_components(parent, HAS_CHILDREN);
    add_components(child, HAS_PARENT);
    children[parent].entities[child] = child;
    parents[child] = parent;
}

void remove_parent(Entity child) {
    if(!entity_alive[child]) {
        //Error
        return;
    }
    if(!entity_alive[parents[child]]) {
        //Warning no parent is currently set
    }
    //Removing this child from the parent
    children[parents[child]].entities[child] = 0;
    //If the parent has no more children remove the flag
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(children[parents[child]].entities[i] != 0) {
            delete_components(parents[child], HAS_CHILDREN);
        }
    }

    //Remove the parent from the child
    parents[child] = 0;
    delete_components(child, HAS_PARENT);
}

void remove_child(Entity parent, Entity child) {
    //Removing the child from the parent
    children[parent].entities[child] = 0;
    //If the parent has no more children remove the flag
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(children[parent].entities[i] != 0) {
            delete_components(parents[child], HAS_CHILDREN);
        }
    }

    //Removing the parent from the child
    parents[child] = 0;
    delete_components(child, HAS_PARENT);

}

Children get_children(Entity entity) {
    return children[entity];
}
Entity get_parent(Entity entity) {
    return parents[entity];
}
