#ifndef DISCO_MESSAGE
#define DISCO_MESSAGE

#include "attachment.h"
#include "channel.h"
#include "component.h"
#include "embed.h"
#include "message_activity.h"
#include "reaction.h"
#include "sticker.h"
#include "user.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct bot_client bot_client_t;

/**
 * @addtogroup Models
 * @{
 * @addtogroup Message
 * Message functions and structures
 * @{
 *
 */

// https://discord.com/developers/docs/resources/channel#allowed-mentions-object-allowed-mentions-structure
struct discord_allowed_mentions {
    int parse_role_mentions;     ///< if roles should be mentioned
    int parse_user_mentions;     ///< if users should be mentioned
    int parse_everyone_mentions; ///< if @everyone should mention
    uint64_t *roles;
    int roles_count;
    uint64_t *users;
    int users_count;
    bool replied_user;
};

// https://discord.com/developers/docs/resources/channel#message-reference-object
struct discord_message_reference {
    uint64_t message_id;
    uint64_t channel_id;
    uint64_t guild_id;
    bool fail_if_not_exists;
};

/**
 * @brief Creates a message_reference structure from a given JSON.
 *
 * @param data
 * @return void* discord_message_reference struct.
 */
void *_d_json_to_message_reference(cJSON *data);

/**
 * @brief Copies a message reference structure.
 *
 * @param message Message reference to copy.
 * @return struct discord_message_reference*
 */
struct discord_message_reference *_d_copy_message_reference(struct discord_message_reference *src);

/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param message
 */
void discord_destroy_message_reference(struct discord_message_reference *message);

// https://discord.com/developers/docs/resources/channel#create-message
struct discord_create_message {
    bool tts;
    struct discord_embed *embed; // for single embeds this can be used
    struct discord_embed **embeds;
    int embeds_count;
    struct discord_allowed_mentions *allowed_mentions;
    struct discord_message_reference *message_reference;
    struct discord_component **components;
    int components_count;
    uint64_t *sticker_ids;
    int sticker_ids_count;
    struct discord_attachment **attachments;
    int attachments_count;
    int flags;
};

// https://discord.com/developers/docs/resources/channel#message-object-message-types
enum Discord_Message_Type { DEFAULT,
                            RECIPIENT_ADD,
                            RECIPIENT_REMOVE,
                            CALL,
                            CHANNEL_NAME_CHANGE,
                            CHANNEL_ICON_CHANGE,
                            CHANNEL_PINNED_MESSAGE,
                            GUILD_MEMBER_JOIN,
                            USER_PREMIUM_GUILD_SUBSCRIPTION,
                            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1,
                            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2,
                            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3,
                            CHANNEL_FOLLOW_ADD = 12,
                            GUILD_DISCOVERY_DISQUALIFIED = 14,
                            GUILD_DISCOVERY_REQUALIFIED,
                            GUILD_DISCOVERY_GRACE_PERIOD_INITIAL_WARNING,
                            GUILD_DISCOVERY_GRACE_PERIOD_FINAL_WARNING,
                            THREAD_CREATED,
                            REPLY,
                            CHAT_INPUT_COMMAND,
                            THREAD_STARTER_MESSAGE,
                            GUILD_INVITE_REMINDER,
                            CONTEXT_MENU_COMMAND };

// https://discord.com/developers/docs/resources/channel#message-object
struct discord_message {
    uint64_t id;
    uint64_t channel_id;
    uint64_t guild_id;
    struct discord_user *user;
    struct discord_member *member;
    char *content;
    char *timestamp;
    char *edited_timestamp;
    bool tts;
    bool mention_everyone;
    struct discord_member **mentions;
    int mentions_count;
    uint64_t *mention_roles;
    int mention_roles_count;
    struct discord_channel **mention_channels;
    int mention_channels_count;
    struct discord_attachment **attachments;
    int attachments_count;
    struct discord_embed **embeds;
    int embeds_count;
    struct discord_reaction **reactions;
    int reactions_count;
    char *nonce;
    bool pinned;
    uint64_t webhook_id;
    enum Discord_Message_Type type;
    struct discord_message_activity *activity;
    struct discord_application *application;
    uint64_t application_id;
    struct discord_message_reference *message_reference;
    int flags;
    struct discord_message *referenced_message;
    struct discord_interaction *interaction;
    struct discord_channel *thread;
    struct discord_component **components;
    int components_count;
    struct discord_message_sticker_item **sticker_items;
    int sticker_items_count;
    struct discord_sticker **stickers;
    int stickers_count;
};

/**
 * @brief Creates a message structure from a given JSON.
 *
 * @param data
 * @return void* discord_message struct
 */
void *_d_json_to_message(cJSON *data);

/**
 * @brief Copies a message structure.
 *
 * @param message Message to copy.
 * @return struct discord_message*
 */
struct discord_message *_d_copy_message(struct discord_message *message);

/**
 * @brief Destroys the given structure and frees the pointer.
 *
 * @param message
 */
void discord_destroy_message(struct discord_message *message);

/**
 * @brief Fills in a JSON with the attributes of the message structure.
 *
 * @param json JSON struct that should be filled in with the values.
 * @param content Discord message content.
 * @param message Create message struct.
 */
void _d_create_message_to_json(cJSON *json, char *content, struct discord_create_message *message);

/** @} @} */
#endif
