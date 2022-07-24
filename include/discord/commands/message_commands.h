#ifndef DISCO_MESSAGE_COMMANDS
#define DISCO_MESSAGE_COMMANDS

#include <discord/disco.h>
#include <discord/message.h>

/** @addtogroup Commands
 * @{
 * @addtogroup Message
 * @{
 */

/**
 * @brief Sends a message and if enabled also returns the sent message struct.
 *
 * @param bot Active bot instance.
 * @param content Message content to send (if empty, embed or components need to exist).
 * @param channel_id ID of the channel the message should be sent in.
 * @param message A discord_create_message struct. Can be NULL.
 * @param return_struct Bool if the sent message should be returned or not (if yes,
 *                      it has to be destroyed with discord_destroy_message() after)
 * @return struct discord_message* NULL or the struct of the sent message.
 */
struct discord_message *discord_channel_send_message(bot_client_t *bot, char *content, uint64_t channel_id, struct discord_create_message *message, bool return_struct);

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
 * @param message_array Pointer to a message array. This will be modified.
 * @param limit Max number of messages to return (1-100). 0 to ignore and use the default of 50.
 * @param around Get messages around this message ID. 0 to ignore.
 * @param before Get messages before this message ID. 0 to ignore.
 * @param after Get messages after this message ID. 0 to ignore.
 * @return int Amount of messages in the array.
 */
int discord_get_messages(bot_client_t *bot, uint64_t channel_id, struct discord_message ***message_array, int limit, uint64_t around, uint64_t before, uint64_t after);

/**
 * @brief Returns a specific message by channel and message ID.
 * This function first checks the cache and returns whatever is in the cache if it exists.
 * This function has the upside that deleted messages can be retrieved if they're
 * still in the cache.
 * @param bot Active bot instance.
 * @param channel_id Channel to fetch message from.
 * @param message_id Message to fetch.
 * @return struct discord_message* Struct of the message or NULL if the message doesn't exist.
 */
struct discord_message *discord_get_message(bot_client_t *bot, uint64_t channel_id, uint64_t message_id);

/**
 * @brief Fetches the message from Discord without first checking if it's in the cache.
 * The message will be added to the cache if it doesn't exist yet. discord_get_message
 * is preferred over this fetch function, because this will force a new entry to the cache
 * on every call.
 * Deleted messages won't be retrieved.
 * @param bot Active bot instance.
 * @param channel_id Channel to fetch message from.
 * @param message_id Message to fetch.
 * @return struct discord_message* Struct of the message or NULL if the message doesn't exist.
 */
struct discord_message *discord_fetch_message(bot_client_t *bot, uint64_t channel_id, uint64_t message_id);

/** @} @} */
#endif
