#ifndef DISCO_GUILD
#define DISCO_GUILD

#include "emoji.h"
#include "role.h"
#include "sticker.h"
#include <stdbool.h>
#include <stdint.h>

// https://discord.com/developers/docs/resources/guild#guild-object
struct discord_guild {
    int64_t id;
    char *name;
    char *icon;
    char *icon_hash;
    char *splash;
    char *discovery_splash;
    bool owner;
    int64_t owner_id;
    char *permissions;
    char *region;
    int64_t afk_channel_id;
    int afk_timeout;
    bool widget_enabled;
    int64_t widget_channel_id;
    int verification_level;
    int default_message_notifications;
    int explicit_content_filter;
    struct discord_role **roles;
    int roles_count;
    struct discord_emoji **emojis;
    int emojis_count;
    int mfa_level;
    int64_t application_id;
    int64_t system_channel_id;
    int system_channel_flags;
    int64_t rules_channel_id;
    int max_presences;
    int max_members;
    char *vanity_url_code;
    char *description;
    char *banner;
    int premium_tier;
    int premium_subscription_count;
    char *preferred_locale;
    int64_t public_updates_channel_id;
    int max_video_channel_users;
    int approximate_member_count;
    int approximate_presence_count;
    // TODO welcome_screen currently excluded
    int nsfw_level;
    struct discord_sticker **stickers;
    int stickers_count;
    bool premium_progress_bar_enabled;
};

/**
 * @brief Creates a guild structure from a given JSON
 *
 * @param data
 * @return void* discord_guild struct
 */
void *disco_create_guild_struct_json(cJSON *data);

/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param ch guild struct
 */
void disco_destroy_guild(struct discord_guild *guild);

#endif
