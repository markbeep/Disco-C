#include "libs/discord/disco.h"
#include "libs/discord/structures/message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sample_on_ready(bot_client_t *bot, int argc, char **argv) {
    (void)bot;
    (void)argc;
    (void)argv;
    printf("===============================\n");
    printf("Successfully logged in\n");
    printf("Username:\t%s\n", bot->user->username);
    printf("User ID:\t%s\n", bot->user->id);
    printf("===============================\n\n");
    disco_channel_send_message(bot, "Successfully logged in", "944968090490380321", NULL);
}

int main() {

    // LOG LEVEL
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    // init to 0. Without this some errors could show up
    disco_event_callbacks_t callbacks = {0};
    callbacks.on_ready = &sample_on_ready;

    // starts the bot. This function blocks
    disco_start_bot(&callbacks);

    return 0;
}