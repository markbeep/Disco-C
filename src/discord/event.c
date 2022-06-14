#include "event.h"
#include "../utils/cache.h"
#include "../utils/disco_logging.h"
#include "../utils/t_pool.h"
#include "../utils/timer.h"
#include "../web/websocket.h"
#include "structures/user.h"

void event_handle_ready(void *b) {
    bot_client_t *bot = (bot_client_t *)b;
    bot->callbacks->on_ready(bot);
}

void event_handle_message_create(void *w) {
    event_pool_workload_t *work = (event_pool_workload_t *)w;
    struct discord_message *message = (struct discord_message *)work->data;
    work->bot->callbacks->on_message(work->bot, message);
    free(work);
}

struct edit_message {
    struct discord_message *old;
    struct discord_message *new;
};

void event_handle_message_update(void *w) {
    event_pool_workload_t *work = (event_pool_workload_t *)w;
    struct edit_message *edit = (struct edit_message *)work->data;
    work->bot->callbacks->on_message_edit(work->bot, edit->old, edit->new);
    free(edit);
    free(work);
}

struct delete_message {
    char *id;
    char *channel_id;
    char *guild_id;
    struct discord_message *message;
};

void event_handle_message_delete(void *w) {
    event_pool_workload_t *work = (event_pool_workload_t *)w;
    struct delete_message *del = (struct delete_message *)work->data;
    d_log_notice("id = %s, cid = %s, gid = %s\n", del->id, del->channel_id, del->guild_id);

    if (!del->id || !del->channel_id) {
        d_log_debug("Message or channel ID in event_handle_message_delete is NULL\n");
    } else {
        work->bot->callbacks->on_message_delete(work->bot, del->id, del->channel_id, del->guild_id, del->message);
    }

    // deletes the message from the cache
    if (del->message)
        disco_cache_delete_message(del->id);
    if (del->id)
        free(del->id);
    if (del->channel_id)
        free(del->channel_id);
    if (del->guild_id)
        free(del->guild_id);
    free(del);
    free(work);
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
            t_pool_add_work(bot->thread_pool, event_handle_ready, (void *)bot);
    } else if (strncmp(event, "RESUMED", 8) == 0) {

    } else if (strncmp(event, "RECONNECT", 10) == 0) {

    } else if (strncmp(event, "INVALID_SESSION", 16) == 0) {

    } else if (strncmp(event, "APPLICATION_COMMAND_PERMISSIONS_UPDATE", 39) == 0) {

    } else if (strncmp(event, "CHANNEL_CREATE", 15) == 0) {

    } else if (strncmp(event, "CHANNEL_UPDATE", 15) == 0) {
        if (bot->callbacks->on_channel_create) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            struct discord_channel *channel = disco_create_channel_struct_json(data);
            work->data = (void *)channel;
            d_log_debug("Channel ID = %s\n", channel->id);

            // adds the channel to cache
            disco_cache_set_channel(channel);

            t_pool_add_work(bot->thread_pool, &event_handle_message_create, work);
        }
    } else if (strncmp(event, "CHANNEL_DELETE", 15) == 0) {

    } else if (strncmp(event, "CHANNEL_PINS_UPDATE", 20) == 0) {

    } else if (strncmp(event, "THREAD_CREATE", 14) == 0) {

    } else if (strncmp(event, "THREAD_UPDATE", 14) == 0) {

    } else if (strncmp(event, "THREAD_DELETE", 14) == 0) {

    } else if (strncmp(event, "THREAD_LIST_SYNC", 17) == 0) {

    } else if (strncmp(event, "THREAD_MEMBER_UPDATE", 21) == 0) {

    } else if (strncmp(event, "THREAD_MEMBERS_UPDATE", 22) == 0) {

    } else if (strncmp(event, "GUILD_CREATE", 13) == 0) {

    } else if (strncmp(event, "GUILD_UPDATE", 13) == 0) {

    } else if (strncmp(event, "GUILD_DELETE", 13) == 0) {

    } else if (strncmp(event, "GUILD_BAN_ADD", 14) == 0) {

    } else if (strncmp(event, "GUILD_BAN_REMOVE", 17) == 0) {

    } else if (strncmp(event, "GUILD_EMOJIS_UPDATE", 20) == 0) {

    } else if (strncmp(event, "GUILD_STICKERS_UPDATE", 22) == 0) {

    } else if (strncmp(event, "GUILD_INTEGRATIONS_UPDATE", 26) == 0) {

    } else if (strncmp(event, "GUILD_MEMBER_ADD", 17) == 0) {

    } else if (strncmp(event, "GUILD_MEMBER_REMOVE", 20) == 0) {

    } else if (strncmp(event, "GUILD_MEMBER_UPDATE", 20) == 0) {

    } else if (strncmp(event, "GUILD_MEMBERS_CHUNK", 20) == 0) {

    } else if (strncmp(event, "GUILD_ROLE_CREATE", 18) == 0) {

    } else if (strncmp(event, "GUILD_ROLE_UPDATE", 18) == 0) {

    } else if (strncmp(event, "GUILD_ROLE_DELETE", 18) == 0) {

    } else if (strncmp(event, "GUILD_SCHEDULED_EVENT_CREATE", 29) == 0) {

    } else if (strncmp(event, "GUILD_SCHEDULED_EVENT_UPDATE", 29) == 0) {

    } else if (strncmp(event, "GUILD_SCHEDULED_EVENT_DELETE", 29) == 0) {

    } else if (strncmp(event, "GUILD_SCHEDULED_EVENT_USER_ADD", 31) == 0) {

    } else if (strncmp(event, "GUILD_SCHEDULED_EVENT_USER_REMOVE", 34) == 0) {

    } else if (strncmp(event, "INTEGRATION_CREATE", 19) == 0) {

    } else if (strncmp(event, "INTEGRATION_UPDATE", 19) == 0) {

    } else if (strncmp(event, "INTEGRATION_CREATE", 19) == 0) {

    } else if (strncmp(event, "INVITE_CREATE", 14) == 0) {

    } else if (strncmp(event, "INVITE_DELETE", 14) == 0) {

    } else if (strncmp(event, "MESSAGE_CREATE", 15) == 0) {
        if (bot->callbacks->on_message) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            // we free the message struct when cleaning up the cache
            struct discord_message *message = disco_create_message_struct_json(data);
            work->data = (void *)message;
            d_log_debug("Message ID = %s\n", message->id);

            // adds the message to cache
            disco_cache_set_message(message);

            t_pool_add_work(bot->thread_pool, &event_handle_message_create, work);
        }
    } else if (strncmp(event, "MESSAGE_UPDATE", 15) == 0) {
        if (bot->callbacks->on_message_edit) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            // we free the message struct when cleaning up the cache
            struct discord_message *message = disco_create_message_struct_json(data);
            // to be freed inside the event handle function
            struct edit_message *edt_msg = (struct edit_message *)malloc(sizeof(struct edit_message));
            edt_msg->old = disco_cache_get_message(message->id);
            edt_msg->new = message;
            work->data = (void *)edt_msg;

            d_log_debug("Message ID = %s\n", message->id);

            // adds the message to cache
            disco_cache_set_message(message);

            t_pool_add_work(bot->thread_pool, &event_handle_message_update, work);
        }
    } else if (strncmp(event, "MESSAGE_DELETE", 15) == 0) {
        if (bot->callbacks->on_message_delete) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            struct delete_message *del = (struct delete_message *)malloc(sizeof(struct delete_message));
            // we need to allocate the IDs anew, because the JSON with the original IDs gets freed
            del->id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "id"));
            if (del->id)
                del->id = strndup(del->id, strnlen(del->id, 20));
            del->channel_id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "channel_id"));
            if (del->channel_id)
                del->channel_id = strndup(del->channel_id, strnlen(del->channel_id, 20));
            del->guild_id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "guild_id"));
            if (del->guild_id)
                del->guild_id = strndup(del->guild_id, strnlen(del->guild_id, 20));
            del->message = del->id ? disco_cache_get_message(del->id) : NULL;
            work->data = (void *)del;
            t_pool_add_work(bot->thread_pool, &event_handle_message_delete, work);
        }
    } else if (strncmp(event, "MESSAGE_DELETE_BULK", 20) == 0) {

    } else if (strncmp(event, "MESSAGE_REACTION_ADD", 21) == 0) {

    } else if (strncmp(event, "MESSAGE_REACTION_REMOVE", 24) == 0) {

    } else if (strncmp(event, "MESSAGE_REACTION_REMOVE_ALL", 28) == 0) {

    } else if (strncmp(event, "MESSAGE_REACTION_REMOVE_EMOJI", 30) == 0) {

    } else if (strncmp(event, "PRESENCE_UPDATE", 16) == 0) {

    } else if (strncmp(event, "STAGE_INSTANCE_CREATE", 22) == 0) {

    } else if (strncmp(event, "STAGE_INSTANCE_DELETE", 22) == 0) {

    } else if (strncmp(event, "STAGE_INSTANCE_UPDATE", 22) == 0) {

    } else if (strncmp(event, "TYPING_START", 13) == 0) {

    } else if (strncmp(event, "USER_UPDATE", 12) == 0) {

    } else if (strncmp(event, "VOICE_STATE_UPDATE", 19) == 0) {

    } else if (strncmp(event, "VOICE_SERVER_UPDATE", 20) == 0) {

    } else if (strncmp(event, "WEBHOOKS_UPDATE", 16) == 0) {
    }
}
