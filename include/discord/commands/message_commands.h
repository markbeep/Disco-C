#ifndef DISCO_MESSAGE_COMMANDS
#define DISCO_MESSAGE_COMMANDS

#include <discord/disco.h>
#include <discord/message.h>

/** @addtogroup Commands
 * @{
 * @addtogroup Message_Commands
 * @{
 */

/**
 * @brief Discord Message Callback.
 * Can be added to a for example discord_channel_send_message function
 * to call the callback once the message was received.
 *
 */
struct discord_message_cb {
    void *data; ///< Data that will be passed along the callback function.
    void (*cb)(bot_client_t *, struct discord_message *, void *data);
};

struct discord_multiple_message_cb {
    void *data;
    void (*cb)(bot_client_t *, size_t size, struct discord_message **arr, void *data);
};

/**
 * @brief Sends a message and if enabled also returns the sent message struct.
 *
 * @param bot Active bot instance.
 * @param content Message content to send (if empty, embed or components need to exist).
 * @param channel_id ID of the channel the message should be sent in.
 * @param message A discord_create_message struct. Can be NULL.
 * @param cb discord_message_cb struct which contains a callback that will be called with the given data. Can be NULL to ignore.
 */
void discord_channel_send_message(bot_client_t *bot, char *content, uint64_t channel_id, struct discord_create_message *message, struct discord_message_cb *cb);

/**
 * @brief Edit a message
 *
 * @param bot Active bot instance.
 * @param content Message content to edit to.
 * @param channel_id Channel ID of the message.
 * @param message_id Message ID to edit.
 * @param message struct discord_create_message Create message to edit to. Can be NULL.
 */
void discord_channel_edit_message(bot_client_t *bot, char *content, uint64_t channel_id, uint64_t message_id, struct discord_create_message *message);

/**
 * @brief Returns an array of Discord message structs.
 * The parameters around, before and after are mutually exclusive, only
 * one may be passed at a time. All the retrieved messages will be placed
 * into the cache.
 * NOTE: Messages don't have to be destroyed as they are all in the cache.
 * Only the array itself has to be freed.
 * @param bot Active bot instance.
 * @param channel_id Channel to fetch messages from.
 * @param limit Max number of messages to return (1-100). 0 to ignore and use the default of 50.
 * @param around Get messages around this message ID. 0 to ignore.
 * @param before Get messages before this message ID. 0 to ignore.
 * @param after Get messages after this message ID. 0 to ignore.
 * @param cb Callback struct which is called once ther request is done.
 */
void discord_get_messages(bot_client_t *bot, uint64_t channel_id, int limit, uint64_t around, uint64_t before, uint64_t after, struct discord_multiple_message_cb *cb);

/**
 * @brief Returns a specific message by channel and message ID.
 * This function first checks the cache and returns whatever is in the cache if it exists.
 * This function has the upside that deleted messages can be retrieved if they're
 * still in the cache.
 * @param bot Active bot instance.
 * @param channel_id Channel to fetch message from.
 * @param message_id Message to fetch.
 */
void discord_get_message(bot_client_t *bot, uint64_t channel_id, uint64_t message_id, struct discord_message_cb *cb);

/**
 * @brief Fetches the message from Discord without first checking if it's in the cache.
 * The message will be added to the cache if it doesn't exist yet. discord_get_message
 * is preferred over this fetch function, because this will force a new entry to the cache
 * on every call.
 * Deleted messages won't be retrieved.
 * @param bot Active bot instance.
 * @param channel_id Channel to fetch message from.
 * @param message_id Message to fetch.
 */
void discord_fetch_message(bot_client_t *bot, uint64_t channel_id, uint64_t message_id, struct discord_message_cb *cb);

/** @} @} */
#endif
