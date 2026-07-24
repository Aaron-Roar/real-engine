#ifndef GRID_H
#define GRID_H
#include "math2d.h"
#include "entity_components.h"
#define CELL_SIZE 20
#define GRID_ROWS 25
#define GRID_COLS 25
typedef struct {
    Entity entities[MAX_ENTITIES];
    bool entity_present[MAX_ENTITIES];
} Cell;

typedef struct {
    Cell cells[GRID_ROWS][GRID_COLS];
} Grid;


typedef struct {
    bool pairs[MAX_ENTITIES][MAX_ENTITIES];
} BooleanPairs;
MEMORY_DECLARE_OBJECT_POOL(AABBPool, AABB);
extern AABBPool aabbs_pool;
#define aabbs aabbs_pool.objects
uint32_t world_y_to_row(Vec1D y);
bool grid_tables_init(void);
void grid_tables_destroy(void);
void add_entity_to_grids(Entity entity);
bool checked_pair(Entity entity_1, Entity entity_2);
void add_pair(Entity entity_1, Entity entity_2);
void clear_grid();
void grid_update_aabb(Entity entity);
extern Grid grid;
extern BooleanPairs pair_checked;;
#endif
