#include "platform_process.h"

FILE *platform_process_open_write(const char *command) {
#ifdef _WIN32
    return _popen(command, "wb");
#else
    return popen(command, "w");
#endif
}

int platform_process_close(FILE *process) {
#ifdef _WIN32
    return _pclose(process);
#else
    return pclose(process);
#endif
}
