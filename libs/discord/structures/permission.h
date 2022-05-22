#ifndef DISCO_PERMISSION
#define DISCO_PERMISSION

#include "../../utils/cJSON.h"

// https://discord.com/developers/docs/resources/channel#overwrite-object
struct discord_overwrite {
    char *id;
    int type;
    char *allow;
    char *deny;
};

// TODO implement
void *disco_create_overwrite_struct_json(cJSON *data);
void disco_destroy_overwrite(struct discord_overwrite *overwrite);

#endif
