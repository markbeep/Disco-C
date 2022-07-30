#ifndef DISCO_EMOJI
#define DISCO_EMOJI

#include <cJSON/cJSON.h>
#include <stdbool.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Emoji
 * Emoji functions and structures
 * @{
 *
 */

/** https://discord.com/developers/docs/resources/emoji#emoji-object */
struct discord_emoji {
    char *id;
    char *name;
    char **roles;
    int roles_count;
    struct discord_user *user;
    bool require_colons;
    bool managed;
    bool animated;
    bool available;
};

/**
 * @brief Creates an emoji from a given JSON
 *
 * @param data
 * @return void* discord_emoji struct
 */
void *_d_json_to_emoji(cJSON *data);

/**
 * @brief Copies an emoji structure.
 *
 * @param src Emoji to copy.
 * @return struct discord_emoji*
 */
struct discord_emoji *_d_copy_emoji(struct discord_emoji *src);

/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param emoji
 */
void discord_destroy_emoji(struct discord_emoji *emoji);

/** @} @} */
#endif
