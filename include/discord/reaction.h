#ifndef DISCO_REACTION
#define DISCO_REACTION

#include "emoji.h"
#include <cJSON/cJSON.h>
#include <stdbool.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Reaction
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

/**
 * @brief Creates a reaction structure from a given JSON.
 *
 * @param data
 * @return void* discord_reaction struct
 */
void *_d_json_to_reaction(cJSON *data);

/**
 * @brief Copies a reaction structure.
 *
 * @param reaction Reaction to copy.
 * @return struct discord_reaction*
 */
struct discord_reaction *_d_copy_reaction(struct discord_reaction *reaction);

/**
 * @brief Destroys the given structure and frees the pointer.
 *
 * @param reaction
 */
void discord_destroy_reaction(struct discord_reaction *reaction);

/** @} @} */
#endif
