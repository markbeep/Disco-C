#include "../src/discord/disco.h"
#include "../src/discord/structures/message.h"

void example_on_ready(bot_client_t *bot);
void example_on_message(bot_client_t *bot, struct discord_message *message);
void example_on_edit(bot_client_t *bot, struct discord_message *old, struct discord_message *new);
void example_on_delete(struct bot_client *bot, char *message_id, char *channel_id, char *guild_id, struct discord_message *message);
