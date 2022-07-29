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

static int count = 0; // keeps track of what the last sent count is
static uint64_t bot_to_watch = 0UL;
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

void on_ready(bot_client_t *bot) {
    printf("====================================\n");
    printf("Successfully logged in\n");
    printf("Username:\t%s\n", bot->user->username);
    printf("User ID:\t%ju\n", bot->user->id);
    printf("====================================\n\n");
    fflush(stdout);

    // checks what the last count is
    count = get_last_message_count(bot);
}

static void cmd_count(bot_client_t *bot, struct discord_message *message) {
    struct discord_embed embed = {0};
    struct discord_create_message msg = {.embed = &embed};
    embed.color = 0xadd8e6;
    char s[40];
    embed.description = s;
    sprintf(s, "Count = `%d`", count);
    discord_channel_send_message(bot, NULL, message->channel_id, &msg, false);
}

static void cmd_watch(bot_client_t *bot, struct discord_message *message) {
    struct discord_embed embed = {0};
    struct discord_create_message msg = {.embed = &embed};
    embed.color = 0xadd8e6;
    char s[50];
    embed.description = s;

    // parse input
    char *token, *rest = message->content;
    token = strtok_r(rest, " ", &rest);
    token = strtok_r(rest, " ", &rest);
    if (token) {
        bot_to_watch = (uint64_t)strtoull(token, NULL, 10);
    }
    if (bot_to_watch == 0) {
        sprintf(s, "Watch = nobody <:sadge:851469686578741298>");
    } else {
        sprintf(s, "Watch = <@%ju>", bot_to_watch);
    }
    discord_channel_send_message(bot, NULL, message->channel_id, &msg, false);
}

static void send_count(bot_client_t *bot, int n) {
    char s[20];
    sprintf(s, "%d", n);
    discord_channel_send_message(bot, s, count_channel_id, NULL, false);
}

void on_message(bot_client_t *bot, struct discord_message *message) {
    // no content or no member (so not a guild message)
    if (!message->content || !message->member)
        goto cleanup;

    // commands to call
    if (strncmp(message->content, "!count", 6) == 0) {
        cmd_count(bot, message);
        goto cleanup;
    }
    if (message->user->id == owner_id && strncmp(message->content, "!watch", 6) == 0) {
        cmd_watch(bot, message);
        goto cleanup;
    }

    // if it's not the bot I'm watching (nor the owner) or not the count channel
    if ((message->user->id != owner_id && message->user->id != bot_to_watch) ||
        message->channel_id != count_channel_id)
        goto cleanup;
    int n = (int)strtol(message->content, NULL, 10);
    if (n >= count) { // ignore any number below our count
        count = n;
        send_count(bot, n + 1);
    }

cleanup:
    discord_destroy_message(message);
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
    d_set_log_level(D_LOG_ERR | D_LOG_NOTICE);

    // starts the bot. This function blocks
    discord_start_bot(&callbacks, DISCORD_TOKEN, &conf);
    return 0;
}
