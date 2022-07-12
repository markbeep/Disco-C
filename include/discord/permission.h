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

// https://discord.com/developers/docs/resources/channel#overwrite-object
struct discord_overwrite {
    char *id;
    int type;
    char *allow;
    char *deny;
};

// TODO implement
void *discord_create_overwrite_struct_json(cJSON *data);
void discord_destroy_overwrite(struct discord_overwrite *overwrite);

/** @} @} */
#endif
