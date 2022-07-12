#ifndef DISCO_USER
#define DISCO_USER

#include <cJSON/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup User
 * User functions and structures
 * @{
 *
 */

// https://discord.com/developers/docs/resources/user#user-object
struct discord_user {
    uint64_t id;
    char *username;
    char *discriminator;
    char *avatar;
    bool bot;
    bool system;
    bool mfa_enabled;
    char *banner;
    int accent_color;
    char *locale;
    bool verified;
    char *email;
    int flags;
    int premium_type;
    int public_flags;
};

/**
 * @brief Creates a discod_user struct with the passed in JSON
 *
 * @param data The Discord JSON for a user
 * @return struct discord_user*
 */
void *discord_create_user_struct_json(cJSON *data);

/**
 * @brief Destroys the discord_user structs and frees the passed in pointer
 *
 * @param user
 */
void discord_destroy_user(struct discord_user *user);

/** @}
 * @addtogroup Member
 * Member functions and structures
 * @{
 */

// https://discord.com/developers/docs/resources/guild#guild-member-object
struct discord_member {
    struct discord_user *user;
    char *nick;
    char *avatar;
    char **roles;
    int roles_count;
    char *joined_at;
    char *premium_since;
    bool deaf;
    bool mute;
    bool pending;
    char *permissions;
    char *communication_disabled_until;
};

// TODO comment
void *discord_create_member_struct_json(cJSON *data, struct discord_user *user);
void discord_destroy_member(struct discord_member *member);

/** @} @} */
#endif
