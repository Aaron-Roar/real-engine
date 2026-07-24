#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

typedef enum {
    ERROR_RESULT_VALUE,
    ERROR_RESULT_ERROR,
} ErrorResultKind;

typedef enum EngineError {
    ERROR_NONE = 0,

    ERROR_MEMORY_POOL_NULL_POINTER,
    ERROR_MEMORY_POOL_CAPACITY_OVERFLOW,
    ERROR_MEMORY_POOL_ALLOCATION_FAILED,
    ERROR_MEMORY_POOL_FULL,
    ERROR_MEMORY_POOL_INVALID_OBJECT,
    ERROR_MEMORY_POOL_OBJECT_NOT_USED,
    ERROR_MEMORY_POOL_SHRINK_WOULD_REMOVE_USED_OBJECT,

    ERROR_ENGINE_ALREADY_RUNNING,
    ERROR_ENGINE_SDL_INIT_FAILED,
    ERROR_ENGINE_ENTITY_TABLES_INIT_FAILED,
    ERROR_ENGINE_PHYSICS_TABLES_INIT_FAILED,
    ERROR_ENGINE_GRAPHICS_TABLES_INIT_FAILED,
    ERROR_ENGINE_GRID_TABLES_INIT_FAILED,
    ERROR_ENGINE_GRAPHICS_INIT_FAILED,
    ERROR_ENGINE_MAX_ENTITIES_EXCEEDED,
    ERROR_ENGINE_TABLE_EXPANSION_FAILED,
} EngineError;

/*
 * A result type needs an explicit result name in C.
 *
 * C macros cannot reliably build a type name from an arbitrary C type because
 * valid types can include spaces, pointers, and other tokens that cannot be
 * pasted into an identifier. Pass the PascalCase ResultType explicitly.
 */
#define ERROR_DECLARE_RESULT_TYPE(ResultType, ValueType) \
    typedef union Result##ResultType { \
        ValueType value; \
        EngineError error; \
    } Result##ResultType; \
    \
    typedef struct ResultType { \
        ErrorResultKind kind; \
        Result##ResultType result; \
    } ResultType

ERROR_DECLARE_RESULT_TYPE(EngineResult, bool);

EngineResult error_result_value(bool value);
EngineResult error_result_error(EngineError error);
const char *error_string(EngineError error);

#endif
