#include "example.h"

void example_on_ready(bot_client_t *bot) {
    printf("===============================\n");
    printf("Successfully logged in\n");
    printf("Username:\t%s\n", bot->user->username);
    printf("User ID:\t%s\n", bot->user->id);
    printf("===============================\n\n");
    // disco_channel_send_message(bot, "Successfully logged in", "944968090490380321", NULL);
    disco_channel_edit_message(bot, "New content", "944968090490380321", "976875892641001593", NULL);
}

void example_on_message(bot_client_t *bot, struct discord_message *message) {
    (void)bot;
    fprintf(stderr, "Received a message: %s\n", message ? message->content : "(null)");
}