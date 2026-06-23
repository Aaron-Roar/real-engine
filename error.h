#ifndef ERROR_H
#define ERROR_H
#include "entity_components.h"

#define MAX_ENTITY_REPORT 500
#define MAX_COMPONENT_REPORT 500
#define MAX_SYSTEM_REPORT 500
#define MAX_ENGINE_REPORT 500
#define MAX_ERROR_REPORT (MAX_ENTITY_REPORT + MAX_COMPONENT_REPORT + MAX_SYSTEM_REPORT + MAX_ENGINE_REPORT)
typedef struct EntityReport {
    char report[MAX_ENTITY_REPORT];
} EntityReport;
typedef struct SystemReport {
    char report[MAX_SYSTEM_REPORT];
} SystemReport;
typedef struct ComponentReport {
    char report[MAX_COMPONENT_REPORT];
} ComponentReport;
typedef struct EngineReport {
    char report[MAX_ENGINE_REPORT];
} EngineReport;
typedef struct ErrorReport {
    EntityReport entity;
    SystemReport system;
    ComponentReport component;
    EngineReport engine;
} ErrorReport;
typedef enum ErrorCode {
    ERROR_CODE_DOES_NOT_EXIST   = 1 << 0,
    ENTITY_RANGE_EXCEEDED       = 1 << 1,
    ENTITY_DOES_NOT_EXIST       = 1 << 2,
    FAILED_ADD_ENTITY           = 1 << 3,
    FAILED_DELETE_ENTITY        = 1 << 4,
    FAILED_ADD_COMPONENTS       = 1 << 5,
    FAILED_DELETE_COMPONENTS    = 1 << 6,
    FAILED_UPDATE_ACCELERATION  = 1 << 7,
    ACCELERATING_MASSLESS_ENTITY= 1 << 8,
    INCOMPATABLE_COMPONENTS     = 1 << 9,
} ErrorCode;
typedef enum ErrorLevel {ENTITY, COMPONENT, SYSTEM, CORE} ErrorLevel;
typedef enum ErrorSeverity {WARNING, MINIMAL, CRITICAL} ErrorSeverity;
typedef struct ErrorInfo {
    ErrorCode code;
    ErrorLevel level;
    ErrorSeverity severity;
    char* string;
} ErrorInfo;
typedef struct Error {
    ErrorCode code;
    EntityList entities;
    ErrorSeverity severity;
    ErrorReport report;
} Error;

void error_print(Error err);
void error_add_entity(Error* err, Entity entity);

ErrorInfo error_get_info(ErrorCode code);
#endif
