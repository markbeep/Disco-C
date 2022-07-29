#ifndef DISCO_MESSAGE_ACTIVITY
#define DISCO_MESSAGE_ACTIVITY

#include <cJSON/cJSON.h>
#include <stdint.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Message_Activity
 * Message Activity functions and structures
 * @{
 *
 */

enum Discord_Message_Activity_Type { MESSAGE_ACTIVITY_JOIN = 1,
                                     MESSAGE_ACTIVITY_SPECTATE = 2,
                                     MESSAGE_ACTIVITY_LISTEN = 3,
                                     MESSAGE_ACTIVITY_JOIN_REQUEST = 5 };

struct discord_message_activity {
    enum Discord_Message_Activity_Type type;
    uint64_t party_id;
};

/**
 * @brief Creates a message activity structure from a given JSON.
 *
 * @param data
 * @return void*
 */
void *_d_json_to_message_activity(cJSON *data);

/**
 * @brief Copies a message activity structure.
 *
 * @param activity Message activity to copy.
 * @return struct discord_message_activity*
 */
struct discord_message_activity *_d_copy_message_activity(struct discord_message_activity *activity);

/**
 * @brief Destroys the given structure.
 *
 * @param activity
 */
void discord_destroy_message_activity(struct discord_message_activity *activity);

/** @} @} */
#endif
