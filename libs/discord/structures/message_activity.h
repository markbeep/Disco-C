#ifndef DISCO_MESSAGE_ACTIVITY
#define DISCO_MESSAGE_ACTIVITY

#include "../../utils/cJSON.h"

struct discord_message_activity {
};

// TODO implement
struct discord_message_activity *disco_create_message_activity_struct_json(cJSON *data);
void disco_destroy_message_activity(struct discord_message_activity *activity);

#endif