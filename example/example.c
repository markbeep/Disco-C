#include "example.h"
#include "../src/utils/disco_logging.h"
#include "sys/time.h"
#include <stdbool.h>

void example_on_ready(bot_client_t *bot) {
    printf("====================================\n");
    printf("Successfully logged in\n");
    printf("Username:\t%s\n", bot->user->username);
    printf("User ID:\t%s\n", bot->user->id);
    printf("====================================\n\n");
}

void example_on_message(bot_client_t *bot, struct discord_message *message) {
    (void)bot;
    fprintf(stderr, "Received a message: %s\n", message ? message->content : "(null)");
    if (message->content) { // message content is NULL if there's none
        if (message->author && message->author->bot) {
            fprintf(stderr, "User is a bot. Ignoring\n");
            return;
        }
        // example of a ping command
        if (strncmp(message->content, "!ping", 6) == 0) {
            struct timeval stop, start;
            gettimeofday(&start, NULL);
            // sends the initial message to a channel
            struct discord_message *msg = disco_channel_send_message(bot, "Pinging...", message->channel_id, NULL, true);
            gettimeofday(&stop, NULL);
            char time_passed[32];
            long delta = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000;
            sprintf(time_passed, "Ping: %lu ms\nHeartbeat: %lu ms", delta, bot->heartbeat_latency);
            // edits the message right after the message has been sent to check the latency
            disco_channel_edit_message(bot, time_passed, msg->channel_id, msg->id, NULL);
            // don't forget to destroy the message in the end to avoid memory leaks
            disco_destroy_message(msg);

        } else if (strncmp(message->content, "!exit", 6) == 0) {
            // softly ends the bot
            websocket_close(bot);

        } else if (strncmp(message->content, "!r", 3) == 0) {
            // reconnects the websocket
            websocket_reconnect(bot);
        }
    }
}

void example_on_edit(bot_client_t *bot, struct discord_message *old, struct discord_message *new) {
    char content[120];
    sprintf(content, "Message %s was edited. Message in cache: %s. Old message ID = %s", new->id, old ? "Yes" : "No", old ? old->id : NULL);
    disco_channel_send_message(bot, content, new->channel_id, NULL, false);
}

void example_on_delete(bot_client_t *bot, char *message_id, char *channel_id, char *guild_id, struct discord_message *message) {
    if (message) {
        char content[50];
        sprintf(content, "Cache: Yes\nID: `%s`", message_id);
        disco_channel_send_message(bot, content, channel_id, NULL, false);
    } else {
        char content[100];
        sprintf(content, "Cache: No\nID: %s, channel ID: %s, guild ID: %s", message_id, channel_id, guild_id);
        disco_channel_send_message(bot, content, channel_id, NULL, false);
    }
}

void example_channel_create(bot_client_t *bot, struct discord_channel *channel) {
    (void)bot;
    d_log_normal("Channel created with ID %s\n", channel->id);
}
void example_channel_update(bot_client_t *bot, struct discord_channel *old, struct discord_channel *new) {
    (void)bot;
    if (old) {
        d_log_normal("Channel in cache was updated: %s\n", old->id);
    } else {
        d_log_normal("Channel NOT in cache was updated: %s\n", new->id);
    }
}
void example_channel_delete(bot_client_t *bot, char *channel_id, char *guild_id, char *parent_id, enum Discord_Channel_Type type, struct discord_channel *channel) {
    (void)bot;
    (void)guild_id;
    (void)parent_id;
    (void)type;
    if (channel) {
        d_log_normal("Channel in cache was deleted: %s\n", channel->id);
    } else {
        d_log_normal("Channel NOT in cache was deleted: %s\n", channel_id);
    }
}
