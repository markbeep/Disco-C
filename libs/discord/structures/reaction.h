#ifndef DISCO_REACTION
#define DISCO_REACTION

#include "emoji.h"
#include <cJSON/cJSON.h>

struct discord_reaction {
    int count;
    int me;
    struct discord_emoji *emoji;
};

// TODO implement
void *disco_create_reaction_struct_json(cJSON *data);
void disco_destroy_reaction(struct discord_reaction *reaction);

#endif
