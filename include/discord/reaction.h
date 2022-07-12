#ifndef DISCO_REACTION
#define DISCO_REACTION

#include "emoji.h"
#include <cJSON/cJSON.h>
#include <stdbool.h>

/**
 * \addtogroup Reaction
 * Reaction functions and structures
 * @{
 *
 */

// https://discord.com/developers/docs/resources/channel#reaction-object
struct discord_reaction {
    int count;
    bool me;
    struct discord_emoji *emoji;
};

// TODO implement
void *discord_create_reaction_struct_json(cJSON *data);
void discord_destroy_reaction(struct discord_reaction *reaction);

/** @}*/
#endif
