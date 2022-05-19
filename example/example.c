#include "example.h"
#include "sys/time.h"

void example_on_ready(bot_client_t *bot) {
    printf("===============================\n");
    printf("Successfully logged in\n");
    printf("Username:\t%s\n", bot->user->username);
    printf("User ID:\t%s\n", bot->user->id);
    printf("===============================\n\n");
}

void example_on_message(bot_client_t *bot, struct discord_message *message) {
    (void)bot;
    fprintf(stderr, "Received a message: %s\n", message ? message->content : "(null)");
    if (message) {
        if (message->author && message->author->bot) {
            fprintf(stderr, "User is a bot. Ignoring\n");
            return;
        }
        // example of a ping command
        if (strncmp(message->content, "!ping", 6) == 0) {
            struct timeval stop, start;
            gettimeofday(&start, NULL);
            struct discord_message *msg = disco_channel_send_message(bot, "Pinging...", message->channel_id, NULL, 1);
            gettimeofday(&stop, NULL);
            char time_passed[32];
            sprintf(time_passed, "Ping: %lu ms", (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000);
            disco_channel_edit_message(bot, time_passed, msg->channel_id, msg->id, NULL);
            disco_destroy_message(msg);
        }
    }
}
