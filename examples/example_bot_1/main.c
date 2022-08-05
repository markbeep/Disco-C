#include "example_slash_commands/example_slash.h"
#include <config.h>
#include <discord/disco.h>
#include <discord/message.h>
#include <stdbool.h>
#include <sys/time.h>
#include <utils/disco_logging.h>

void on_ready(bot_client_t *bot) {
    printf("====================================\n");
    printf("Successfully logged in\n");
    printf("Username:\t%s\n", bot->user->username);
    printf("User ID:\t%ju\n", bot->user->id);
    printf("====================================\n\n");
    fflush(stdout);
}

static void pong_callback(bot_client_t *bot, struct discord_message *msg, void *data) {
    // gets the time passed
    struct timeval stop, *start = (struct timeval *)data;
    gettimeofday(&stop, NULL);
    char time_passed[32];
    long delta = (stop.tv_sec - start->tv_sec) * 1000 + (stop.tv_usec - start->tv_usec) / 1000;
    sprintf(time_passed, "Ping: %lu ms\nHeartbeat: %lu ms", delta, bot->heartbeat_latency);

    struct discord_embed embed = {
        .description = time_passed,
        .color = 0x8000,
    };
    struct discord_create_message send_message = {
        .embed = &embed,
    };
    // edits the message right after the message has been sent to check the latency
    discord_channel_edit_message(bot, NULL, msg->channel_id, msg->id, &send_message);
    // don't forget to destroy the received message in the end to avoid memory leaks
    discord_destroy_message(msg);
}

void example_on_message(bot_client_t *bot, struct discord_message *message) {
    (void)bot;
    if (message->content) { // message content is NULL if there's none
        if (message->user && message->user->bot) {
            fprintf(stderr, "User is a bot. Ignoring\n");
            discord_destroy_message(message);
            return;
        }
        // example of a ping command
        if (strncmp(message->content, "!ping", 6) == 0) {
            struct timeval *start = malloc(sizeof(struct timeval));
            gettimeofday(start, NULL);

            // creates the embed
            struct discord_embed embed = {
                .description = "Pinging...",
                .color = 0x8000,
            };
            struct discord_create_message send_message = {
                .embed = &embed,
            };
            // sends the initial message to a channel
            struct discord_message_cb *cb = (struct discord_message_cb *)malloc(sizeof(struct discord_message_cb));

            // creates the callback, because we want to receive the message
            cb->cb = &pong_callback;
            cb->data = start;
            discord_channel_send_message(bot, NULL, message->channel_id, &send_message, cb);

        } else if (strncmp(message->content, "!exit", 6) == 0) {
            // softly ends the bot
            discord_destroy_message(message);
            websocket_exit(bot);
            return;

        } else if (strncmp(message->content, "!r", 3) == 0) {
            // reconnects the websocket
            websocket_reconnect(bot);
        }
    }
    discord_destroy_message(message);
}

void example_on_edit(bot_client_t *bot, struct discord_message *old, struct discord_message *new) {
    char content[120];
    sprintf(content, "Message %ju was edited. Message in cache: %s. Old message ID = %ju", new->id, old ? "Yes" : "No", old ? old->id : 0);
    discord_channel_send_message(bot, content, new->channel_id, NULL, false);

    if (old)
        discord_destroy_message(old);
    if (new)
        discord_destroy_message(new);
}

void example_on_delete(bot_client_t *bot, uint64_t message_id, uint64_t channel_id, uint64_t guild_id, struct discord_message *message) {
    if (message) {
        char content[50];
        sprintf(content, "Cache: Yes\nID: `%ju`", message_id);
        discord_channel_send_message(bot, content, channel_id, NULL, false);
    } else {
        char content[100];
        sprintf(content, "Cache: No\nID: %ju, channel ID: %ju, guild ID: %ju", message_id, channel_id, guild_id);
        discord_channel_send_message(bot, content, channel_id, NULL, false);
    }

    if (message)
        discord_destroy_message(message);
}

void example_channel_create(bot_client_t *bot, struct discord_channel *channel) {
    (void)bot;
    d_log_normal("Channel created with ID %ju\n", channel->id);

    discord_destroy_channel(channel);
}
void example_channel_update(bot_client_t *bot, struct discord_channel *old, struct discord_channel *new) {
    (void)bot;
    if (old) {
        d_log_normal("Channel in cache was updated: %ju\n", old->id);
    } else {
        d_log_normal("Channel NOT in cache was updated: %ju\n", new->id);
    }

    if (old)
        discord_destroy_channel(old);
    if (new)
        discord_destroy_channel(new);
}
void example_channel_delete(bot_client_t *bot, uint64_t channel_id, uint64_t guild_id, uint64_t parent_id, enum Discord_Channel_Type type, struct discord_channel *channel) {
    (void)bot;
    (void)guild_id;
    (void)parent_id;
    (void)type;
    if (channel) {
        d_log_normal("Channel in cache was deleted: %ju\n", channel->id);
    } else {
        d_log_normal("Channel NOT in cache was deleted: %ju\n", channel_id);
    }

    if (channel)
        discord_destroy_channel(channel);
}

void example_interaction_create(bot_client_t *bot, struct discord_interaction *interaction) {
    (void)bot;
    struct discord_interaction_data *data = interaction->data;
    d_log_notice("Interaction: id = %ju, type = %d, gid = %ju, cid = %ju\n", interaction->id, (int)interaction->type, interaction->guild_id, interaction->channel_id);
    d_log_notice("Data: name = %s, custom_id = %s", data->name, data->custom_id);
    if (strcmp(data->name, "hello") == 0) {
        hello_callback(bot, interaction);
    }

    discord_destroy_interaction(interaction); // cleanup once we don't need it anymore
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // Enable logging
    d_set_log_level(D_LOG_ALL);

    // init to 0. Without this some errors could show up
    discord_event_callbacks_t callbacks = {0};
    callbacks.on_ready = &on_ready;
    callbacks.on_message = &example_on_message;
    callbacks.on_message_edit = &example_on_edit;
    callbacks.on_message_delete = &example_on_delete;
    callbacks.on_channel_create = &example_channel_create;
    callbacks.on_channel_update = &example_channel_update;
    callbacks.on_channel_delete = &example_channel_delete;
    callbacks.on_interaction_create = &example_interaction_create;

    struct discord_config config = {.message_cache_size = 2};

    // starts the bot. This function blocks
    discord_start_bot(&callbacks, DISCORD_TOKEN, &config);

    return 0;
}
