#include "event.h"
#include "../web/websocket.h"

void event_handle(bot_client_t *bot_client, cJSON *data, char *event) {
    lwsl_user("Event: %s\n", event);
    (void)data;

    if (strncmp(event, "READY", 30) == 0) {
        fprintf(stderr, "Received a READY event\n");
        if (bot_client->callbacks->on_ready)
            bot_client->callbacks->on_ready(bot_client, 0, NULL);
    }

    if (strncmp(event, "MESSAGE_CREATE", 30) == 0) {
        fprintf(stderr, "Received a MESSAGE_CREATE event\n");
        channel_test_send_message(bot_client, "this was sent by a Discord C library");
        if (bot_client->callbacks->on_message)
            bot_client->callbacks->on_message(bot_client, 0, NULL);
    }
}