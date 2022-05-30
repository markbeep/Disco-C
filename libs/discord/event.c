#include "event.h"
#include "../utils/disco_logging.h"
#include "../utils/t_pool.h"
#include "../utils/timer.h"
#include "../web/websocket.h"
#include "structures/user.h"

void event_handle_message_create(void *m) {
    event_pool_workload_t *work = (event_pool_workload_t *)m;
    struct discord_message *message = (struct discord_message *)work->data;
    work->bot->callbacks->on_message(work->bot, message);
    disco_destroy_message(message);
    free(m);
}

void event_handle(bot_client_t *bot, cJSON *data, char *event) {
    lwsl_user("Event: %s\n", event);

    if (!bot->callbacks) {
        d_log_err("No callback functions defined!\n");
        return;
    }

    if (strncmp(event, "READY", 30) == 0) {
        d_log_normal("Received a READY event\n");
        // adds the user struct to the bot struct
        cJSON *user_data = cJSON_GetObjectItem(data, "user");
        bot->user = (struct discord_user *)disco_create_user_struct_json(user_data);
        // calls the on_ready callback
        if (bot->callbacks->on_ready)
            t_pool_add_work(bot->thread_pool, bot->callbacks->on_ready, (void *)bot);
    }

    else if (strncmp(event, "RESUMED", 30) == 0) {

    }

    else if (strncmp(event, "RECONNECT", 30) == 0) {

    }

    else if (strncmp(event, "INVALID SESSION", 30) == 0) {

    }

    else if (strncmp(event, "RESUMED", 30) == 0) {

    }

    else if (strncmp(event, "MESSAGE_CREATE", 30) == 0) {
        TIMER_START_FIRST
        d_log_normal("Received a MESSAGE_CREATE event\n");
        if (bot->callbacks->on_message) {
            // this is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            work->data = (void *)disco_create_message_struct_json(data);
            t_pool_add_work(bot->thread_pool, &event_handle_message_create, work);
        }
        TIMER_END("message_event");
    }

    else {
        d_log_normal("Received a different event: %s\n", event);
    }
}
