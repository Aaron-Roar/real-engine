#include "entity_components.h"
#include <stdbool.h>

bool entity_alive[MAX_ENTITIES] = {0}; //What entities are active
uint32_t entity_mask[MAX_ENTITIES] = {0}; //Bit map of the components each entity has

//Disasters
DisasterType disaster_type[MAX_ENTITIES] = {0};
DisasterSeverity disaster_serverity[MAX_ENTITIES] = {0};

//Moveable Objects
MotionType motion_type[MAX_ENTITIES] = {0};
Position positions[MAX_ENTITIES] = {0};
Velocity velocities[MAX_ENTITIES] = {0};
Acceleration accelerations[MAX_ENTITIES] = {0};
float mass[MAX_ENTITIES] = {0};
Entity targets[MAX_ENTITIES] = {0};
Force forces[MAX_ENTITIES] = {0};
TimeWindow time_windows[MAX_ENTITIES] = {0};

uint32_t entity_counter = 1; //Temporary solution. Leaks memory on entity deletion
Entity add_entity() {
    entity_alive[entity_counter] = 1;
    return entity_counter;
}

void delete_entity(Entity e) {
    entity_alive[e] = 0;
    entity_mask[e] = 0;
}

const int component_count = sizeof(component_names) / sizeof(component_names[0]);

void print_entity_components(Entity e) {
    printf("Entity #: %d\n", e);
    for (int i = 0; i < component_count; i++) {
    }
}
