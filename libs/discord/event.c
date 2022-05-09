#include "event.h"

void event_handle(client_websocket_t *client, cJSON *data, char *event) {
    lwsl_user("Event: %s\n", event);
    (void)data;

    if (strcmp(event, "READY") == 0) {
    }

    if (strcmp(event, "MESSAGE_CREATE") == 0) {
        channel_test_send_message(client, "test");
    }
}