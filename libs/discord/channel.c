#include "./structures/channel.h"
#include "../web/request.h"
#include "./structures/message.h"
#include "disco.h"
#include <curl/curl.h>

// TODO implement
struct discord_channel *disco_create_channel_struct_json(cJSON *data) {
    (void)data;
}

void disco_destroy_channel(struct discord_channel *channel) {
    (void)channel;
    return NULL;
}

// TODO implement
struct discord_channel_mention *disco_create_channel_mention_struct_json(cJSON *data) {
    (void)data;
    return NULL;
}

void disco_destroy_channel_mention(struct discord_channel_mention *channel) {
    (void)channel;
}