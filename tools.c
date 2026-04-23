#include <time.h>
#include "entity_components.h"

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}

#define MAX_COMBINED_STRING 250
typedef struct {
    char string[MAX_COMBINED_STRING];
    size_t length;
} CombinedString;

