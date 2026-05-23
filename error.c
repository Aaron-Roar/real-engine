#include "error.h"
#include "tools.h"
#include "stdio.h"


static const ErrorInfo error_info[] = {
    {ENTITY_RANGE_EXCEEDED, ENTITY, MINIMAL, "|ENTITY_RANGE_EXCEEDED|"},
    {ENTITY_DOES_NOT_EXIST, ENTITY, WARNING,"|ENTITY_DOES_NOT_EXIST|"},
    {FAILED_ADD_ENTITY, ENTITY, WARNING, "|FAILED_ADD_ENTITY|"},
    {FAILED_DELETE_ENTITY, ENTITY, WARNING, "|FAILED_DELETE_ENTITY|"},
    {FAILED_ADD_COMPONENTS, COMPONENT, WARNING, "|FAILED_ADD_COMPONENTS|"},
    {FAILED_DELETE_COMPONENTS, COMPONENT, WARNING, "|FAILED_DELETE_COMPONENTS|"},
    {FAILED_UPDATE_ACCELERATION, SYSTEM, WARNING, "|FAILED_UPDATE_ACCELERATION|"},
    {ACCELERATING_MASSLESS_ENTITY, SYSTEM, WARNING, "|ACCELERATING_MASSLESS_ENTITY|"},
    {ERROR_CODE_DOES_NOT_EXIST, ENGINE, CRITICAL, "|ERROR_CODE_DOES_NOT_EXIST|"},
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
                case ENGINE:
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

void error_print(ErrorCode code) {
    Error error = {0};
    error.code = code;
    error.severity = error_severity_level(code);
    error.report = error_generate_report(code);

    char bit_flags[33] = {0};
    binary_to_string(code, bit_flags, sizeof(bit_flags));

    printf("[!]Error Begin\n-----------------------\n");
    printf("(ErrorSummary)\n");
    printf("[!]Error: %d\n[!]ErrorFlags: %s\n[!]Severity(0-2): %d\n", error.code, bit_flags, error.severity);
    printf("\n(ErrorReport)\n");
    printf("[!]EntityReport: %s\n[!]ComponentReport: %s\n[!]SystemReport: %s\n[!]EngineReport: %s\n", error.report.entity.report, error.report.component.report, error.report.system.report, error.report.engine.report);
    printf("-----------------------\n[!]Error End\n");
}

int main() {
    error_print(ENTITY_DOES_NOT_EXIST | FAILED_UPDATE_ACCELERATION | ACCELERATING_MASSLESS_ENTITY);
}
