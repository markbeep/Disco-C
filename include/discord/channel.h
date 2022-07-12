#ifndef DISCO_CHANNEL
#define DISCO_CHANNEL

#include "permission.h"
#include "thread.h"
#include "user.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Channel
 * Channel functions and structures
 * @{
 *
 */

// https://discord.com/developers/docs/resources/channel#channel-object-channel-types
enum Discord_Channel_Type { CHANNEL_GUILD_TEXT = 0,
                            CHANNEL_DM = 1,
                            CHANNEL_GROUP_DM = 1,
                            CHANNEL_GUILD_VOICE = 2,
                            CHANNEL_GUILD_CATEGORY = 4,
                            CHANNEL_GUILD_NEWS = 5,
                            CHANNEL_GUILD_NEWS_THREAD = 10,
                            CHANNEL_GUILD_PUBLIC_THREAD = 11,
                            CHANNEL_GUILD_PRIVATE_THREAD = 12,
                            CHANNEL_GUILD_STAGE_VOICE = 13,
                            CHANNEL_GUILD_DIRECTORY = 14,
                            CHANNEL_GUILD_FORUM = 15 };

// https://discord.com/developers/docs/resources/channel#channel-object
struct discord_channel {
    uint64_t id;
    enum Discord_Channel_Type type;
    uint64_t guild_id;
    int position;
    struct discord_overwrite **permission_overwrites;
    int permission_overwrites_count;
    char *name;
    char *topic;
    bool nsfw;
    uint64_t last_message_id;
    int bitrate;
    int user_limit;
    int rate_limit_per_user;
    struct discord_user **recipients;
    int recipients_count;
    char *icon;
    uint64_t owner_id;
    uint64_t application_id;
    uint64_t parent_id;
    char *last_pin_timestamp;
    char *rtc_region;
    int video_quality_mode;
    int message_count;
    int member_count;
    struct discord_thread_metadata *thread_metadata;
    struct discord_thread_member *member;
    int default_auto_archive_duration;
    char *permissions;
    int flags;
};

// https://discord.com/developers/docs/resources/channel#channel-mention-object
struct discord_channel_mention {
    uint64_t id;
    uint64_t guild_id;
    int type;
    char *name;
};

/**
 * @brief Creates a channel structure from a given JSON
 *
 * @param data
 * @return void* discord_channel struct
 */
void *discord_create_channel_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param ch channel struct
 */
void discord_destroy_channel(struct discord_channel *ch);

/**
 * @brief Creates a channel_mention structure from a given JSON
 *
 * @param data
 * @return void* discord_channel_mention struct
 */
void *discord_create_channel_mention_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param ch channel_mention struct
 */
void discord_destroy_channel_mention(struct discord_channel_mention *ch);

/** @} @} */
#endif
