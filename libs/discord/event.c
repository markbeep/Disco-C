#include "event.h"
#include "../utils/disco_logging.h"
#include "../utils/timer.h"
#include "../web/websocket.h"
#include "structures/user.h"

void event_handle(bot_client_t *bot_client, cJSON *data, char *event) {
    lwsl_user("Event: %s\n", event);

    if (!bot_client->callbacks) {
        d_log_err("No callback functions defined!\n");
        return;
    }

    if (strncmp(event, "READY", 30) == 0) {
        d_log_normal("Received a READY event\n");
        // adds the user struct to the bot struct
        cJSON *user_data = cJSON_GetObjectItem(data, "user");
        bot_client->user = (struct discord_user *)disco_create_user_struct_json(user_data);

        // calls the on_ready callback
        if (bot_client->callbacks->on_ready)
            bot_client->callbacks->on_ready(bot_client);
    }

    if (strncmp(event, "MESSAGE_CREATE", 30) == 0) {
        TIMER_START_FIRST
        d_log_normal("Received a MESSAGE_CREATE event\n");
        if (bot_client->callbacks->on_message) {
            struct discord_message *message = (struct discord_message *)disco_create_message_struct_json(data);
            bot_client->callbacks->on_message(bot_client, message);
            disco_destroy_message(message);
        }
        TIMER_END("message_event");
    }
}