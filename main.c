#include "example/example.h"
#include "libs/discord/disco.h"
#include "libs/discord/structures/message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    // LOG LEVEL
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    // init to 0. Without this some errors could show up
    disco_event_callbacks_t callbacks = {0};
    callbacks.on_ready = &example_on_ready;
    callbacks.on_message = &example_on_message;

    // starts the bot. This function blocks
    disco_start_bot(&callbacks);

    return 0;
}