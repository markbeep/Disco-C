#ifndef EXAMPLE_SLASH
#define EXAMPLE_SLASH

#include <discord/disco.h>
#include <discord/interaction.h>

void hello_callback(bot_client_t *bot, struct discord_interaction *interaction);
int register_hello_command(const char *token);

#endif
