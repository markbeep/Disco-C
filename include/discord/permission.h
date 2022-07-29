#ifndef DISCO_PERMISSION
#define DISCO_PERMISSION

#include <cJSON/cJSON.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Permission
 * Permission functions and structures
 * @{
 *
 */

/** https://discord.com/developers/docs/resources/channel#overwrite-object */
struct discord_overwrite {
    char *id;
    int type;
    char *allow;
    char *deny;
};

/**
 * @brief Creates a permission overwrite structure from a given JSON
 *
 * @param data
 * @return void* discord_overwrite struct
 */
void *_d_json_to_overwrite(cJSON *data);

/**
 * @brief Copies an overwrite structure.
 *
 * @param src Overwrite to copy.
 * @return struct discord_overwrite*
 */
struct discord_overwrite *_d_copy_overwrite(struct discord_overwrite *src);

/**
 * @brief Destroys the given structure and frees the pointer.
 *
 * @param overwrite
 */
void discord_destroy_overwrite(struct discord_overwrite *overwrite);

/** @} @} */
#endif
