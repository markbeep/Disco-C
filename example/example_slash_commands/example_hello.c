#include "../../src/discord/structures/command.h"
#include <string.h>

void hello_callback() {
}

int register_hello_command() {
    struct discord_application_command command = {0};
    command.name = "hello";
    command.description = "Replies with \"Hello\"";
    command.type = COMMAND_CHAT_INPUT;
    command.guild_id = 944968090490380318L;
    return discord_command_register(&command);
}
