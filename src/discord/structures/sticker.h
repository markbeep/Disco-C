#ifndef DISCO_STICKER
#define DISCO_STICKER

#include "user.h"
#include <cJSON/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

enum Discord_Sticker_Format { STICKER_FORMAT_PNG = 1,
                              STICKER_FORMAT_APNG = 2,
                              STICKER_FORMAT_LOTTIE = 3 };

// https://discord.com/developers/docs/resources/sticker#sticker-item-object
struct discord_message_sticker_item {
    char *id;
    char *name;
    enum Discord_Sticker_Format format_type;
};

// TODO implement
void *disco_create_message_sticker_item_struct_json(cJSON *data);
void disco_destroy_message_sticker(struct discord_message_sticker_item *message_sticker);

enum Discord_Sticker_Type { STICKER_TYPE_STANDARD = 1,
                            STICKER_TYPE_GUILD = 2 };

// https://discord.com/developers/docs/resources/sticker#sticker-object
struct discord_sticker {
    char *id;
    uint64_t pack_id;
    char *name;
    char *description;
    char *tags;
    enum Discord_Sticker_Type type;
    enum Discord_Sticker_Format format_type;
    bool available;
    char *guild_id;
    struct discord_user *user;
    int sort_value;
};

// TODO implement
void *disco_create_sticker_struct_json(cJSON *data);
void disco_destroy_sticker(struct discord_sticker *sticker);

#endif
