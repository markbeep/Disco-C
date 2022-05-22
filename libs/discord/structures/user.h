#ifndef DISCO_USER
#define DISCO_USER

#include "../../utils/cJSON.h"

// https://discord.com/developers/docs/resources/user#user-object
struct discord_user {
    char *id;
    char *username;
    char *discriminator;
    char *avatar;
    int bot;
    int system;
    int mfa_enabled;
    char *banner;
    int accent_color;
    char *locale;
    int verified;
    char *email;
    int flags;
    int premium_type;
    int public_flags;
};

// https://discord.com/developers/docs/resources/guild#guild-member-object
struct discord_member {
    struct discord_user *user;
    char *nick;
    char *avatar;
    char **roles;
    int roles_count;
    char *joined_at;
    char *premium_since;
    int deaf;
    int mute;
    int pending;
    char *permissions;
    char *communication_disabled_until;
};

/**
 * @brief Creates a discod_user struct with the passed in JSON
 *
 * @param data The Discord JSON for a user
 * @return struct discord_user*
 */
void *disco_create_user_struct_json(cJSON *data);

/**
 * @brief Destroys the discord_user structs and frees the passed in pointer
 *
 * @param user
 */
void disco_destroy_user(struct discord_user *user);

// TODO comment
void *disco_create_member_struct_json(cJSON *data, struct discord_user *user);
void disco_destroy_member(struct discord_member *member);

#endif
