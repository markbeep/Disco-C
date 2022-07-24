/**
 * @file main.c
 * @author markbeep
 * @brief Example file that receives a number and one ups it in the specified channel.
 * @version 0.1
 * @date 2022-07-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <config2.h> // note that this uses a different config file to run a different instance
#include <discord/disco.h>
#include <discord/message.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/disco_logging.h>

static int watch = 0;
static uint64_t count_channel_id = 996746797236105236UL;
static uint64_t owner_id = 205704051856244736UL;

/**
 * @brief Gets the last sent integer in the channel.
 *
 * @param bot
 * @return int
 */
static int get_last_message_count(bot_client_t *bot) {
    struct discord_message **arr;
    int arr_size = discord_get_messages(bot, count_channel_id, &arr, 1, 0, 0, 0);
    int res = -1;
    if (arr_size > 0 && arr[0]) {
        res = (int)strtol(arr[0]->content, NULL, 10);
        free(arr);
    }
    return res;
}

static void send_new_count(bot_client_t *bot) {
    char s[20];
    sprintf(s, "%d", watch + 1);
    watch += 2;
    discord_channel_send_message(bot, s, count_channel_id, NULL, false);
}

void on_ready(bot_client_t *bot) {
    printf("====================================\n");
    printf("Successfully logged in\n");
    printf("Username:\t%s\n", bot->user->username);
    printf("User ID:\t%ju\n", bot->user->id);
    printf("====================================\n\n");
    fflush(stdout);

    // checks what the last count is and sends the next one
    watch = get_last_message_count(bot);
    send_new_count(bot);
}

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
        if (!token) {
            char s[40];
            sprintf(s, "Watching for `%d`", watch);
            discord_channel_send_message(bot, s, message->channel_id, NULL, false);
            return;
        }
        watch = (int)strtol(token, NULL, 10);
        char s[40];
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

    // Make errors show up in the console
    d_set_log_level(D_LOG_ERR);

    // starts the bot. This function blocks
    discord_start_bot(&callbacks, DISCORD_TOKEN, &conf);
    return 0;
}
