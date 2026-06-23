#include "error.h"
#include "tools.h"
#include "console.h"
#include "stdio.h"


static const ErrorInfo error_info[] = {
    {ERROR_CODE_DOES_NOT_EXIST, CORE, CRITICAL, "|ERROR_CODE_DOES_NOT_EXIST|"},
    {ENTITY_RANGE_EXCEEDED, ENTITY, MINIMAL, "|ENTITY_RANGE_EXCEEDED|"},
    {ENTITY_DOES_NOT_EXIST, ENTITY, WARNING,"|ENTITY_DOES_NOT_EXIST|"},
    {FAILED_ADD_ENTITY, ENTITY, WARNING, "|FAILED_ADD_ENTITY|"},
    {FAILED_DELETE_ENTITY, ENTITY, WARNING, "|FAILED_DELETE_ENTITY|"},
    {FAILED_ADD_COMPONENTS, COMPONENT, WARNING, "|FAILED_ADD_COMPONENTS|"},
    {FAILED_DELETE_COMPONENTS, COMPONENT, WARNING, "|FAILED_DELETE_COMPONENTS|"},
    {FAILED_UPDATE_ACCELERATION, SYSTEM, WARNING, "|FAILED_UPDATE_ACCELERATION|"},
    {ACCELERATING_MASSLESS_ENTITY, SYSTEM, WARNING, "|ACCELERATING_MASSLESS_ENTITY|"},
    {INCOMPATABLE_COMPONENTS, COMPONENT, WARNING, "|INCOMPATABLE_COMPONENTS|"},
};
const uint64_t error_info_size = sizeof(error_info)/sizeof(error_info[0]);

ErrorReport error_generate_report(ErrorCode code) {
    ErrorReport error_report = {0};
    for (int i = 0; i < error_info_size; i++){
        if( (error_info[i].code & code) == error_info[i].code) {
            switch (error_info[i].level) {
                case ENTITY:
                    tools_append_string(
                            error_info[i].string,
                            error_report.entity.report,
                            tool_sizeof_string(error_info[i].string, '\0'),
                            MAX_ENTITY_REPORT
                    );
                    break;
                case SYSTEM:
                    tools_append_string(
                            error_info[i].string,
                            error_report.system.report,
                            tool_sizeof_string(error_info[i].string, '\0'),
                            MAX_SYSTEM_REPORT
                    );
                    break;
                case COMPONENT:
                    tools_append_string(
                            error_info[i].string,
                            error_report.component.report,
                            tool_sizeof_string(error_info[i].string, '\0'),
                            MAX_COMPONENT_REPORT
                    );
                    break;
                case CORE:
                    tools_append_string(
                            error_info[i].string,
                            error_report.engine.report,
                            tool_sizeof_string(error_info[i].string, '\0'),
                            MAX_ENGINE_REPORT
                    );
                    break;
            }
        }
    }
    return error_report;
}

ErrorSeverity error_severity_level(ErrorCode code) {
    ErrorSeverity severity = WARNING;
    for (int i = 0; i < error_info_size; i++) {
        if( (error_info[i].code & code) == error_info[i].code) {
            if(error_info[i].severity > severity)
                severity = error_info[i].severity;
        }
    }
    return severity;
}

void error_add_entity(Error* err, Entity entity) {
    if(err->entities.entity_amount <= MAX_ENTITIES) {
        err->entities.concerned_entities[err->entities.entity_amount] = entity;
        err->entities.entity_amount += 1;
    }
}

void error_print_entities(EntityList entities) {
    console_write(ERROR, "Entities: { ");
    if(entities.entity_amount > 0) {
        for(int i = 0; i <= entities.entity_amount - 1; i++) {
            console_write(ERROR, "%d ", entities.concerned_entities[i]);
        }
    console_write(ERROR, "}\n");
    }
}

void error_print(Error err) {
    if(err.code != 0) {
        err.severity = error_severity_level(err.code);
        err.report = error_generate_report(err.code);

        char bit_flags[33] = {0};
        binary_to_string(err.code, bit_flags, sizeof(bit_flags));

        console_write(ERROR, "---Error---\n");
        console_write(ERROR, "(ErrorSummary)\n");
        console_write(ERROR, "ErrorCode: %d\n", err.code);
        console_write(ERROR, "ErrorFlags: %s\n", bit_flags);
        console_write(ERROR, "Severity: %d\n", err.severity);
        console_write(ERROR,"\n");
        console_write(ERROR, "(ErrorReport)\n");
        console_write(ERROR, "EntityReport: %s\n", err.report.entity.report);
        console_write(ERROR, "ComponentReport: %s\n", err.report.component.report);
        console_write(ERROR, "SystemReport: %s\n", err.report.system.report);
        console_write(ERROR, "EngineReport: %s\n", err.report.engine.report);
        error_print_entities(err.entities);
        console_write(ERROR, "---Error---\n");
    }
}
