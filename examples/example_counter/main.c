#include <config2.h>
#include <discord/commands/message_send.h>
#include <discord/disco.h>
#include <discord/message.h>
#include <stdio.h>
#include <stdlib.h>

void on_ready(bot_client_t *bot) {
    printf("====================================\n");
    printf("Successfully logged in\n");
    printf("Username:\t%s\n", bot->user->username);
    printf("User ID:\t%ld\n", bot->user->id);
    printf("====================================\n\n");
}

static int watch = 0;

void on_message(bot_client_t *bot, struct discord_message *message) {
    // no content or no member given
    if (!message->content || !message->member)
        return;

    // change watch score
    // if (message->member->user->id == )

    // if not count channel
    if (message->member->user->id == bot->user->id || message->channel_id != 996746797236105236)
        return;
    int n = (int)strtol(message->content, NULL, 10);
    if (n == watch) {
        char s[20];
        watch += 2;
        sprintf(s, "%d", ++n);
        discord_channel_send_message(bot, s, message->channel_id, NULL, false);
    }
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // init to 0. Without this some errors could show up
    discord_event_callbacks_t callbacks = {0};
    callbacks.on_ready = &on_ready;
    callbacks.on_message = &on_message;

    // starts the bot. This function blocks
    discord_start_bot(&callbacks, DISCORD_TOKEN);
    return 0;
}
