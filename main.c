#include "libs/discord/disco.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sample_on_ready(bot_client_t *bot, int argc, char **argv) {
    (void)bot;
    (void)argc;
    (void)argv;
    printf("Successfully logged in\n");
}

int main() {

    // LOG LEVEL
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    // init to 0. Without this some errors could show up
    disco_callbacks_t callbacks = {0};
    callbacks.on_ready = &sample_on_ready;

    // starts the bot. This function blocks
    disco_start_bot(&callbacks);

    return 0;
}