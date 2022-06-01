#include "example.h"
#include "sys/time.h"

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
            struct discord_message *msg = disco_channel_send_message(bot, "Pinging...", message->channel_id, NULL, 1);
            gettimeofday(&stop, NULL);
            char time_passed[32];
            sprintf(time_passed, "Ping: %lu ms", (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000);
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
    char content[80];
    sprintf(content, "Message %s was edited. Message in cache: %s", new->id, old ? "Yes" : "No");
    disco_channel_send_message(bot, content, new->channel_id, NULL, 0);
}

void example_on_delete(struct bot_client *bot, char *message_id, char *channel_id, char *guild_id, struct discord_message *message) {
    if (message) {
        int message_size = message->content ? (int)strnlen(message->content, 4096) : 0;
        char content[50];
        sprintf(content, "Cache: Yes\nID: `%s`", message_id);
        disco_channel_send_message(bot, content, channel_id, NULL, 0);
    } else {
        char content[100];
        sprintf(content, "Cache: No\nID: %s, channel ID: %s, guild ID: %s", message_id, channel_id, guild_id);
        disco_channel_send_message(bot, content, channel_id, NULL, 0);
    }
}
