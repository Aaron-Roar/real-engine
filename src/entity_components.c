#include "entity_components.h"
#include "engine_internal.h"
#include <stdbool.h>
#include <string.h>

MEMORY_DECLARE_OBJECT_POOL(EntityAlivePool, bool);
MEMORY_DEFINE_OBJECT_POOL(EntityAlivePool, bool)
MEMORY_DEFINE_OBJECT_POOL(EntityMaskPool, CMask)
MEMORY_DEFINE_OBJECT_POOL(TargetPool, Entity)
MEMORY_DEFINE_OBJECT_POOL(ParentPool, Parent)
MEMORY_DEFINE_OBJECT_POOL(ChildrenPool, Children)
MEMORY_DEFINE_OBJECT_POOL(LifeTimePool, LifeTime)

typedef struct EntityIdPool {
    Entity free_ids[MAX_ENTITIES];
    EntityIndex free_indices[MAX_ENTITIES];
    size_t free_count;
    size_t free_index_count;
    size_t live_count;
    Entity next_id;
    EntityIndex next_index;
} EntityIdPool;

EntityAlivePool entity_alive_pool = {0};
EntityMaskPool entity_mask_pool = {0};
static EntityIdPool entity_id_pool = {0};
static EntityIndex entity_to_index[MAX_ENTITIES + 1] = {0};
static Entity index_to_entity[MAX_ENTITIES] = {0};
static uint16_t entity_generations[MAX_ENTITIES + 1] = {0};
static Entity entity_alive_entities[MAX_ENTITIES] = {0};
static EntityIndex entity_alive_positions[MAX_ENTITIES + 1] = {0};

//Moveable Objects
TargetPool targets_pool = {0};
ParentPool parents_pool = {0};
ChildrenPool children_pool = {0};
LifeTimePool life_times_pool = {0};

static void entity_id_pool_init(void) {
    memset(entity_id_pool.free_ids, 0, sizeof(entity_id_pool.free_ids));
    memset(entity_id_pool.free_indices, 0, sizeof(entity_id_pool.free_indices));
    memset(entity_to_index, 0, sizeof(entity_to_index));
    memset(index_to_entity, 0, sizeof(index_to_entity));
    memset(entity_generations, 0, sizeof(entity_generations));
    memset(entity_alive_entities, 0, sizeof(entity_alive_entities));
    memset(entity_alive_positions, 0, sizeof(entity_alive_positions));
    entity_id_pool.free_count = 0;
    entity_id_pool.free_index_count = 0;
    entity_id_pool.live_count = 0;
    entity_id_pool.next_id = 1;
    entity_id_pool.next_index = 0;
}

EngineResult entity_tables_init(void) {
    if(EntityAlivePool_init(&entity_alive_pool, 0).kind == ERROR_RESULT_ERROR) {
        goto fail;
    }
    if(EntityMaskPool_init(&entity_mask_pool, 0).kind == ERROR_RESULT_ERROR) {
        goto fail;
    }
    if(TargetPool_init(&targets_pool, 0).kind == ERROR_RESULT_ERROR) {
        goto fail;
    }
    if(ParentPool_init(&parents_pool, 0).kind == ERROR_RESULT_ERROR) {
        goto fail;
    }
    if(ChildrenPool_init(&children_pool, 0).kind == ERROR_RESULT_ERROR) {
        goto fail;
    }
    if(LifeTimePool_init(&life_times_pool, 0).kind == ERROR_RESULT_ERROR) {
        goto fail;
    }
    entity_id_pool_init();
    return error_result_value(true);

fail:
    entity_tables_destroy();
    return error_result_error(ERROR_ENGINE_ENTITY_TABLES_INIT_FAILED);
}

EngineResult entity_tables_ensure_capacity(size_t capacity) {
    size_t new_capacity;

    if(capacity > MAX_ENTITIES) {
        return error_result_error(ERROR_ENGINE_MAX_ENTITIES_EXCEEDED);
    }
    if(capacity <= entity_alive_pool.capacity) {
        return error_result_value(true);
    }
    new_capacity = entity_alive_pool.capacity == 0 ? 16 : entity_alive_pool.capacity;
    while(new_capacity < capacity) {
        new_capacity *= 2;
    }
    if(new_capacity > MAX_ENTITIES) {
        new_capacity = MAX_ENTITIES;
    }
    if(new_capacity > entity_alive_pool.capacity && EntityAlivePool_expand(&entity_alive_pool, new_capacity - entity_alive_pool.capacity).kind == ERROR_RESULT_ERROR) { return error_result_error(ERROR_ENGINE_TABLE_EXPANSION_FAILED); }
    if(new_capacity > entity_mask_pool.capacity && EntityMaskPool_expand(&entity_mask_pool, new_capacity - entity_mask_pool.capacity).kind == ERROR_RESULT_ERROR) { return error_result_error(ERROR_ENGINE_TABLE_EXPANSION_FAILED); }
    if(new_capacity > targets_pool.capacity && TargetPool_expand(&targets_pool, new_capacity - targets_pool.capacity).kind == ERROR_RESULT_ERROR) { return error_result_error(ERROR_ENGINE_TABLE_EXPANSION_FAILED); }
    if(new_capacity > parents_pool.capacity && ParentPool_expand(&parents_pool, new_capacity - parents_pool.capacity).kind == ERROR_RESULT_ERROR) { return error_result_error(ERROR_ENGINE_TABLE_EXPANSION_FAILED); }
    if(new_capacity > children_pool.capacity && ChildrenPool_expand(&children_pool, new_capacity - children_pool.capacity).kind == ERROR_RESULT_ERROR) { return error_result_error(ERROR_ENGINE_TABLE_EXPANSION_FAILED); }
    if(new_capacity > life_times_pool.capacity && LifeTimePool_expand(&life_times_pool, new_capacity - life_times_pool.capacity).kind == ERROR_RESULT_ERROR) { return error_result_error(ERROR_ENGINE_TABLE_EXPANSION_FAILED); }
    return error_result_value(true);
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

static bool entity_id_valid(Entity entity) {
    Entity slot = entity & 0xFFFFu;
    uint16_t generation = (uint16_t)(entity >> 16);

    return slot > ENTITY_INVALID && slot <= MAX_ENTITIES && generation > 0;
}

static bool entity_table_index_valid(EntityIndex index) {
    return index < MAX_ENTITIES;
}

static Entity entity_make(uint32_t slot, uint16_t generation) {
    return ((Entity)generation << 16) | slot;
}

static uint32_t entity_slot(Entity entity) {
    return entity & 0xFFFFu;
}

static uint16_t entity_generation(Entity entity) {
    return (uint16_t)(entity >> 16);
}

bool entity_is_alive(Entity entity) {
    EntityIndex index;

    if(!entity_get_index(entity, &index)) {
        return false;
    }
    return entity_index_is_alive(index);
}

bool entity_index_is_alive(EntityIndex index) {
    if(!entity_table_index_valid(index)) {
        return false;
    }
    if(index >= entity_alive_pool.capacity) {
        return false;
    }
    if(entity_alive_pool.objects == NULL || entity_alive_pool.used == NULL) {
        return false;
    }
    if(entity_alive_pool.used[index] == 0) {
        return false;
    }
    return entity_alive_pool.objects[index];
}

uint32_t entity_alive_count(void) {
    return (uint32_t)entity_id_pool.live_count;
}

EntityResult entity_alive_at(uint32_t position) {
    if(position >= entity_id_pool.live_count) {
        return ERROR_RESULT_MAKE_ERROR(EntityResult, ERROR_ENGINE_ENTITY_NOT_FOUND);
    }
    return ERROR_RESULT_MAKE_VALUE(EntityResult, entity_alive_entities[position]);
}

bool entity_get_index(Entity entity, EntityIndex *index) {
    uint32_t slot;

    if(index == NULL || !entity_id_valid(entity)) {
        return false;
    }
    slot = entity_slot(entity);
    if(entity_generations[slot] != entity_generation(entity)) {
        return false;
    }
    if(entity_to_index[slot] >= MAX_ENTITIES) {
        return false;
    }
    if(index_to_entity[entity_to_index[slot]] != entity) {
        return false;
    }
    *index = entity_to_index[slot];
    return true;
}

EntityResult entity_from_index(EntityIndex index) {
    if(!entity_table_index_valid(index)) {
        return ERROR_RESULT_MAKE_ERROR(EntityResult, ERROR_ENGINE_INVALID_ENTITY);
    }
    if(!entity_index_is_alive(index)) {
        return ERROR_RESULT_MAKE_ERROR(EntityResult, ERROR_ENGINE_ENTITY_NOT_FOUND);
    }
    return ERROR_RESULT_MAKE_VALUE(EntityResult, index_to_entity[index]);
}

EntityResult entity_add() {
    Entity entity;
    uint32_t slot;
    EntityIndex index;
    size_t required_capacity;
    EngineResult result;
    bool reused_slot = false;
    bool reused_index = false;

    if(entity_id_pool.free_count > 0) {
        entity_id_pool.free_count -= 1;
        slot = entity_id_pool.free_ids[entity_id_pool.free_count];
        reused_slot = true;
    } else {
        if(entity_id_pool.next_id > MAX_ENTITIES) {
            return ERROR_RESULT_MAKE_ERROR(EntityResult, ERROR_ENGINE_MAX_ENTITIES_EXCEEDED);
        }
        slot = entity_id_pool.next_id;
        entity_id_pool.next_id += 1;
    }

    if(entity_id_pool.free_index_count > 0) {
        entity_id_pool.free_index_count -= 1;
        index = entity_id_pool.free_indices[entity_id_pool.free_index_count];
        reused_index = true;
    } else {
        if(entity_id_pool.next_index >= MAX_ENTITIES) {
            return ERROR_RESULT_MAKE_ERROR(EntityResult, ERROR_ENGINE_MAX_ENTITIES_EXCEEDED);
        }
        index = entity_id_pool.next_index;
        entity_id_pool.next_index += 1;
    }

    required_capacity = (size_t)index + 1;
    result = engine_tables_ensure_capacity(required_capacity);
    if(result.kind == ERROR_RESULT_ERROR) {
        if(reused_slot && entity_id_pool.free_count < MAX_ENTITIES) {
            entity_id_pool.free_ids[entity_id_pool.free_count] = slot;
            entity_id_pool.free_count += 1;
        }
        else if(!reused_slot && entity_id_pool.next_id > 1) {
            entity_id_pool.next_id -= 1;
        }
        if(reused_index && entity_id_pool.free_index_count < MAX_ENTITIES) {
            entity_id_pool.free_indices[entity_id_pool.free_index_count] = index;
            entity_id_pool.free_index_count += 1;
        }
        else if(!reused_index && entity_id_pool.next_index > 0) {
            entity_id_pool.next_index -= 1;
        }
        return ERROR_RESULT_MAKE_ERROR(EntityResult, result.result.error);
    }

    if(entity_generations[slot] == 0) {
        entity_generations[slot] = 1;
    }
    entity = entity_make(slot, entity_generations[slot]);
    entity_id_pool.live_count += 1;
    entity_to_index[slot] = index;
    index_to_entity[index] = entity;
    entity_alive_positions[slot] = (EntityIndex)(entity_id_pool.live_count - 1);
    entity_alive_entities[entity_alive_positions[slot]] = entity;

    (void)EntityAlivePool_store_at(&entity_alive_pool, index, true);
    (void)EntityMaskPool_store_at(&entity_mask_pool, index, 0);
    return ERROR_RESULT_MAKE_VALUE(EntityResult, entity);
}

static void entity_clear_index(EntityIndex index) {
    if(!entity_table_index_valid(index)) {
        return;
    }
    if(index < entity_alive_pool.capacity && entity_alive_pool.used[index]) {
        (void)EntityAlivePool_release_at(&entity_alive_pool, index);
    }
    if(index < entity_mask_pool.capacity && entity_mask_pool.used[index]) {
        (void)EntityMaskPool_release_at(&entity_mask_pool, index);
    }
    if(index < targets_pool.capacity && targets_pool.used[index]) {
        (void)TargetPool_release_at(&targets_pool, index);
    }
    if(index < parents_pool.capacity && parents_pool.used[index]) {
        (void)ParentPool_release_at(&parents_pool, index);
    }
    if(index < children_pool.capacity && children_pool.used[index]) {
        (void)ChildrenPool_release_at(&children_pool, index);
    }
    if(index < life_times_pool.capacity && life_times_pool.used[index]) {
        (void)LifeTimePool_release_at(&life_times_pool, index);
    }
}

EngineResult entity_delete(Entity entity) {
    EntityIndex index;
    EntityIndex removed_alive_position;
    EntityIndex last_alive_position;
    Entity moved_entity;
    uint32_t moved_slot;
    uint32_t slot;

    if(!entity_id_valid(entity)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }
    if(!entity_get_index(entity, &index) || !entity_index_is_alive(index)) {
        return error_result_error(ERROR_ENGINE_ENTITY_NOT_FOUND);
    }
    entity_clear_index(index);
    slot = entity_slot(entity);
    removed_alive_position = entity_alive_positions[slot];
    last_alive_position = (EntityIndex)(entity_id_pool.live_count - 1);
    moved_entity = entity_alive_entities[last_alive_position];
    moved_slot = entity_slot(moved_entity);

    entity_alive_entities[removed_alive_position] = moved_entity;
    entity_alive_positions[moved_slot] = removed_alive_position;
    entity_alive_entities[last_alive_position] = ENTITY_INVALID;
    entity_alive_positions[slot] = 0;
    entity_to_index[slot] = 0;
    index_to_entity[index] = ENTITY_INVALID;
    entity_generations[slot] += 1;
    if(entity_generations[slot] == 0) {
        entity_generations[slot] = 1;
    }

    if(entity_id_pool.free_count < MAX_ENTITIES) {
        entity_id_pool.free_ids[entity_id_pool.free_count] = slot;
        entity_id_pool.free_count += 1;
    }
    if(entity_id_pool.free_index_count < MAX_ENTITIES) {
        entity_id_pool.free_indices[entity_id_pool.free_index_count] = index;
        entity_id_pool.free_index_count += 1;
    }
    if(entity_id_pool.live_count > 0) {
        entity_id_pool.live_count -= 1;
    }
    return error_result_value(true);
}

EngineResult entity_add_components(Entity entity, CMask mask) {
    EntityIndex index;

    if(!entity_id_valid(entity)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }
    if(!entity_get_index(entity, &index) || !entity_index_is_alive(index)) {
        return error_result_error(ERROR_ENGINE_ENTITY_NOT_FOUND);
    }
    (void)EntityMaskPool_store_at(&entity_mask_pool, index, entity_mask[index] | mask);
    return error_result_value(true);
}

EngineResult entity_delete_components(Entity entity, CMask mask) {
    EntityIndex index;

    if(!entity_id_valid(entity)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }
    if(!entity_get_index(entity, &index) || !entity_index_is_alive(index)) {
        return error_result_error(ERROR_ENGINE_ENTITY_NOT_FOUND);
    }
    (void)EntityMaskPool_store_at(&entity_mask_pool, index, entity_mask[index] & ~mask);
    return error_result_value(true);
}

bool entity_has_components(Entity entity, CMask components) {
  EntityIndex index;

  if(!entity_get_index(entity, &index)) {
    return false;
  }
  if(!entity_index_is_alive(index)) {
    return false;
  }
  return entity_index_has_components(index, components);
}

bool entity_index_has_components(EntityIndex index, CMask components) {
  if(!entity_index_is_alive(index)) {
    return false;
  }
  if( (entity_mask[index] & components) == components) {
    return true;
  }
  return false;
}


EngineResult entity_set_child(Entity parent, Entity child) {
    EntityIndex parent_index;
    EntityIndex child_index;
    EngineResult result;

    if(!entity_get_index(parent, &parent_index) || !entity_get_index(child, &child_index)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }
    if(!entity_index_is_alive(parent_index) || !entity_index_is_alive(child_index)) {
        return error_result_error(ERROR_ENGINE_ENTITY_NOT_FOUND);
    }
    result = entity_add_components(parent, HAS_CHILDREN);
    if(result.kind == ERROR_RESULT_ERROR) {
        return result;
    }
    result = entity_add_components(child, HAS_PARENT);
    if(result.kind == ERROR_RESULT_ERROR) {
        return result;
    }
    (void)ChildrenPool_store_at(&children_pool, parent_index, children[parent_index]);
    children[parent_index].entities[child_index] = child;
    (void)ParentPool_store_at(&parents_pool, child_index, parent);
    return error_result_value(true);
}

EngineResult entity_set_parent(Entity child, Entity parent) {
    EntityIndex parent_index;
    EntityIndex child_index;
    EngineResult result;

    if(!entity_get_index(parent, &parent_index) || !entity_get_index(child, &child_index)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }
    if(!entity_index_is_alive(parent_index) || !entity_index_is_alive(child_index)) {
        return error_result_error(ERROR_ENGINE_ENTITY_NOT_FOUND);
    }
    result = entity_add_components(parent, HAS_CHILDREN);
    if(result.kind == ERROR_RESULT_ERROR) {
        return result;
    }
    result = entity_add_components(child, HAS_PARENT);
    if(result.kind == ERROR_RESULT_ERROR) {
        return result;
    }
    (void)ChildrenPool_store_at(&children_pool, parent_index, children[parent_index]);
    children[parent_index].entities[child_index] = child;
    (void)ParentPool_store_at(&parents_pool, child_index, parent);
    return error_result_value(true);
}

EngineResult entity_remove_parent(Entity child) {
    EntityIndex child_index;
    Entity parent;
    EntityIndex parent_index;
    EngineResult result;

    if(!entity_get_index(child, &child_index) || !entity_index_is_alive(child_index)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }
    parent = parents[child_index];
    if(!entity_get_index(parent, &parent_index) || !entity_index_is_alive(parent_index)) {
        //Warning no parent is currently set
        return error_result_value(true);
    }
    //Removing this child from the parent
    children[parent_index].entities[child_index] = ENTITY_INVALID;
    //If the parent has no more children remove the flag
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(children[parent_index].entities[i] != ENTITY_INVALID) {
            result = entity_delete_components(parent, HAS_CHILDREN);
            if(result.kind == ERROR_RESULT_ERROR) {
                return result;
            }
        }
    }

    //Remove the parent from the child
    if(child_index < parents_pool.capacity && parents_pool.used[child_index]) {
        (void)ParentPool_release_at(&parents_pool, child_index);
    }
    result = entity_delete_components(child, HAS_PARENT);
    if(result.kind == ERROR_RESULT_ERROR) {
        return result;
    }
    return error_result_value(true);
}

EngineResult entity_remove_child(Entity parent, Entity child) {
    EntityIndex parent_index;
    EntityIndex child_index;
    EngineResult result;

    if(!entity_get_index(parent, &parent_index) || !entity_index_is_alive(parent_index)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }
    if(!entity_get_index(child, &child_index) || !entity_index_is_alive(child_index)) {
        //Warning this child is currently not alive
        return error_result_value(true);
    }
    //Removing the child from the parent
    children[parent_index].entities[child_index] = ENTITY_INVALID;
    //If the parent has no more children remove the flag
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(children[parent_index].entities[i] != ENTITY_INVALID) {
            result = entity_delete_components(parent, HAS_CHILDREN);
            if(result.kind == ERROR_RESULT_ERROR) {
                return result;
            }
        }
    }

    //Removing the parent from the child
    if(child_index < parents_pool.capacity && parents_pool.used[child_index]) {
        (void)ParentPool_release_at(&parents_pool, child_index);
    }
    result = entity_delete_components(child, HAS_PARENT);
    if(result.kind == ERROR_RESULT_ERROR) {
        return result;
    }

    return error_result_value(true);
}

ChildrenResult entity_get_children(Entity entity) {
    EntityIndex index;

    if(!entity_get_index(entity, &index) || !entity_index_is_alive(index)) {
        return ERROR_RESULT_MAKE_ERROR(ChildrenResult, ERROR_ENGINE_INVALID_ENTITY);
    }
    return ERROR_RESULT_MAKE_VALUE(ChildrenResult, children[index]);
}
ParentResult entity_get_parent(Entity entity) {
    EntityIndex index;

    if(!entity_get_index(entity, &index) || !entity_index_is_alive(index)) {
        return ERROR_RESULT_MAKE_ERROR(ParentResult, ERROR_ENGINE_INVALID_ENTITY);
    }
    return ERROR_RESULT_MAKE_VALUE(ParentResult, parents[index]);
}





EngineResult entity_set_life_time(Entity entity, Time expirey_time, Tick expirey_tick) {
    EntityIndex index;
    EngineResult result;

    if(!entity_get_index(entity, &index) || !entity_index_is_alive(index)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }

    result = entity_add_components(entity, LIFETIME);
    if(result.kind == ERROR_RESULT_ERROR) {
        return result;
    }
    (void)LifeTimePool_store_at(&life_times_pool, index, (LifeTime){
        .expirey_time = expirey_time,
        .expirey_tick = expirey_tick
    });
    return error_result_value(true);
}

EngineResult entity_remove_life_time(Entity entity) {
    EntityIndex index;
    EngineResult result;

    if(!entity_get_index(entity, &index)) {
        return error_result_error(ERROR_ENGINE_INVALID_ENTITY);
    }
    result = entity_delete_components(entity, LIFETIME);
    if(result.kind == ERROR_RESULT_ERROR) {
        return result;
    }
    if(index < life_times_pool.capacity && life_times_pool.used[index]) {
        (void)LifeTimePool_release_at(&life_times_pool, index);
    }
    return error_result_value(true);
}
