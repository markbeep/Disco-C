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

    if (strncmp(event, "READY", 6) == 0) {
        d_log_normal("Received a READY event\n");
        // adds the user struct to the bot struct
        cJSON *user_data = cJSON_GetObjectItem(data, "user");
        bot->user = (struct discord_user *)disco_create_user_struct_json(user_data);
        // calls the on_ready callback
        if (bot->callbacks->on_ready)
            t_pool_add_work(bot->thread_pool, bot->callbacks->on_ready, (void *)bot);
    }

    else if (strncmp(event, "RESUMED", 8) == 0) {

    }

    else if (strncmp(event, "RECONNECT", 10) == 0) {

    }

    else if (strncmp(event, "INVALID_SESSION", 16) == 0) {

    }

    else if (strncmp(event, "APPLICATION_COMMAND_PERMISSIONS_UPDATE", 39) == 0) {

    }

    else if (strncmp(event, "CHANNEL_CREATE", 15) == 0) {

    }

    else if (strncmp(event, "CHANNEL_UPDATE", 15) == 0) {

    }

    else if (strncmp(event, "CHANNEL_DELETE", 15) == 0) {

    }

    else if (strncmp(event, "CHANNEL_PINS_UPDATE", 20) == 0) {

    }

    else if (strncmp(event, "THREAD_CREATE", 14) == 0) {

    }

    else if (strncmp(event, "THREAD_UPDATE", 14) == 0) {

    }

    else if (strncmp(event, "THREAD_DELETE", 14) == 0) {

    }

    else if (strncmp(event, "THREAD_LIST_SYNC", 17) == 0) {

    }

    else if (strncmp(event, "THREAD_MEMBER_UPDATE", 21) == 0) {

    }

    else if (strncmp(event, "THREAD_MEMBERS_UPDATE", 22) == 0) {

    }

    else if (strncmp(event, "GUILD_CREATE", 13) == 0) {

    }

    else if (strncmp(event, "GUILD_UPDATE", 13) == 0) {

    }

    else if (strncmp(event, "GUILD_DELETE", 13) == 0) {

    }

    else if (strncmp(event, "GUILD_BAN_ADD", 14) == 0) {

    }

    else if (strncmp(event, "GUILD_BAN_REMOVE", 17) == 0) {

    }

    else if (strncmp(event, "GUILD_EMOJIS_UPDATE", 20) == 0) {

    }

    else if (strncmp(event, "GUILD_STICKERS_UPDATE", 22) == 0) {

    }

    else if (strncmp(event, "GUILD_INTEGRATIONS_UPDATE", 26) == 0) {

    }

    else if (strncmp(event, "GUILD_MEMBER_ADD", 17) == 0) {

    }

    else if (strncmp(event, "GUILD_MEMBER_REMOVE", 20) == 0) {

    }

    else if (strncmp(event, "GUILD_MEMBER_UPDATE", 20) == 0) {

    }

    else if (strncmp(event, "GUILD_MEMBERS_CHUNK", 20) == 0) {

    }

    else if (strncmp(event, "GUILD_ROLE_CREATE", 18) == 0) {

    }

    else if (strncmp(event, "GUILD_ROLE_UPDATE", 18) == 0) {

    }

    else if (strncmp(event, "GUILD_ROLE_DELETE", 18) == 0) {

    }

    else if (strncmp(event, "GUILD_SCHEDULED_EVENT_CREATE", 29) == 0) {

    }

    else if (strncmp(event, "GUILD_SCHEDULED_EVENT_UPDATE", 29) == 0) {

    }

    else if (strncmp(event, "GUILD_SCHEDULED_EVENT_DELETE", 29) == 0) {

    }

    else if (strncmp(event, "GUILD_SCHEDULED_EVENT_USER_ADD", 31) == 0) {

    }

    else if (strncmp(event, "GUILD_SCHEDULED_EVENT_USER_REMOVE", 34) == 0) {

    }

    else if (strncmp(event, "INTEGRATION_CREATE", 19) == 0) {

    }

    else if (strncmp(event, "INTEGRATION_UPDATE", 19) == 0) {

    }

    else if (strncmp(event, "INTEGRATION_CREATE", 19) == 0) {

    }

    else if (strncmp(event, "INVITE_CREATE", 14) == 0) {

    }

    else if (strncmp(event, "INVITE_DELETE", 14) == 0) {

    }

    else if (strncmp(event, "MESSAGE_CREATE", 15) == 0) {
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

    else if (strncmp(event, "MESSAGE_UPDATE", 15) == 0) {

    }

    else if (strncmp(event, "MESSAGE_DELETE", 15) == 0) {

    }

    else if (strncmp(event, "MESSAGE_DELETE_BULK", 20) == 0) {

    }

    else if (strncmp(event, "MESSAGE_REACTION_ADD", 21) == 0) {

    }

    else if (strncmp(event, "MESSAGE_REACTION_REMOVE", 24) == 0) {

    }

    else if (strncmp(event, "MESSAGE_REACTION_REMOVE_ALL", 28) == 0) {

    }

    else if (strncmp(event, "MESSAGE_REACTION_REMOVE_EMOJI", 30) == 0) {

    }

    else if (strncmp(event, "PRESENCE_UPDATE", 16) == 0) {

    }

    else if (strncmp(event, "STAGE_INSTANCE_CREATE", 22) == 0) {

    }

    else if (strncmp(event, "STAGE_INSTANCE_DELETE", 22) == 0) {

    }

    else if (strncmp(event, "STAGE_INSTANCE_UPDATE", 22) == 0) {

    }

    else if (strncmp(event, "TYPING_START", 13) == 0) {

    }

    else if (strncmp(event, "USER_UPDATE", 12) == 0) {

    }

    else if (strncmp(event, "VOICE_STATE_UPDATE", 19) == 0) {

    }

    else if (strncmp(event, "VOICE_SERVER_UPDATE", 20) == 0) {

    }

    else if (strncmp(event, "WEBHOOKS_UPDATE", 16) == 0) {
    }
}
