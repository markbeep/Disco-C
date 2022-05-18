#include "event.h"
#include "../web/websocket.h"
#include "structures/user.h"

void event_handle(bot_client_t *bot_client, cJSON *data, char *event) {
    lwsl_user("Event: %s\n", event);

    if (!bot_client->callbacks) {
        fprintf(stderr, "No callback functions defined!\n");
        return;
    }

    if (strncmp(event, "READY", 30) == 0) {
        fprintf(stderr, "Received a READY event\n");
        // adds the user struct to the bot struct
        cJSON *user_data = cJSON_GetObjectItem(data, "user");
        bot_client->user = disco_create_user_struct_json(user_data);

        // calls the on_ready callback
        if (bot_client->callbacks->on_ready)
            bot_client->callbacks->on_ready(bot_client, 0, NULL);
    }

    if (strncmp(event, "MESSAGE_CREATE", 30) == 0) {
        fprintf(stderr, "Received a MESSAGE_CREATE event\n");
        if (bot_client->callbacks->on_message)
            bot_client->callbacks->on_message(bot_client, 0, NULL);
    }
}