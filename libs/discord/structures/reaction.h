#ifndef DISCO_REACTION
#define DISCO_REACTION

#include "../../utils/cJSON.h"

struct discord_reaction {
};

// TODO implement
struct discord_reaction *disco_create_reaction_struct_json(cJSON *data);
void disco_destroy_reaction(struct discord_reaction *reaction);

#endif