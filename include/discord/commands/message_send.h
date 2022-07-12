#ifndef DISCO_MESSAGE_SEND
#define DISCO_MESSAGE_SEND

#include <discord/disco.h>
#include <discord/message.h>

/** \addtogroup Message @{ */

/**
 * @brief Sends a message and if enabled also returns the sent message struct.
 *
 * @param bot Active bot instance
 * @param content Message content to send (if empty, embed or components need to exist)
 * @param channel_id ID of the channel the message should be sent in
 * @param message A discord_create_message struct. Can be NULL
 * @param return_struct Bool if the sent message should be returned or not (if yes,
 *                      it has to be destroyed with discord_destroy_message() after)
 * @return struct discord_message* NULL or the struct of the sent message
 */
struct discord_message *discord_channel_send_message(bot_client_t *bot, char *content, uint64_t channel_id, struct discord_create_message *message, bool return_struct);
void discord_channel_edit_message(bot_client_t *bot, char *content, uint64_t channel_id, uint64_t message_id, struct discord_create_message *message);

/** @} */
#endif
