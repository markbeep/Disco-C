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

/** https://discord.com/developers/docs/resources/user#user-object */
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
 * @brief Creates a discord_user struct with the passed in JSON
 *
 * @param data The Discord JSON for a user
 * @return struct discord_user*
 */
void *_d_json_to_user(cJSON *data);

/**
 * @brief Copies a user structure.
 *
 * @param user User to copy.
 * @return struct discord_user*
 */
struct discord_user *_d_copy_user(struct discord_user *user);

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

/**
 * @brief Discord guild member structure.
 * This structure is mostly available on events that are given inside of guilds.
 * Events like a message being created inside a guild.
 * Discord docs: https://discord.com/developers/docs/resources/guild#guild-member-object
 */
struct discord_member {
    struct discord_user *user;          ///< User structure if given.
    char *nick;                         ///< Guild nickname. NULL if member has no nickname.
    char *avatar;                       ///< Avatar hash if the member has a avatar.
    uint64_t *roles;                    ///< List of roles the member has
    int roles_count;                    ///< Amount of roles the member has.
    char *joined_at;                    ///< ISO8601 timestamp when the user joined.
    char *premium_since;                ///< ISO8601 timestamp when the user started boosting the server.
    bool deaf;                          ///< If the user is deafened inside a voice channel
    bool mute;                          ///< If the user is muted inside a voice channel
    bool pending;                       ///< whether the user has not yet passed the guild's Membership Screening requirements.
    char *permissions;                  ///< total permissions of the member in the channel.
    char *communication_disabled_until; ///< ISO8601 timestamp when the user's timeout will end
};

/**
 * @brief Creates a discord_member struct with the passed in JSON.
 *
 * @param data The Discord JSON for a member.
 * @return struct discord_member*
 */
void *_d_json_to_member(cJSON *data, struct discord_user *user);

/**
 * @brief Copies a member structure.
 *
 * @param member Member to copy.
 * @param user User to add to the member struct. Doesn't get copied again.
 * @return struct discord_member*
 */
struct discord_member *_d_copy_member(struct discord_member *member, struct discord_user *user);

void discord_destroy_member(struct discord_member *member);

/** @} @} */
#endif
