#ifndef ENTITY_COMPONENTS_H
#define ENTITY_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "engine.h"
#include "memory.h"


/** Stable entity id used by public APIs. */
typedef uint32_t Entity; //An id for an entity

/** Index into component tables used by engine internals and compute loops. */
typedef uint32_t EntityIndex; //An index into component tables

/** Invalid entity id sentinel. */
#define ENTITY_INVALID 0

/** Maximum number of live or reusable entity slots. */
#define MAX_ENTITIES 10000

/** Maximum number of component bits reserved by the engine. */
#define MAX_COMPONENTS 100

/** Result type for functions that return an Entity. */
ERROR_DECLARE_RESULT_TYPE(EntityResult, Entity);

/** Component mask value for an entity. */
typedef uint32_t CMask; //The bit mask for an entities components

/** Object pool storing per-index entity component masks. */
MEMORY_DECLARE_OBJECT_POOL(EntityMaskPool, CMask);

/** Pool backing the entity_mask table. */
extern EntityMaskPool entity_mask_pool;

/** Direct component mask table indexed by EntityIndex. */
#define entity_mask entity_mask_pool.objects

/** Component bits stored in CMask values. */
typedef enum {
    /** No components. */
    NONE                    = 0,
    /** Entity is static in the physics system. */
    STATIC                  = 1 << 0,
    /** Entity is dynamic in the physics system. */
    DYNAMIC                 = 1 << 1,
    /** Entity participates in collision checks. */
    COLLISION               = 1 << 2,
    /** Entity has a hit box shape. */
    HIT_BOX                 = 1 << 3,
    /** Entity has an axis lock constraint. */
    AXIS_LOCK               = 1 << 4,
    /** Entity has an angle lock constraint. */
    ANGLE_LOCK              = 1 << 5,

    /** Entity represents a force source. */
    FORCE                   = 1 << 6,
    /** Entity represents a torque source. */
    TORQUE                  = 1 << 7,
    /** Entity targets another entity. */
    TARGETABLE              = 1 << 8,

    /** Entity has mass. */
    MASS                    = 1 << 9,
    /** Reserved time-window component bit. */
    TIMEWINDOW              = 1 << 10,
    /** Entity has a parent entity. */
    PARENT                      = 1 << 11,
    /** Entity has one or more child entities. */
    CHILD                       = 1 << 12,
    /** Entity has a transform lock constraint. */
    TRANSFORM_LOCK              = 1 << 13,
    /** Entity represents a joint constraint. */
    JOINT                       = 1 << 14,
    /** Entity has a lifetime expiry. */
    LIFETIME                    = 1 << 15,
    /** Entity is treated as a particle by collision/render helpers. */
    PARTICLE                    = 1 << 16,
    /** Entity has an animated sprite. */
    ANIMATED_SPRITE             = 1 << 17,
    /** Entity keeps physics values but skips physics updates. */
    HOLD                        = 1 << 18,
} Component;

/** Parent entity id alias. */
typedef Entity Parent;

/** Child entity id alias. */
typedef Entity Child;

/** Dynamic pool storing one parent entity's child ids. */
MEMORY_DECLARE_OBJECT_POOL(ChildPool, Child);

/** Dynamic child entity list owned by one parent entity component. */
typedef struct Children {
    /** Pool of child entity ids. */
    ChildPool entities;
} Children;

/** Result type for functions returning Children. */
ERROR_DECLARE_RESULT_TYPE(ChildrenResult, Children);

/** Result type for functions returning Parent. */
ERROR_DECLARE_RESULT_TYPE(ParentResult, Parent);

/** Expiry settings for an entity lifetime. */
typedef struct LifeTime {
    /** Engine time at which the entity expires, or zero when unused. */
    Time expirey_time;
    /** Engine tick at which the entity expires, or zero when unused. */
    Tick expirey_tick;
} LifeTime;

/** Pool storing parent entity ids by EntityIndex. */
MEMORY_DECLARE_OBJECT_POOL(ParentPool, Parent);
/** Pool storing child lists by EntityIndex. */
MEMORY_DECLARE_OBJECT_POOL(ChildrenPool, Children);
/** Pool storing lifetime data by EntityIndex. */
MEMORY_DECLARE_OBJECT_POOL(LifeTimePool, LifeTime);
/** Pool storing target entity ids by EntityIndex. */
MEMORY_DECLARE_OBJECT_POOL(TargetPool, Entity);

/** Parent pool backing the parents table. */
extern ParentPool parents_pool;
/** Children pool backing the children table. */
extern ChildrenPool children_pool;
/** Lifetime pool backing the life_times table. */
extern LifeTimePool life_times_pool;
/** Target pool backing the targets table. */
extern TargetPool targets_pool;

/** Parent table indexed by EntityIndex. */
#define parents parents_pool.objects
/** Children table indexed by EntityIndex. */
#define children children_pool.objects
/** Lifetime table indexed by EntityIndex. */
#define life_times life_times_pool.objects
/** Target table indexed by EntityIndex. */
#define targets targets_pool.objects

/**
 * Check whether an entity id currently refers to a live entity.
 *
 * @param entity Entity id to check.
 * @return true when the entity id is valid and live.
 */
bool entity_is_alive(Entity entity);

/**
 * Check whether an entity table index is currently live.
 *
 * @param index Entity table index to check.
 * @return true when the index is live.
 */
bool entity_index_is_alive(EntityIndex index);

/**
 * Return the number of currently alive entities.
 *
 * Systems can use this with entity_alive_at() to iterate only live entities
 * instead of scanning every possible entity slot.
 *
 * @return Number of alive entities.
 */
uint32_t entity_alive_count(void);

/**
 * Return the entity id stored at a dense alive-list position.
 *
 * The position is not a component table index and can change when entities are
 * deleted. Resolve the returned id with entity_get_index() before accessing
 * component tables.
 *
 * @param position Dense alive-list position.
 * @return EntityResult containing the entity id, or an error.
 */
EntityResult entity_alive_at(uint32_t position);

/**
 * Resolve an entity id to its current table index.
 *
 * @param entity Entity id to resolve.
 * @param index Out parameter receiving the current table index.
 * @return true when the entity resolves to a live index.
 */
bool entity_get_index(Entity entity, EntityIndex *index);

/**
 * Resolve a table index to its current entity id.
 *
 * @param index Entity table index to resolve.
 * @return EntityResult containing the entity id, or an error.
 */
EntityResult entity_from_index(EntityIndex index);

/**
 * Add a live entity and grow entity-indexed tables as needed.
 *
 * @return EntityResult containing a stable entity id, or an error.
 */
EntityResult entity_add(void);

/**
 * Delete an entity and make its id/index slots reusable.
 *
 * @param entity Entity id to delete.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_delete(Entity entity);

/**
 * Add one or more component bits to an entity.
 *
 * @param entity Entity id to update.
 * @param mask Component mask bits to add.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_add_components(Entity entity, CMask mask);

/**
 * Check whether an entity has all requested components.
 *
 * @param entity Entity id to check.
 * @param components Component mask that must be present.
 * @return true when all requested bits are set.
 */
bool entity_has_components(Entity entity, CMask components);

/**
 * Check whether an entity index has all requested components.
 *
 * @param index Entity table index to check.
 * @param components Component mask that must be present.
 * @return true when all requested bits are set.
 */
bool entity_index_has_components(EntityIndex index, CMask components);

/**
 * Remove component bits from an entity.
 *
 * @param entity Entity id to update.
 * @param mask Component mask bits to remove.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_delete_components(Entity entity, CMask mask);

/**
 * Set a parent-child relationship.
 *
 * @param parent Parent entity id.
 * @param child Child entity id.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_set_child(Entity parent, Entity child);

/**
 * Set a parent-child relationship from the child side.
 *
 * @param child Child entity id.
 * @param parent Parent entity id.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_set_parent(Entity child, Entity parent);

/**
 * Remove the parent from a child entity.
 *
 * @param child Child entity id.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_remove_parent(Entity child);

/**
 * Remove a child from a parent entity.
 *
 * @param parent Parent entity id.
 * @param child Child entity id.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_remove_child(Entity parent, Entity child);

/**
 * Get the children table for an entity.
 *
 * @param entity Entity id to inspect.
 * @return ChildrenResult containing child data, or an error.
 */
ChildrenResult entity_get_children(Entity entity);

/**
 * Get the parent entity for an entity.
 *
 * @param entity Entity id to inspect.
 * @return ParentResult containing parent id, or an error.
 */
ParentResult entity_get_parent(Entity entity);

/**
 * Set an entity lifetime expiry by time and/or tick.
 *
 * @param entity Entity id to update.
 * @param expirey_time Engine time expiry, or zero when unused.
 * @param expirey_tick Engine tick expiry, or zero when unused.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_set_life_time(Entity entity, Time expirey_time, Tick expirey_tick);

/**
 * Remove lifetime data from an entity.
 *
 * @param entity Entity id to update.
 * @return EngineResult describing success or failure.
 */
EngineResult entity_remove_life_time(Entity entity);
#endif
