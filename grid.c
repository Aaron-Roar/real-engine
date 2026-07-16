#include "grid.h"
#include "entity_components.h"
#include "physics.h"
#include "math2d.h"
#include "console.h"
#include "math.h"


AABB aabbs[MAX_ENTITIES] = {0};

Grid grid = {0};
BooleanPairs pair_checked = {0};

bool checked_pair(Entity entity_1, Entity entity_2) {
    return pair_checked.pairs[entity_1][entity_2];
}
void add_pair(Entity entity_1, Entity entity_2) {
    pair_checked.pairs[entity_1][entity_2] = true;
    pair_checked.pairs[entity_2][entity_1] = true;
}

void grid_update_aabb(Entity entity) {
    aabbs[entity] = math_create_aabb(world_hit_boxes[entity]);
}

void clear_grid() {
    memset(&grid, 0, sizeof(Grid));
    memset(&pair_checked, 0, sizeof(BooleanPairs));
}

uint32_t world_x_to_col(Vec1D x) {
    float col = ((GRID_COLS - 1)/2) + (x/(CELL_SIZE));
    if(col >= GRID_COLS) {
        return GRID_COLS - 1;
    }
    if(col < 0) {
        return 0;
    }
    return col;
}

uint32_t world_y_to_row(Vec1D y) {
    float row = ((GRID_ROWS - 1)/2) + (y/(CELL_SIZE));
    if(row >= GRID_ROWS) {
        return GRID_ROWS - 1;
    }
    if(row < 0) {
        return 0;
    }
    return row;
}

void add_entity_to_grid(Entity entity, uint32_t row, uint32_t col) {
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(!grid.cells[row][col].entity_present[i]) {
            grid.cells[row][col].entities[i] = entity;
            grid.cells[row][col].entity_present[i] = true;
            return;
        }
    }
}

void add_entity_to_grids(Entity entity) {
    AABB aabb = aabbs[entity];
    uint32_t top_row = world_y_to_row(aabb.max_y);
    uint32_t bot_row = world_y_to_row(aabb.min_y);
    uint32_t left_col = world_x_to_col(aabb.min_x);
    uint32_t right_col = world_x_to_col(aabb.max_x);

    for(int row = bot_row; row <= top_row; row += 1) {
        for(int col = left_col; col <= right_col; col += 1) {
            add_entity_to_grid(entity, row, col);
        }
    }
}

