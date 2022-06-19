#include "../src/discord/disco.h"
#include "../src/discord/structures/message.h"

void example_on_ready(bot_client_t *bot);
void example_on_message(bot_client_t *bot, struct discord_message *message);
void example_on_edit(bot_client_t *bot, struct discord_message *old, struct discord_message *new);
void example_on_delete(bot_client_t *bot, char *message_id, char *channel_id, char *guild_id, struct discord_message *message);
void example_channel_create(bot_client_t *bot, struct discord_channel *channel);
void example_channel_update(bot_client_t *bot, struct discord_channel *old, struct discord_channel *new);
void example_channel_delete(bot_client_t *bot, char *channel_id, char *guild_id, char *parent_id, enum Discord_Channel_Type type, struct discord_channel *channel);
