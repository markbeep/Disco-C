#include "../../src/discord/structures/command.h"
#include "../../src/discord/structures/interaction.h"
#include "../../src/utils/disco_logging.h"
#include <string.h>

void hello_callback(struct discord_interaction *interaction) {
    struct discord_interaction_callback callback = {
        .type = DISCORD_CALLBACK_CHANNEL_MESSAGE_WITH_SOURCE,
        .data.message.content = "Hello there",
    };
    disco_send_interaction(&callback, interaction);
}

int register_hello_command() {
    struct discord_application_command command = {0};
    command.name = "hello";
    command.description = "Replies with \"Hello\"";
    command.type = COMMAND_CHAT_INPUT;
    command.guild_id = 944968090490380318L;
    return discord_command_register(&command);
}
