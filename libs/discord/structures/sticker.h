#ifndef DISCO_STICKER
#define DISCO_STICKER

#include "../../utils/cJSON.h"

struct discord_message_sticker_item {
};

// TODO implement
void *disco_create_message_sticker_item_struct_json(cJSON *data);
void disco_destroy_message_sticker(struct discord_message_sticker_item *message_sticker);

struct discord_sticker {
};

// TODO implement
void *disco_create_sticker_struct_json(cJSON *data);
void disco_destroy_sticker(struct discord_sticker *sticker);

#endif