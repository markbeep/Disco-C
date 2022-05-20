#ifndef DISCO_CHANNEL
#define DISCO_CHANNEL

#include "permission.h"
#include "thread.h"
#include "user.h"

// https://discord.com/developers/docs/resources/channel#channel-object-channel-types
enum discord_channel_type { GUILD_TEXT,
                            GUILD_VOICE,
                            GROUP_DM,
                            GUILD_CATEGORY,
                            GUILD_NEWS = 5,
                            GUILD_NEWS_THREAD = 10,
                            GUILD_PUBLIC_THREAD,
                            GUILD_PRIVATE_THREADly,
                            GUILD_STAGE_VOICE,
                            GUILD_DIRECTORY,
                            GUILD_FORUM };

// https://discord.com/developers/docs/resources/channel#channel-object
struct discord_channel {
    char *id;
    enum discord_channel_type type;
    char *guild_id;
    int position;
    struct discord_overwrite **permission_overwrites;
    int permission_overwrites_count;
    char *name;
    char *topic;
    int nsfw;
    char *last_message_id;
    int bitrate;
    int user_limit;
    int rate_limit_per_user;
    struct discord_user **recipients;
    int recipients_count;
    char *icon;
    char *owner_id;
    char *application_id;
    char *parent_id;
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

struct discord_channel_mention {
    char *id;
    char *guild_id;
    int type;
    char *name;
};

// TODO comments
void *disco_create_channel_struct_json(cJSON *data);
void disco_destroy_channel(struct discord_channel *channel);

void *disco_create_channel_mention_struct_json(cJSON *data);
void disco_destroy_channel_mention(struct discord_channel_mention *channel);

#endif
