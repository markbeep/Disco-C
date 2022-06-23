#ifndef DISCO_MESSAGE_ACTIVITY
#define DISCO_MESSAGE_ACTIVITY

#include <cJSON/cJSON.h>
#include <stdint.h>

enum Discord_Message_Activity_Type { MESSAGE_ACTIVITY_JOIN = 1,
                                     MESSAGE_ACTIVITY_SPECTATE = 2,
                                     MESSAGE_ACTIVITY_LISTEN = 3,
                                     MESSAGE_ACTIVITY_JOIN_REQUEST = 5 };

struct discord_message_activity {
    enum Discord_Message_Activity_Type type;
    uint64_t party_id;
};

// TODO implement
void *disco_create_message_activity_struct_json(cJSON *data);
void disco_destroy_message_activity(struct discord_message_activity *activity);

#endif
