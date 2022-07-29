#include <discord/event.h>
#include <discord/user.h>
#include <string.h>
#include <utils/cache.h>
#include <utils/disco_logging.h>
#include <utils/t_pool.h>
#include <utils/timer.h>
#include <web/websocket.h>

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
    uint64_t id;
    uint64_t channel_id;
    uint64_t guild_id;
    struct discord_message *message;
};

void event_handle_message_delete(void *w) {
    event_pool_workload_t *work = (event_pool_workload_t *)w;
    struct delete_message *del = (struct delete_message *)work->data;
    d_log_notice("id = %ju, cid = %ju, gid = %ju\n", del->id, del->channel_id, del->guild_id);

    if (!del->id || !del->channel_id) {
        d_log_debug("Message or channel ID in event_handle_message_delete is NULL\n");
    } else {
        work->bot->callbacks->on_message_delete(work->bot, del->id, del->channel_id, del->guild_id, del->message);
    }
    free(del);
    free(work);
}

void event_handle_channel_create(void *w) {
    event_pool_workload_t *work = (event_pool_workload_t *)w;
    struct discord_channel *channel = (struct discord_channel *)work->data;
    work->bot->callbacks->on_channel_create(work->bot, channel);
    free(work);
}

struct edit_channel {
    struct discord_channel *old;
    struct discord_channel *new;
};

void event_handle_channel_update(void *w) {
    event_pool_workload_t *work = (event_pool_workload_t *)w;
    struct edit_channel *edit = (struct edit_channel *)work->data;
    work->bot->callbacks->on_channel_update(work->bot, edit->old, edit->new);
    free(edit);
    free(work);
}

struct delete_channel {
    uint64_t id;
    uint64_t guild_id;
    uint64_t parent_id;
    enum Discord_Channel_Type type;
    struct discord_channel *channel;
};

void event_handle_channel_delete(void *w) {
    event_pool_workload_t *work = (event_pool_workload_t *)w;
    struct delete_channel *del = (struct delete_channel *)work->data;
    d_log_notice("id = %ju, pid = %ju, gid = %ju\n", del->id, del->parent_id, del->guild_id);

    if (!del->id || !del->guild_id) {
        d_log_debug("Channel or guild ID in event_handle_channel_delete is NULL\n");
    } else {
        work->bot->callbacks->on_channel_delete(work->bot, del->id, del->guild_id, del->parent_id, del->type, del->channel);
    }
    free(del);
    free(work);
}

void event_handle_interaction_create(void *w) {
    event_pool_workload_t *work = (event_pool_workload_t *)w;
    struct discord_interaction *interaction = (struct discord_interaction *)work->data;
    work->bot->callbacks->on_interaction_create(work->bot, interaction);
    free(work);
}

void event_handle(bot_client_t *bot, cJSON *data, char *event) {
    d_log_notice("Event: %s\n", event);

    if (!bot->callbacks) {
        d_log_err("No callback functions defined!\n");
        return;
    }

    if (strncmp(event, "READY", 6) == 0) {
        d_log_normal("Received a READY event\n");
        // adds the user struct to the bot struct
        cJSON *user_data = cJSON_GetObjectItem(data, "user");
        if (user_data) {
            if (bot->user) {
                d_log_debug("Received a second READY event, recreating the bot user...\n");
                discord_destroy_user(bot->user);
            }
            bot->user = (struct discord_user *)_d_json_to_user(user_data);
        }
        user_data = cJSON_GetObjectItem(data, "session_id");
        if (user_data) {
            d_log_debug("Received the sequence id: %s\n", user_data->valuestring);
            bot->websocket_client->session_id = strndup(user_data->valuestring, 100);
        }
        // calls the on_ready callback
        if (bot->callbacks->on_ready)
            t_pool_add_work(bot->thread_pool, event_handle_ready, (void *)bot);
    } else if (strncmp(event, "RESUMED", 8) == 0) {

    } else if (strncmp(event, "RECONNECT", 10) == 0) {

    } else if (strncmp(event, "INVALID_SESSION", 16) == 0) {

    } else if (strncmp(event, "APPLICATION_COMMAND_PERMISSIONS_UPDATE", 39) == 0) {

    } else if (strncmp(event, "CHANNEL_CREATE", 15) == 0) {
        if (bot->callbacks->on_channel_create) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            struct discord_channel *channel = _d_json_to_channel(data);
            work->data = (void *)_d_copy_channel(channel);
            d_log_debug("Channel ID = %ju\n", channel->id);

            // adds the channel to cache
            discord_cache_set_channel(channel);

            t_pool_add_work(bot->thread_pool, &event_handle_channel_create, work);
        }

    } else if (strncmp(event, "CHANNEL_UPDATE", 15) == 0) {
        if (bot->callbacks->on_channel_update) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            // we free the channel struct when cleaning up the cache
            struct discord_channel *channel = _d_json_to_channel(data);

            // this gets freed inside the event handle function
            struct edit_channel *edt_channel = (struct edit_channel *)malloc(sizeof(struct edit_channel));
            edt_channel->old = _d_copy_channel(discord_cache_get_channel(channel->id));
            edt_channel->new = _d_copy_channel(channel);
            work->data = (void *)edt_channel;

            d_log_debug("Channel ID = %ju\n", channel->id);

            // adds the new channel to the cache
            discord_cache_set_channel(channel);

            t_pool_add_work(bot->thread_pool, &event_handle_channel_update, work);
        }

    } else if (strncmp(event, "CHANNEL_DELETE", 15) == 0) {
        if (bot->callbacks->on_channel_delete) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            struct delete_channel *del = (struct delete_channel *)malloc(sizeof(struct delete_channel));
            // we need to allocate the IDs anew, because the JSON with the original IDs gets freed
            char *del_id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "id"));
            if (del_id)
                del->id = (uint64_t)strtoll(del_id, NULL, 10);
            char *del_parent_id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "parent_id"));
            if (del_parent_id)
                del->parent_id = (uint64_t)strtoll(del_parent_id, NULL, 10);
            char *del_guild_id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "guild_id"));
            if (del_guild_id)
                del->guild_id = (uint64_t)strtoll(del_guild_id, NULL, 10);
            cJSON *c = cJSON_GetObjectItem(data, "type");
            del->type = c->valueint;
            del->channel = del->id ? _d_copy_channel(discord_cache_get_channel(del->id)) : NULL;
            work->data = (void *)del;
            t_pool_add_work(bot->thread_pool, &event_handle_channel_delete, work);
        }

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

    } else if (strncmp(event, "INTEGRATION_DELETE", 19) == 0) {

    } else if (strncmp(event, "INVITE_CREATE", 14) == 0) {

    } else if (strncmp(event, "INVITE_DELETE", 14) == 0) {

    } else if (strncmp(event, "MESSAGE_CREATE", 15) == 0) {
        if (bot->callbacks->on_message) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            // we free the message struct when cleaning up the cache
            struct discord_message *message = _d_json_to_message(data);
            work->data = (void *)_d_copy_message(message);
            d_log_debug("Message ID = %ju\n", message->id);

            // adds the message to cache
            discord_cache_set_message(message);

            t_pool_add_work(bot->thread_pool, &event_handle_message_create, work);
        }
    } else if (strncmp(event, "MESSAGE_UPDATE", 15) == 0) {
        if (bot->callbacks->on_message_edit) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            // we free the message struct when cleaning up the cache
            struct discord_message *message = _d_json_to_message(data);
            // to be freed inside the event handle function
            struct edit_message *edt_msg = (struct edit_message *)malloc(sizeof(struct edit_message));
            edt_msg->old = _d_copy_message(discord_cache_get_message(message->id));
            edt_msg->new = _d_copy_message(message);
            work->data = (void *)edt_msg;

            d_log_debug("Message ID = %ju\n", message->id);

            // adds the message to cache
            discord_cache_set_message(message);

            t_pool_add_work(bot->thread_pool, &event_handle_message_update, work);
        }
    } else if (strncmp(event, "MESSAGE_DELETE", 15) == 0) {
        if (bot->callbacks->on_message_delete) {
            // the work struct is then freed inside the thread function
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            struct delete_message *del = (struct delete_message *)malloc(sizeof(struct delete_message));
            // we need to allocate the IDs anew, because the JSON with the original IDs gets freed
            char *del_id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "id"));
            if (del_id)
                del->id = (uint64_t)strtoll(del_id, NULL, 10);
            char *del_channel_id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "channel_id"));
            if (del_channel_id)
                del->channel_id = (uint64_t)strtoll(del_channel_id, NULL, 10);
            char *del_guild_id = cJSON_GetStringValue(cJSON_GetObjectItem(data, "guild_id"));
            if (del_guild_id)
                del->guild_id = (uint64_t)strtoll(del_guild_id, NULL, 10);
            del->message = del->id ? _d_copy_message(discord_cache_get_message(del->id)) : NULL;
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
    } else if (strncmp(event, "INTERACTION_CREATE", 19) == 0) {
        if (bot->callbacks->on_interaction_create) {
            event_pool_workload_t *work = (event_pool_workload_t *)malloc(sizeof(struct event_pool_workload));
            work->bot = bot;
            struct discord_interaction *interaction = _d_json_to_interaction(data);
            work->data = (void *)interaction;

            t_pool_add_work(bot->thread_pool, &event_handle_interaction_create, work);
        }
    }
}
