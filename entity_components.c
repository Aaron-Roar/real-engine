#include "entity_components.h"
#include "console.h"
#include "tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

MEMORY_DEFINE_OBJECT_POOL(EntityAlivePool, bool)
MEMORY_DEFINE_OBJECT_POOL(EntityMaskPool, CMask)
MEMORY_DEFINE_OBJECT_POOL(TargetPool, Entity)
MEMORY_DEFINE_OBJECT_POOL(ParentPool, Parent)
MEMORY_DEFINE_OBJECT_POOL(ChildrenPool, Children)
MEMORY_DEFINE_OBJECT_POOL(LifeTimePool, LifeTime)

EntityAlivePool entity_alive_pool = {0};
EntityMaskPool entity_mask_pool = {0};
EntityIdPool entity_id_pool = {0};

//Moveable Objects
TargetPool targets_pool = {0};
ParentPool parents_pool = {0};
ChildrenPool children_pool = {0};
LifeTimePool life_times_pool = {0};

static void entity_id_pool_init(void) {
    for(size_t i = 0; i < MAX_ENTITIES; i += 1) {
        entity_id_pool.free_ids[i] = (Entity)(MAX_ENTITIES - 1 - i);
    }
    entity_id_pool.free_count = MAX_ENTITIES;
    entity_id_pool.live_count = 0;
}

bool entity_tables_init(void) {
    if(EntityAlivePool_init(&entity_alive_pool, MAX_ENTITIES).kind == RESULT_ERROR) {
        goto fail;
    }
    if(EntityMaskPool_init(&entity_mask_pool, MAX_ENTITIES).kind == RESULT_ERROR) {
        goto fail;
    }
    if(TargetPool_init(&targets_pool, MAX_ENTITIES).kind == RESULT_ERROR) {
        goto fail;
    }
    if(ParentPool_init(&parents_pool, MAX_ENTITIES).kind == RESULT_ERROR) {
        goto fail;
    }
    if(ChildrenPool_init(&children_pool, MAX_ENTITIES).kind == RESULT_ERROR) {
        goto fail;
    }
    if(LifeTimePool_init(&life_times_pool, MAX_ENTITIES).kind == RESULT_ERROR) {
        goto fail;
    }
    entity_id_pool_init();
    return true;

fail:
    entity_tables_destroy();
    return false;
}

void entity_tables_destroy(void) {
    (void)EntityAlivePool_destroy(&entity_alive_pool);
    (void)EntityMaskPool_destroy(&entity_mask_pool);
    (void)TargetPool_destroy(&targets_pool);
    (void)ParentPool_destroy(&parents_pool);
    (void)ChildrenPool_destroy(&children_pool);
    (void)LifeTimePool_destroy(&life_times_pool);
    entity_id_pool = (EntityIdPool){0};
}

static bool entity_index_valid(Entity entity) {
    return entity < MAX_ENTITIES;
}

Entity entity_add() {
    Entity entity;

    if(entity_id_pool.free_count == 0) {
        console_write(LOG_ENGINE, "Error: failed to add entity, MAX_ENTITIES exceeded\n");
        return 0; //Unused location
    }

    entity_id_pool.free_count -= 1;
    entity = entity_id_pool.free_ids[entity_id_pool.free_count];
    entity_id_pool.live_count += 1;

    (void)EntityAlivePool_store_at(&entity_alive_pool, entity, true);
    (void)EntityMaskPool_store_at(&entity_mask_pool, entity, 0);
    return entity;
}

void entity_clear(Entity entity) {
    if(!entity_index_valid(entity)) {
        return;
    }
    if(entity_alive_pool.used[entity]) {
        (void)EntityAlivePool_release_at(&entity_alive_pool, entity);
    }
    if(entity_mask_pool.used[entity]) {
        (void)EntityMaskPool_release_at(&entity_mask_pool, entity);
    }
    if(targets_pool.used[entity]) {
        (void)TargetPool_release_at(&targets_pool, entity);
    }
    if(parents_pool.used[entity]) {
        (void)ParentPool_release_at(&parents_pool, entity);
    }
    if(children_pool.used[entity]) {
        (void)ChildrenPool_release_at(&children_pool, entity);
    }
    if(life_times_pool.used[entity]) {
        (void)LifeTimePool_release_at(&life_times_pool, entity);
    }
}

void entity_delete(Entity entity) {
    if(!entity_index_valid(entity)) {
        console_write(LOG_ENGINE, "Error: failed to delete entity %u, entity exceeds MAX_ENTITIES\n", entity);
        return;
    }
    if(entity_alive[entity] == 0) {
        console_write(LOG_ENGINE, "Error: failed to delete entity %u, entity does not exist\n", entity);
        return;
    }
    entity_clear(entity);

    if(entity_id_pool.free_count < MAX_ENTITIES) {
        entity_id_pool.free_ids[entity_id_pool.free_count] = entity;
        entity_id_pool.free_count += 1;
    }
    if(entity_id_pool.live_count > 0) {
        entity_id_pool.live_count -= 1;
    }
}

void entity_add_components(Entity entity, CMask mask) {
    if(!entity_index_valid(entity)) {
        console_write(LOG_ENGINE, "Error: failed to add components to entity %u, entity exceeds MAX_ENTITIES\n", entity);
        return;
    }
    if(entity_alive[entity] == 0) {
        console_write(LOG_ENGINE, "Error: failed to add components to entity %u, entity does not exist\n", entity);
        return;
    }
    (void)EntityMaskPool_store_at(&entity_mask_pool, entity, entity_mask[entity] | mask);
}

void entity_delete_components(Entity entity, CMask mask) {
    if(!entity_index_valid(entity)) {
        console_write(LOG_ENGINE, "Error: failed to delete components from entity %u, entity exceeds MAX_ENTITIES\n", entity);
        return;
    }
    if(entity_alive[entity] == 0) {
        console_write(LOG_ENGINE, "Error: failed to delete components from entity %u, entity does not exist\n", entity);
        return;
    }
    (void)EntityMaskPool_store_at(&entity_mask_pool, entity, entity_mask[entity] & ~mask);
}

bool entity_has_components(Entity entity, Component components) {
  if(!entity_index_valid(entity)) {
    return false;
  }
  if( (entity_mask[entity] & components) == components) {
    return true;
  }
  return false;
}


void entity_set_child(Entity parent, Entity child) {
    if(!entity_index_valid(parent) || !entity_index_valid(child)) {
        return;
    }
    if(!entity_alive[parent] || !entity_alive[child]) {
        //Error parent or child not alive
        return;
    }
    entity_add_components(parent, HAS_CHILDREN);
    entity_add_components(child, HAS_PARENT);
    (void)ChildrenPool_store_at(&children_pool, parent, children[parent]);
    children[parent].entities[child] = child;
    (void)ParentPool_store_at(&parents_pool, child, parent);
}

void entity_set_parent(Entity child, Entity parent) {
    if(!entity_index_valid(parent) || !entity_index_valid(child)) {
        return;
    }
    if(!entity_alive[parent] || !entity_alive[child]) {
        //Error parent or child not alive
        return;
    }
    entity_add_components(parent, HAS_CHILDREN);
    entity_add_components(child, HAS_PARENT);
    (void)ChildrenPool_store_at(&children_pool, parent, children[parent]);
    children[parent].entities[child] = child;
    (void)ParentPool_store_at(&parents_pool, child, parent);
}

void entity_remove_parent(Entity child) {
    if(!entity_index_valid(child)) {
        return;
    }
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
            entity_delete_components(parents[child], HAS_CHILDREN);
        }
    }

    //Remove the parent from the child
    if(parents_pool.used[child]) {
        (void)ParentPool_release_at(&parents_pool, child);
    }
    entity_delete_components(child, HAS_PARENT);
}

void entity_remove_child(Entity parent, Entity child) {
    if(!entity_index_valid(parent) || !entity_index_valid(child)) {
        return;
    }
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
            entity_delete_components(parents[child], HAS_CHILDREN);
        }
    }

    //Removing the parent from the child
    if(parents_pool.used[child]) {
        (void)ParentPool_release_at(&parents_pool, child);
    }
    entity_delete_components(child, HAS_PARENT);

}

Children entity_get_children(Entity entity) {
    if(!entity_index_valid(entity)) {
        return (Children){0};
    }
    if(!entity_alive[entity]) {
        //Error
        return (Children){0};
    }
    return children[entity];
}
Parent entity_get_parent(Entity entity) {
    if(!entity_index_valid(entity)) {
        return 0;
    }
    return parents[entity];
}





void entity_set_life_time(Entity entity, Time expirey_time, Tick expirey_tick) {
    if(!entity_index_valid(entity)) {
        return;
    }
    if(!entity_alive[entity]) {
        //Error
        return;
    }

    entity_add_components(entity, LIFETIME);
    (void)LifeTimePool_store_at(&life_times_pool, entity, (LifeTime){
        .expirey_time = expirey_time,
        .expirey_tick = expirey_tick
    });
}

void entity_remove_life_time(Entity entity) {
    if(!entity_index_valid(entity)) {
        return;
    }
    entity_delete_components(entity, LIFETIME);
    if(life_times_pool.used[entity]) {
        (void)LifeTimePool_release_at(&life_times_pool, entity);
    }
}
