#ifndef DISCO_COMPONENT
#define DISCO_COMPONENT

#include "../../utils/cJSON.h"

struct discord_component {
};

// TODO implement
struct discord_component *disco_create_component_struct_json(cJSON *data);
void disco_destroy_component(struct discord_component *component);

struct discord_interaction {
};

// TODO implement
struct discord_interaction *disco_create_interaction_struct_json(cJSON *data);
void disco_destroy_interaction(struct discord_interaction *interaction);

#endif