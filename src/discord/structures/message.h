#ifndef DISCO_MESSAGE
#define DISCO_MESSAGE

#include "../disco.h"
#include "attachment.h"
#include "channel.h"
#include "component.h"
#include "embed.h"
#include "message_activity.h"
#include "reaction.h"
#include "sticker.h"
#include "user.h"
#include <stdbool.h>

typedef struct bot_client bot_client_t;

// https://discord.com/developers/docs/resources/channel#allowed-mentions-object-allowed-mentions-structure
struct discord_allowed_mentions {
    int parse_role_mentions;     // if roles should be mentioned
    int parse_user_mentions;     // if users should be mentioned
    int parse_everyone_mentions; // if @everyone should mention
    char **roles;
    int roles_count;
    char **users;
    int users_count;
    bool replied_user;
};

// https://discord.com/developers/docs/resources/channel#message-reference-object
struct discord_message_reference {
    char *message_id;
    char *channel_id;
    char *guild_id;
    bool fail_if_not_exists;
};

/**
 * @brief Creates a message_reference structure from a given JSON
 *
 * @param data
 * @return void* discord_message_reference struct
 */
void *disco_create_message_reference_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param message
 */
void disco_destroy_message_reference(struct discord_message_reference *message);

// https://discord.com/developers/docs/resources/channel#create-message
struct discord_create_message {
    bool tts;
    struct discord_embed **embeds;
    int embeds_count;
    struct discord_allowed_mentions *allowed_mentions;
    struct discord_message_reference *message_reference;
    struct discord_component **components;
    int components_count;
    char **sticker_ids;
    int sticker_ids_count;
    struct discord_attachment **attachments;
    int attachments_count;
    int flags;
};

// https://discord.com/developers/docs/resources/channel#message-object-message-types
enum discord_message_type { DEFAULT,
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
    char *id;
    char *channel_id;
    char *guild_id;
    struct discord_user *author;
    struct discord_member *member;
    char *content;
    char *timestamp;
    char *edited_timestamp;
    bool tts;
    bool mention_everyone;
    struct discord_member **mentions;
    int mentions_count;
    char **mention_roles;
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
    char *webhook_id;
    enum discord_message_type type;
    struct discord_message_activity *activity;
    struct discord_application *application;
    char *application_id;
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
 * @brief Creates a message structure from a given JSON
 *
 * @param data
 * @return void* discord_message struct
 */
void *disco_create_message_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param message
 */
void disco_destroy_message(struct discord_message *message);

/**
 * @brief Sends a message and if enabled also returns the sent message struct
 *
 * @param bot Active bot instance
 * @param content Message content to send (if empty, embed or components need to exist)
 * @param channel_id ID of the channel the message should be sent in
 * @param message A discord_create_message struct. Can be NULL
 * @param return_struct Bool if the sent message should be returned or not (if yes,
 *                      it has to be destroyed with disco_destroy_message() after)
 * @return struct discord_message* NULL or the struct of the sent message
 */
struct discord_message *disco_channel_send_message(bot_client_t *bot, char *content, char *channel_id, struct discord_create_message *message, bool return_struct);
void disco_channel_edit_message(bot_client_t *bot, char *content, char *channel_id, char *message_id, struct discord_create_message *message);

#endif
