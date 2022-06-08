#ifndef DISCO_EMOJI
#define DISCO_EMOJI

#include "user.h"
#include <cJSON/cJSON.h>

// https://discord.com/developers/docs/resources/emoji#emoji-object
struct discord_emoji {
    char *id;
    char *name;
    char **roles;
    int roles_count;
    struct discord_user *user;
    int require_colons;
    int managed;
    int animated;
    int available;
};

void *disco_create_emoji_struct_json(cJSON *data);
void disco_destroy_emoji(struct discord_emoji *emoji);

#endif
