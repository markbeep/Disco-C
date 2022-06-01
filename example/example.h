#include "../libs/discord/disco.h"
#include "../libs/discord/structures/message.h"

void example_on_ready(bot_client_t *bot);
void example_on_message(bot_client_t *bot, struct discord_message *message);
void example_on_edit(bot_client_t *bot, struct discord_message *old, struct discord_message *new);
