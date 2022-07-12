#include <discord/command.h>
#include <discord/interaction.h>
#include <string.h>
#include <utils/disco_logging.h>

void hello_callback(struct discord_interaction *interaction) {
    struct discord_interaction_callback callback = {
        .type = DISCORD_CALLBACK_CHANNEL_MESSAGE_WITH_SOURCE,
        .data.message.content = "Hello there",
    };
    discord_send_interaction(&callback, interaction);
}

int register_hello_command(void) {
    struct discord_application_command command = {0};
    command.name = "hello";
    command.description = "Replies with \"Hello\"";
    command.type = COMMAND_CHAT_INPUT;
    command.guild_id = 944968090490380318L;
    return discord_command_register(&command);
}
