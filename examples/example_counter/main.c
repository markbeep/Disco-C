/**
 * @file main.c
 * @author markbeep
 * @brief Example file that receives a number and one ups it
 * @version 0.1
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <config2.h> // note that this uses a different config file to run a different instance
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
static uint64_t count_channel_id = 996746797236105236UL;
static uint64_t owner_id = 205704051856244736UL;

void on_message(bot_client_t *bot, struct discord_message *message) {
    // no content or no member (so not a guild message)
    if (!message->content || !message->member)
        return;

    // change watch score
    if (message->user->id == owner_id && strncmp(message->content, "!count", 6) == 0) {
        char *token, *rest = message->content;
        // we do it twice to skip the !count from the beginning
        token = strtok_r(rest, " ", &rest);
        token = strtok_r(rest, " ", &rest);
        if (!token)
            return;
        watch = (int)strtol(token, NULL, 10);
        char s[20];
        sprintf(s, "Watching for `%d`", watch);
        discord_channel_send_message(bot, s, message->channel_id, NULL, false);
        return;
    }

    // if it's the myself or not the count channel
    if (message->user->id == bot->user->id || message->channel_id != count_channel_id)
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

    // optionally sets the allowed messages in the cache
    struct discord_config conf = {0};
    conf.message_cache_size = 2;

    // starts the bot. This function blocks
    discord_start_bot(&callbacks, DISCORD_TOKEN, &conf);
    return 0;
}
