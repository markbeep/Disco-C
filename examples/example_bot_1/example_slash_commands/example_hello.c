#include "example_slash.h"
#include <discord/command.h>
#include <string.h>
#include <utils/disco_logging.h>

void hello_callback(bot_client_t *bot, struct discord_interaction *interaction) {
    struct discord_interaction_callback callback = {
        .type = DISCORD_CALLBACK_CHANNEL_MESSAGE_WITH_SOURCE,
        .data.message.content = "Hello there",
    };
    discord_send_interaction(bot, &callback, interaction);
}

int register_hello_command(const char *token) {
    struct discord_application_command command = {0};
    command.name = "hello";
    command.description = "Replies with \"Hello\"";
    command.type = COMMAND_CHAT_INPUT;
    command.guild_id = 944968090490380318L;
    return discord_command_register(&command, token);
}
