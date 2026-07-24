#ifndef ENGINE_INTERNAL_H
#define ENGINE_INTERNAL_H

#include <stddef.h>
#include "error.h"

EngineResult engine_tables_ensure_capacity(size_t capacity);

EngineResult entity_tables_init(void);
EngineResult entity_tables_ensure_capacity(size_t capacity);
void entity_tables_destroy(void);

EngineResult physics_tables_init(void);
EngineResult physics_tables_ensure_capacity(size_t capacity);
void physics_tables_destroy(void);

EngineResult graphics_tables_init(void);
EngineResult graphics_tables_ensure_capacity(size_t capacity);
void graphics_tables_destroy(void);

EngineResult grid_tables_init(void);
EngineResult grid_tables_ensure_capacity(size_t capacity);
void grid_tables_destroy(void);

#endif
