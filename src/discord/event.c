#include <discord/event.h>
#include <discord/user.h>
#include <string.h>
#include <utils/cache.h>
#include <utils/disco_logging.h>
#include <utils/t_pool.h>
#include <utils/timer.h>
#include <web/websocket.h>

void event_handle(bot_client_t *bot, cJSON *data, char *event) {
    d_log_debug("Event: %s\n", event);

    if (!bot->callbacks) {
        d_log_err("No callback functions defined!\n");
        return;
    }

    if (strncmp(event, "READY", 6) == 0) {
        d_log_info("Received a READY event\n");
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
            bot->callbacks->on_ready(bot);
    } else if (strncmp(event, "RESUMED", 8) == 0) {

    } else if (strncmp(event, "RECONNECT", 10) == 0) {

    } else if (strncmp(event, "INVALID_SESSION", 16) == 0) {

    } else if (strncmp(event, "APPLICATION_COMMAND_PERMISSIONS_UPDATE", 39) == 0) {

    } else if (strncmp(event, "CHANNEL_CREATE", 15) == 0) {
        if (bot->callbacks->on_channel_create) {
            // the work struct is then freed inside the thread function
            struct discord_channel *channel = _d_json_to_channel(data);
            struct discord_channel *copy = (void *)_d_copy_channel(channel);
            d_log_debug("Channel ID = %ju\n", channel->id);

            // adds the channel to cache
            discord_cache_set_channel(channel);

            bot->callbacks->on_channel_create(bot, copy);
        }

    } else if (strncmp(event, "CHANNEL_UPDATE", 15) == 0) {
        if (bot->callbacks->on_channel_update) {
            // we free the channel struct when cleaning up the cache
            struct discord_channel *channel = _d_json_to_channel(data);

            // this gets freed inside the event handle function
            struct discord_channel *old = _d_copy_channel(discord_cache_get_channel(channel->id));
            struct discord_channel *new = _d_copy_channel(channel);

            d_log_debug("Channel ID = %ju\n", channel->id);

            // adds the new channel to the cache
            discord_cache_set_channel(channel);

            bot->callbacks->on_channel_update(bot, old, new);
        }

    } else if (strncmp(event, "CHANNEL_DELETE", 15) == 0) {
        if (bot->callbacks->on_channel_delete) {
            // we need to allocate the IDs anew, because the JSON with the original IDs gets freed
            char *del_id_s = cJSON_GetStringValue(cJSON_GetObjectItem(data, "id"));
            uint64_t del_id = 0;
            if (del_id_s)
                del_id = (uint64_t)strtoull(del_id_s, NULL, 10);
            char *del_parent_id_s = cJSON_GetStringValue(cJSON_GetObjectItem(data, "parent_id"));
            uint64_t del_parent_id = 0;
            if (del_parent_id_s)
                del_parent_id = (uint64_t)strtoull(del_parent_id_s, NULL, 10);
            char *del_guild_id_s = cJSON_GetStringValue(cJSON_GetObjectItem(data, "guild_id"));
            uint64_t del_guild_id = 0;
            if (del_guild_id_s)
                del_guild_id = (uint64_t)strtoull(del_guild_id_s, NULL, 10);
            cJSON *c = cJSON_GetObjectItem(data, "type");
            enum Discord_Channel_Type type = c->valueint;
            struct discord_channel *old = del_id ? _d_copy_channel(discord_cache_get_channel(del_id)) : NULL;
            bot->callbacks->on_channel_delete(bot, del_id, del_guild_id, del_parent_id, type, old);
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
            // we free the message struct when cleaning up the cache
            struct discord_message *message = _d_json_to_message(data);
            struct discord_message *copy = (void *)_d_copy_message(message);
            d_log_debug("Message ID = %ju\n", message->id);

            // adds the message to cache
            discord_cache_set_message(message);

            bot->callbacks->on_message(bot, copy);
        }
    } else if (strncmp(event, "MESSAGE_UPDATE", 15) == 0) {
        if (bot->callbacks->on_message_edit) {
            // we free the message struct when cleaning up the cache
            struct discord_message *message = _d_json_to_message(data);
            // to be freed inside the event handle function
            struct discord_message *old = _d_copy_message(discord_cache_get_message(message->id));
            struct discord_message *new = _d_copy_message(message);

            d_log_debug("Message ID = %ju\n", message->id);

            // adds the message to cache
            discord_cache_set_message(message);

            bot->callbacks->on_message_edit(bot, old, new);
        }
    } else if (strncmp(event, "MESSAGE_DELETE", 15) == 0) {
        if (bot->callbacks->on_message_delete) {
            // we need to allocate the IDs anew, because the JSON with the original IDs gets freed
            char *del_id_s = cJSON_GetStringValue(cJSON_GetObjectItem(data, "id"));
            uint64_t del_id = 0;
            if (del_id_s)
                del_id = (uint64_t)strtoull(del_id_s, NULL, 10);
            char *del_channel_id_s = cJSON_GetStringValue(cJSON_GetObjectItem(data, "channel_id"));
            uint64_t del_channel_id = 0;
            if (del_channel_id_s)
                del_channel_id = (uint64_t)strtoull(del_channel_id_s, NULL, 10);
            char *del_guild_id_s = cJSON_GetStringValue(cJSON_GetObjectItem(data, "guild_id"));
            uint64_t del_guild_id = 0;
            if (del_guild_id_s)
                del_guild_id = (uint64_t)strtoull(del_guild_id_s, NULL, 10);
            struct discord_message *old = del_id ? _d_copy_message(discord_cache_get_message(del_id)) : NULL;

            bot->callbacks->on_message_delete(bot, del_id, del_channel_id, del_guild_id, old);
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
            struct discord_interaction *interaction = _d_json_to_interaction(data);
            bot->callbacks->on_interaction_create(bot, interaction);
        }
    }
}
