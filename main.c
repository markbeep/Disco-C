/**
 * @file main.c
 * @brief Example of what the main file of a bot
 * using this library could look like.
 * @version 0.1
 * @date 2022-07-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "examples/example.h"
#include <discord/disco.h>
#include <discord/message.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/disco_logging.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // Enable logging
    d_set_log_level(D_LOG_ERR | D_LOG_NORMAL | D_LOG_NOTICE);

    // init to 0. Without this some errors could show up
    discord_event_callbacks_t callbacks = {0};
    callbacks.on_ready = &example_on_ready;
    callbacks.on_message = &example_on_message;
    callbacks.on_message_edit = &example_on_edit;
    callbacks.on_message_delete = &example_on_delete;
    callbacks.on_channel_create = &example_channel_create;
    callbacks.on_channel_update = &example_channel_update;
    callbacks.on_channel_delete = &example_channel_delete;
    callbacks.on_interaction = &example_interaction_create;

    // starts the bot. This function blocks
    discord_start_bot(&callbacks);

    return 0;
}
