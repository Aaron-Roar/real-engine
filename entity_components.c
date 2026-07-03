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
TransformLock transform_locks[MAX_ENTITIES] = {0};
Joint joints[MAX_ENTITIES] = {0};
LifeTime life_times[MAX_ENTITIES] = {0};

uint32_t entity_counter = 0; //Temporary solution. Leaks memory on entity deletion
Entity add_entity() {
    Error error = {0};

    if(entity_counter >= MAX_ENTITIES) {
        error.code |= FAILED_ADD_ENTITY | ENTITY_RANGE_EXCEEDED;
        error_print(error);
        return 0; //Unused location
    }

    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(!entity_alive[i]) {
            entity_counter += 1;
            entity_alive[i] = 1;
            return i;
        }
    }

    //All entities filled
    return 0;
}

void clear_entity(Entity entity) {
    entity_alive[entity] = 0;
    entity_mask[entity] = 0;
    positions[entity] = (Position){0};
    orientations[entity] = (Orientation){0};
    velocities[entity] = (Velocity){0};
    accelerations[entity] = (Acceleration){0};
    force_accelerations[entity] = (Acceleration){0};
    mass[entity] = (Mass){0};
    targets[entity] = (Entity){0};
    forces[entity] = (Force){0};
    hit_boxes[entity] = (Shape){0};
    angular_velocities[entity] = (AngularVelocity){0};
    angular_accelerations[entity] = (AngularAcceleration){0};
    torque_angular_accelerations[entity] = (AngularAcceleration){0};
    torques[entity] = (Torque){0};
    frictions[entity] = (Friction){0};
    restitutions[entity] = (Restitution){0};
    angle_locks[entity] = (AngleLock){0};
    axis_locks[entity] = (AxisLock){0};
    parents[entity] = (Parent){0};
    children[entity] = (Children){0};
    transform_locks[entity] = (TransformLock){0};
    joints[entity] = (Joint){0};
    life_times[entity] = (LifeTime){0};
}

void delete_entity(Entity entity) {
    Error error = {0};

    if(entity_alive[entity] == 0) {
        error.code |= FAILED_DELETE_ENTITY | ENTITY_DOES_NOT_EXIST;
        error_add_entity(&error, entity);
        error_print(error);
    }
    clear_entity(entity);

    entity_counter -= 1;
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
    delete_components(entity, DYNAMIC);
    console_debug_write(LOG_ENGINE, "Set Entity: %d to DYNAMIC\n", entity);
}

void set_dynamic(Entity entity) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    add_components(entity, DYNAMIC);
    delete_components(entity, STATIC);
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
Entity set_force(Entity entity, Force f) {
    if(!entity_alive[entity]) {
        //Error
        return 0;
    }
    set_dynamic(entity);
    Entity force_entity = add_entity();
    forces[force_entity] = f;
    targets[force_entity] = entity;
    entity_mask[force_entity] |= TARGETABLE | FORCE;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Force: {x: %f, y: %f}\n", entity, f.x, f.y);
    return force_entity;
}

Entity set_torque(Entity entity, Torque t) {
    if(!entity_alive[entity]) {
        //Error
        return 0;
    }
    set_dynamic(entity);
    Entity torque_entity = add_entity();
    torques[torque_entity] = t;
    targets[torque_entity] = entity;
    entity_mask[torque_entity] |= TARGETABLE | TORQUE;
    console_debug_write(LOG_ENGINE, "Set Entity: %d Torque: %f\n", entity, t);
    return torque_entity;
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
    if(!entity_alive[parent]) {
        //Error
        return;
    }
    if(!entity_alive[child]) {
        //Warning this child is currently not alive
    }
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
    if(!entity_alive[entity]) {
        //Error
        return (Children){0};
    }
    return children[entity];
}
Parent get_parent(Entity entity) {
    return parents[entity];
}

void set_transform_lock(
        Entity driven,
        Entity driver, 
        Vec2D local_offset,
        Orientation local_angle,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
        ) {
    if(!entity_alive[driven] || !entity_alive[driver]) {
        //Error
        return;
    }
    add_components(driven, TRANSFORM_LOCK);
    transform_locks[driven] = (TransformLock) {
        .driver = driver,
        .local_offset = local_offset,
        .local_angle = local_angle,
        .lock_position = lock_position,
        .lock_orientation = lock_orientation,
        .inherit_velocity = inherit_velocity
    };
}

void remove_transform_lock(Entity entity) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }
    delete_components(entity, TRANSFORM_LOCK);
    transform_locks[entity] = (TransformLock){0};
}

void set_transform_lock_current_transform(
        Entity driven,
        Entity driver,
        bool lock_position,
        bool lock_orientation,
        bool inherit_velocity
        ) {
    if(!entity_alive[driven] || !entity_alive[driver]) {
        return;
    }

    Vec2D world_offset = {
        .x = positions[driven].x - positions[driver].x,
        .y = positions[driven].y - positions[driver].y
    };

    Vec2D local_offset = rotate_vector(
        world_offset,
        -orientations[driver]
    );

    Orientation local_angle =
        orientations[driven] - orientations[driver];

    set_transform_lock(
        driven,
        driver,
        local_offset,
        local_angle,
        lock_position,
        lock_orientation,
        inherit_velocity
    );
}

Entity set_joint(
    Entity a,
    Entity b,
    JointType type,
    Vec2D local_anchor_a,
    Vec2D local_anchor_b,
    float stiffness,
    float damping
) {
    if(!entity_alive[a] || !entity_alive[b]) {
        return 0;
    }

    Entity joint = add_entity();

    add_components(joint, JOINT);

    Vec2D world_anchor_a = {
        .x = positions[a].x + rotate_vector(local_anchor_a, orientations[a]).x,
        .y = positions[a].y + rotate_vector(local_anchor_a, orientations[a]).y
    };

    Vec2D world_anchor_b = {
        .x = positions[b].x + rotate_vector(local_anchor_b, orientations[b]).x,
        .y = positions[b].y + rotate_vector(local_anchor_b, orientations[b]).y
    };

    Vec2D delta = {
        .x = world_anchor_b.x - world_anchor_a.x,
        .y = world_anchor_b.y - world_anchor_a.y
    };

    joints[joint] = (Joint){
        .type = type,
        .a = a,
        .b = b,
        .local_anchor_a = local_anchor_a,
        .local_anchor_b = local_anchor_b,
        .rest_length = vector_magnitude(delta),
        .stiffness = stiffness,
        .damping = damping,
        .lock_angle = false,
        .rest_angle = orientations[b] - orientations[a],
        .angular_stiffness = 0.0f,
        .angular_damping = 0.0f
    };

    return joint;
}

void set_life_time(Entity entity, Time expirey_time, Tick expirey_tick) {
    if(!entity_alive[entity]) {
        //Error
        return;
    }

    add_components(entity, LIFETIME);
    life_times[entity] = (LifeTime){
        .expirey_time = expirey_time,
        .expirey_tick = expirey_tick
    };
}

void remove_life_time(Entity entity) {
    delete_components(entity, LIFETIME);
    life_times[entity] = (LifeTime){0};
}
