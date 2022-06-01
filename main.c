#include "example/example.h"
#include "libs/discord/disco.h"
#include "libs/discord/structures/message.h"
#include "libs/utils/disco_logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Enable logging
    d_set_log_level(D_LOG_ERR | D_LOG_NORMAL | D_LOG_NOTICE);

    // init to 0. Without this some errors could show up
    disco_event_callbacks_t callbacks = {0};
    callbacks.on_ready = &example_on_ready;
    callbacks.on_message = &example_on_message;
    callbacks.on_message_edit = &example_on_edit;
    callbacks.on_message_delete = &example_on_delete;

    // starts the bot. This function blocks
    disco_start_bot(&callbacks);

    return 0;
}
