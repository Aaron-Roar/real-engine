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
Entity targets[MAX_ENTITIES] = {0};
Parent parents[MAX_ENTITIES] = {0};
Children children[MAX_ENTITIES] = {0};
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

bool has_components(Entity entity, Component components) {
  if( (entity_mask[entity] & components) == components) {
    return true;
  }
  return false;
}

void print_entity_components(Entity entity) {
}





//Applies a force to a target



void print_alive_entities() {
    console_write(LOG_ENGINE, "AliveEntities: {");
    for(int i = 0; i < MAX_ENTITIES; i++) {
        if(entity_alive[i]) {
            console_write(LOG_ENGINE,"%d ", i);
        }
    }
    console_write(LOG_ENGINE, "}\n");
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

