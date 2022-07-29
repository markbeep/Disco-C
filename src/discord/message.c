#include <discord/commands/message_commands.h>
#include <discord/interaction.h>
#include <discord/message.h>
#include <string.h>
#include <utils/cache.h>
#include <utils/disco_logging.h>
#include <web/request.h>

void *_d_json_to_message(cJSON *data) {
    struct discord_message *msg = (struct discord_message *)calloc(1, sizeof(struct discord_message));
    cJSON *tmp_json = NULL;

    msg->id = _d_get_long_from_string_json(data, "id", 0);
    msg->channel_id = _d_get_long_from_string_json(data, "channel_id", 0);
    msg->guild_id = _d_get_long_from_string_json(data, "guild_id", 0);
    msg->webhook_id = _d_get_long_from_string_json(data, "webhook_id", 0);
    if (!msg->webhook_id) {
        // only creates a user if its not a webhook
        cJSON *user = cJSON_GetObjectItem(data, "author");
        if (user) {
            msg->user = (struct discord_user *)_d_json_to_user(user);
        }
        // if theres additionally a member, it creates that as well
        cJSON *member = cJSON_GetObjectItem(data, "member");
        if (member) {
            msg->member = (struct discord_member *)_d_json_to_member(member, msg->user);
        }
    }
    msg->content = _d_get_string_from_json(data, "content");
    // gives us the guarantee that if the message content is empty,
    // the pointer is NULL
    if (msg->content && strnlen(msg->content, 1) == 0) {
        free(msg->content);
        msg->content = NULL;
    }
    msg->timestamp = _d_get_string_from_json(data, "timestamp");
    msg->tts = _d_get_bool_from_json(data, "tts", 0);
    msg->mention_everyone = _d_get_bool_from_json(data, "mention_everyone", 0);

    // mentions array
    tmp_json = cJSON_GetObjectItem(data, "mentions");
    msg->mentions_count = cJSON_GetArraySize(tmp_json);
    if (msg->mentions_count > 0) {
        msg->mentions = (struct discord_member **)malloc((size_t)msg->mentions_count * sizeof(struct discord_members *));
        int i = 0;
        cJSON *cur = NULL;
        cJSON_ArrayForEach(cur, tmp_json) {
            msg->mentions[i++] = (struct discord_member *)_d_json_to_member(cur, NULL);
        }
    }

    // mentioned roles array
    tmp_json = cJSON_GetObjectItem(data, "mention_roles");
    msg->mention_roles_count = cJSON_GetArraySize(tmp_json);
    if (msg->mention_roles_count > 0) {
        cJSON *cur = NULL;
        msg->mention_roles = (uint64_t *)malloc((size_t)msg->mention_roles_count * sizeof(uint64_t));
        int i = 0;
        cJSON_ArrayForEach(cur, tmp_json) {
            msg->mention_roles[i++] = (uint64_t)strtoll(cur->valuestring, NULL, 10);
        }
    }

    // mention channels
    msg->mention_channels_count = _d_get_array_from_json(data, "mention_channels", (void ***)&msg->mention_channels, sizeof(struct discord_channel_mention), &_d_json_to_channel_mention);

    // attachments
    msg->attachments_count = _d_get_array_from_json(data, "attachments", (void ***)&msg->attachments, sizeof(struct discord_attachment), &_d_json_to_attachment);

    // reactions
    msg->reactions_count = _d_get_array_from_json(data, "reactions", (void ***)&msg->reactions, sizeof(struct discord_reaction), &_d_json_to_reaction);

    msg->nonce = _d_get_string_from_json(data, "nonce");
    msg->pinned = _d_get_bool_from_json(data, "pinned", 0);
    msg->type = (enum Discord_Message_Type)_d_get_int_from_json(data, "type", 0);

    // activity
    tmp_json = cJSON_GetObjectItem(data, "activity");
    if (tmp_json)
        msg->activity = (struct discord_message_activity *)_d_json_to_message_activity(tmp_json);

    // application
    tmp_json = cJSON_GetObjectItem(data, "application");
    if (tmp_json)
        msg->application = (struct discord_application *)_d_json_to_application(tmp_json);
    msg->application_id = _d_get_long_from_string_json(data, "application_id", 0);

    // message_reference
    tmp_json = cJSON_GetObjectItem(data, "message_reference");
    if (tmp_json)
        msg->message_reference = (struct discord_message_reference *)_d_json_to_message_reference(tmp_json);

    msg->flags = _d_get_int_from_json(data, "flags", 0);

    // referenced_message
    tmp_json = cJSON_GetObjectItem(data, "referenced_message");
    if (tmp_json && !cJSON_IsNull(tmp_json))
        msg->referenced_message = (struct discord_message *)_d_json_to_message(tmp_json);

    // interaction
    tmp_json = cJSON_GetObjectItem(data, "interaction");
    if (tmp_json)
        msg->interaction = (struct discord_interaction *)_d_json_to_interaction(tmp_json);

    // components
    msg->components_count = _d_get_array_from_json(data, "components", (void ***)&msg->components, sizeof(struct discord_component), &_d_json_to_component);

    // sticker_items
    msg->sticker_items_count = _d_get_array_from_json(data, "sticker_items", (void ***)&msg->sticker_items, sizeof(struct discord_message_sticker_item), &_d_json_to_sticker_item);

    // stickers
    msg->stickers_count = _d_get_array_from_json(data, "stickers", (void ***)&msg->stickers, sizeof(struct discord_sticker), &_d_json_to_sticker);

    return msg;
}

struct discord_message *_d_copy_message(struct discord_message *msg) {
    if (!msg)
        return NULL;
    struct discord_message *c = (struct discord_message *)calloc(1, sizeof(struct discord_message));
    memcpy(c, msg, sizeof(struct discord_message));
    c->user = _d_copy_user(msg->user);
    c->member = _d_copy_member(c->member, c->user);
    if (msg->content)
        c->content = strndup(msg->content, 4096);
    if (msg->timestamp)
        c->timestamp = strndup(msg->timestamp, 50);
    if (msg->edited_timestamp)
        c->edited_timestamp = strndup(msg->edited_timestamp, 50);
    if (c->mentions_count > 0) {
        c->mentions = (struct discord_member **)malloc(c->mentions_count * sizeof(struct discord_member *));
        for (int i = 0; i < c->mentions_count; i++)
            c->mentions[i] = _d_copy_member(msg->mentions[i], NULL);
    }
    if (c->mention_roles_count > 0) {
        c->mention_roles = (uint64_t *)malloc(c->mention_roles_count * sizeof(uint64_t));
        for (int i = 0; i < c->mention_roles_count; i++)
            c->mention_roles[i] = msg->mention_roles[i];
    }
    if (c->mention_channels_count > 0) {
        c->mention_channels = (struct discord_channel **)malloc(c->mention_channels_count * sizeof(struct discord_channel *));
        for (int i = 0; i < c->mention_channels_count; i++)
            c->mention_channels[i] = _d_copy_channel(msg->mention_channels[i]);
    }
    if (c->attachments_count > 0) {
        c->attachments = (struct discord_attachment **)malloc(c->attachments_count * sizeof(struct discord_attachment *));
        for (int i = 0; i < c->attachments_count; i++)
            c->attachments[i] = _d_copy_attachment(msg->attachments[i]);
    }
    if (c->embeds_count > 0) {
        c->embeds = (struct discord_embed **)malloc(c->embeds_count * sizeof(struct discord_embed *));
        for (int i = 0; i < c->embeds_count; i++)
            c->embeds[i] = _d_copy_embed(msg->embeds[i]);
    }
    if (c->reactions_count > 0) {
        c->reactions = (struct discord_reaction **)malloc(c->reactions_count * sizeof(struct discord_reaction *));
        for (int i = 0; i < c->reactions_count; i++)
            c->reactions[i] = _d_copy_reaction(msg->reactions[i]);
    }
    if (msg->nonce)
        c->nonce = strndup(msg->nonce, 200);
    c->activity = _d_copy_message_activity(msg->activity);
    c->application = _d_copy_application(msg->application);
    c->message_reference = _d_copy_message_reference(msg->message_reference);
    c->referenced_message = _d_copy_message(msg->referenced_message);
    c->interaction = _d_copy_interaction(msg->interaction);
    c->thread = _d_copy_channel(msg->thread);
    if (c->components_count > 0) {
        c->components = (struct discord_component **)malloc(c->components_count * sizeof(struct discord_component *));
        for (int i = 0; i < c->components_count; i++)
            c->components[i] = _d_copy_component(msg->components[i]);
    }
    if (c->stickers_count > 0) {
        c->stickers = (struct discord_sticker **)malloc(c->stickers_count * sizeof(struct discord_sticker *));
        for (int i = 0; i < c->stickers_count; i++)
            c->stickers[i] = _d_copy_sticker(msg->stickers[i]);
    }
    return c;
}

void discord_destroy_message(struct discord_message *message) {
    if (message->member)
        discord_destroy_member(message->member);
    else if (message->user) // only delete user if there was no member (user is deleted in member)
        discord_destroy_user(message->user);
    if (message->content)
        free(message->content);
    if (message->timestamp)
        free(message->timestamp);
    if (message->edited_timestamp)
        free(message->edited_timestamp);
    for (int i = 0; i < message->mentions_count; i++)
        discord_destroy_member(message->mentions[i]);
    if (message->mentions)
        free(message->mentions);
    if (message->mention_roles)
        free(message->mention_roles);
    for (int i = 0; i < message->mention_channels_count; i++)
        discord_destroy_channel(message->mention_channels[i]);
    if (message->mention_channels)
        free(message->mention_channels);
    for (int i = 0; i < message->attachments_count; i++)
        discord_destroy_attachment(message->attachments[i]);
    if (message->attachments)
        free(message->attachments);
    for (int i = 0; i < message->embeds_count; i++)
        discord_destroy_embed(message->embeds[i]);
    if (message->embeds)
        free(message->embeds);
    for (int i = 0; i < message->reactions_count; i++)
        discord_destroy_reaction(message->reactions[i]);
    if (message->reactions)
        free(message->reactions);
    if (message->nonce)
        free(message->nonce);
    if (message->activity)
        discord_destroy_message_activity(message->activity);
    if (message->application)
        discord_destroy_application(message->application);
    if (message->message_reference)
        discord_destroy_message_reference(message->message_reference);
    if (message->interaction)
        discord_destroy_interaction(message->interaction);
    if (message->thread)
        discord_destroy_channel(message->thread);
    for (int i = 0; i < message->components_count; i++)
        discord_destroy_component(message->components[i]);
    if (message->components)
        free(message->components);
    for (int i = 0; i < message->sticker_items_count; i++)
        discord_destroy_message_sticker(message->sticker_items[i]);
    if (message->sticker_items)
        free(message->sticker_items);
    for (int i = 0; i < message->stickers_count; i++)
        discord_destroy_sticker(message->stickers[i]);
    if (message->stickers)
        free(message->stickers);
    free(message);
}

void *_d_json_to_message_reference(cJSON *data) {
    struct discord_message_reference *msg = (struct discord_message_reference *)calloc(1, sizeof(struct discord_message_reference));
    msg->message_id = _d_get_long_from_string_json(data, "message_id", 0);
    msg->channel_id = _d_get_long_from_string_json(data, "channel_id", 0);
    msg->guild_id = _d_get_long_from_string_json(data, "guild_id", 0);
    return msg;
}

struct discord_message_reference *_d_copy_message_reference(struct discord_message_reference *src) {
    if (!src)
        return NULL;
    struct discord_message_reference *c = (struct discord_message_reference *)calloc(1, sizeof(struct discord_message_reference));
    memcpy(c, src, sizeof(struct discord_message_reference));
    return c;
}

void discord_destroy_message_reference(struct discord_message_reference *message) {
    free(message);
}

static void embed_add_footer(cJSON *embed, struct discord_embed_footer *footer) {
    cJSON *footer_obj = cJSON_CreateObject();
    if (footer->text)
        cJSON_AddItemToObject(footer_obj, "text", cJSON_CreateString(footer->text));
    if (footer->icon_url)
        cJSON_AddItemToObject(footer_obj, "icon_url", cJSON_CreateString(footer->icon_url));
    if (footer->proxy_icon_url)
        cJSON_AddItemToObject(footer_obj, "proxy_icon_url", cJSON_CreateString(footer->proxy_icon_url));
    cJSON_AddItemToObject(embed, "footer", footer_obj);
}

static void embed_add_media(cJSON *embed, struct discord_embed_media *media, char *media_type) {
    cJSON *media_obj = cJSON_CreateObject();
    if (media->url)
        cJSON_AddItemToObject(media_obj, "url", cJSON_CreateString(media->url));
    if (media->proxy_url)
        cJSON_AddItemToObject(media_obj, "proxy_url", cJSON_CreateString(media->proxy_url));
    cJSON_AddItemToObject(media_obj, "height", cJSON_CreateNumber(media->height));
    cJSON_AddItemToObject(media_obj, "width", cJSON_CreateNumber(media->width));
    cJSON_AddItemToObject(embed, media_type, media_obj);
}

static void embed_add_author(cJSON *embed, struct discord_embed_author *author) {
    cJSON *author_obj = cJSON_CreateObject();
    if (author->name)
        cJSON_AddItemToObject(author_obj, "name", cJSON_CreateString(author->name));
    if (author->url)
        cJSON_AddItemToObject(author_obj, "url", cJSON_CreateString(author->url));
    if (author->icon_url)
        cJSON_AddItemToObject(author_obj, "icon_url", cJSON_CreateString(author->icon_url));
    if (author->proxy_icon_url)
        cJSON_AddItemToObject(author_obj, "proxy_icon_url", cJSON_CreateString(author->proxy_icon_url));
    cJSON_AddItemToObject(embed, "author", author_obj);
}

static void embed_add_field(cJSON *fields, struct discord_embed_field *field) {
    cJSON *field_obj = cJSON_CreateObject();
    if (field->name)
        cJSON_AddItemToObject(field_obj, "name", cJSON_CreateString(field->name));
    if (field->value)
        cJSON_AddItemToObject(field_obj, "value", cJSON_CreateString(field->value));
    if (field->_inline)
        cJSON_AddItemToObject(field_obj, "inline", cJSON_CreateTrue());
    else
        cJSON_AddItemToObject(field_obj, "inline", cJSON_CreateFalse());
    cJSON_AddItemToArray(fields, field_obj);
}

static cJSON *create_embed_json(struct discord_embed *embed) {
    cJSON *embed_obj = cJSON_CreateObject();
    if (embed->title)
        cJSON_AddItemToObject(embed_obj, "title", cJSON_CreateString(embed->title));
    if (embed->description)
        cJSON_AddItemToObject(embed_obj, "description", cJSON_CreateString(embed->description));
    cJSON_AddItemToObject(embed_obj, "type", cJSON_CreateString(embed->type));
    if (embed->url)
        cJSON_AddItemToObject(embed_obj, "url", cJSON_CreateString(embed->url));
    if (embed->timestamp)
        cJSON_AddItemToObject(embed_obj, "timestamp", cJSON_CreateString(embed->timestamp));
    cJSON_AddItemToObject(embed_obj, "color", cJSON_CreateNumber(embed->color));
    if (embed->footer)
        embed_add_footer(embed_obj, embed->footer);
    if (embed->image)
        embed_add_media(embed_obj, embed->image, "image");
    if (embed->thumbnail)
        embed_add_media(embed_obj, embed->thumbnail, "thumbnail");
    if (embed->video)
        embed_add_media(embed_obj, embed->video, "video");
    if (embed->provider) {
        cJSON *provider_obj = cJSON_CreateObject();
        if (embed->provider->name)
            cJSON_AddItemToObject(provider_obj, "name", cJSON_CreateString(embed->provider->name));
        if (embed->provider->name)
            cJSON_AddItemToObject(provider_obj, "url", cJSON_CreateString(embed->provider->url));
        cJSON_AddItemToObject(embed_obj, "provider", provider_obj);
    }
    if (embed->author)
        embed_add_author(embed_obj, embed->author);
    if (embed->fields_count > 0) {
        cJSON *fields = cJSON_AddArrayToObject(embed_obj, "fields");
        for (int i = 0; i < embed->fields_count; i++) {
            embed_add_field(fields, embed->fields[i]);
        }
    }
    return embed_obj;
}

static cJSON *create_allowed_mentions(struct discord_allowed_mentions *allowed_mentions) {
    cJSON *allowed_mentions_obj = cJSON_CreateObject();
    cJSON *parse = cJSON_AddArrayToObject(allowed_mentions_obj, "parse");
    if (allowed_mentions->parse_role_mentions)
        cJSON_AddItemToArray(parse, cJSON_CreateString("roles"));
    if (allowed_mentions->parse_user_mentions)
        cJSON_AddItemToArray(parse, cJSON_CreateString("users"));
    if (allowed_mentions->parse_everyone_mentions)
        cJSON_AddItemToArray(parse, cJSON_CreateString("everyone"));
    if (allowed_mentions->roles_count > 0) {
        cJSON *roles = cJSON_AddArrayToObject(allowed_mentions_obj, "roles");
        for (int i = 0; i < allowed_mentions->roles_count; i++) {
            char tmp[20];
            sprintf(tmp, "%ju", allowed_mentions->roles[i]);
            cJSON_AddItemToArray(roles, cJSON_CreateString(tmp));
        }
    }
    if (allowed_mentions->users_count > 0) {
        cJSON *users = cJSON_AddArrayToObject(allowed_mentions_obj, "users");
        for (int i = 0; i < allowed_mentions->users_count; i++) {
            char tmp[20];
            sprintf(tmp, "%ju", allowed_mentions->users[i]);
            cJSON_AddItemToArray(users, cJSON_CreateString(tmp));
        }
    }
    if (allowed_mentions->replied_user)
        cJSON_AddItemToObject(allowed_mentions_obj, "replied_user", cJSON_CreateTrue());
    else
        cJSON_AddItemToObject(allowed_mentions_obj, "replied_user", cJSON_CreateFalse());
    return allowed_mentions_obj;
}

static cJSON *create_message_reference(struct discord_message_reference *ref) {
    cJSON *ref_obj = cJSON_CreateObject();
    char tmp[20];
    if (ref->message_id) {
        sprintf(tmp, "%ju", ref->message_id);
        cJSON_AddItemToObject(ref_obj, "message_id", cJSON_CreateString(tmp));
    }
    if (ref->channel_id) {
        sprintf(tmp, "%ju", ref->channel_id);
        cJSON_AddItemToObject(ref_obj, "channel_id", cJSON_CreateString(tmp));
    }
    if (ref->guild_id) {
        sprintf(tmp, "%ju", ref->guild_id);
        cJSON_AddItemToObject(ref_obj, "guild_id", cJSON_CreateString(tmp));
    }
    if (ref->fail_if_not_exists)
        cJSON_AddItemToObject(ref_obj, "fail_if_not_exists", cJSON_CreateTrue());
    else
        cJSON_AddItemToObject(ref_obj, "fail_if_not_exists", cJSON_CreateFalse());
    return ref_obj;
}

void _d_create_message_to_json(cJSON *json, char *content, struct discord_create_message *message) {
    // content
    if (content && strnlen(content, 1) > 0) {
        cJSON_AddItemToObject(json, "content", cJSON_CreateString(content));
    }
    if (message) {
        // embeds
        if (message->embeds_count > 0 || message->embed) {
            cJSON *embeds = cJSON_AddArrayToObject(json, "embeds");
            if (message->embed)
                cJSON_AddItemToArray(embeds, create_embed_json(message->embed));
            for (int i = 0; i < message->embeds_count; i++) {
                cJSON_AddItemToArray(embeds, create_embed_json(message->embeds[i]));
            }
        }
        // allowed mentions
        if (message->allowed_mentions)
            cJSON_AddItemToObject(json, "allowed_mentions", create_allowed_mentions(message->allowed_mentions));
        // message reference
        if (message->message_reference)
            cJSON_AddItemToObject(json, "message_reference", create_message_reference(message->message_reference));
        // stickers
        if (message->sticker_ids_count > 0) {
            cJSON *stickers = cJSON_CreateArray();
            for (int i = 0; i < message->sticker_ids_count; i++) {
                char tmp[20];
                sprintf(tmp, "%ju", message->sticker_ids[i]);
                cJSON_AddItemToArray(stickers, cJSON_CreateString(tmp));
            }
            cJSON_AddItemToObject(json, "sticker_ids", stickers);
        }
        // TODO discord_component
        // if (message->components_count > 0) {}
        // TODO attachments
        // if (message->attachments_count > 0) {}
        // flags
        if (message->flags)
            cJSON_AddItemToObject(json, "flags", cJSON_CreateNumber(message->flags));
    }
}

struct discord_message *discord_channel_send_message(bot_client_t *bot, char *content, uint64_t channel_id, struct discord_create_message *message, bool return_struct) {
    cJSON *json = cJSON_CreateObject();
    _d_create_message_to_json(json, content, message);

    char uri[80];
    sprintf(uri, "https://discord.com/api/channels/%ju/messages", channel_id);
    char *response;
    long res = request(uri, &response, json, REQUEST_POST, bot->websocket_client->token);
    struct discord_message *sent_message = NULL;
    d_log_debug("Message sent! Response: %ld\n", res);
    if (response && res != 0 && return_struct) { // only if a struct is requested to be returned
        cJSON *res_json = cJSON_Parse(response);
        sent_message = (struct discord_message *)_d_json_to_message(res_json);
        cJSON_Delete(res_json);
    }
    // free up allocated stuff
    cJSON_Delete(json);
    free(response);
    return sent_message;
}

void discord_channel_edit_message(bot_client_t *bot, char *content, uint64_t channel_id, uint64_t message_id, struct discord_create_message *message) {
    cJSON *json = cJSON_CreateObject();

    // content
    if (content && strnlen(content, 1) > 0) {
        cJSON_AddItemToObject(json, "content", cJSON_CreateString(content));
    }
    if (message) {
        // embeds
        if (message->embeds_count > 0 || message->embed) {
            cJSON *embeds = cJSON_AddArrayToObject(json, "embeds");
            if (message->embed)
                cJSON_AddItemToArray(embeds, create_embed_json(message->embed));
            for (int i = 0; i < message->embeds_count; i++) {
                cJSON_AddItemToArray(embeds, create_embed_json(message->embeds[i]));
            }
        }
        // allowed mentions
        if (message->allowed_mentions)
            cJSON_AddItemToObject(json, "allowed_mentions", create_allowed_mentions(message->allowed_mentions));
        // TODO discord_component
        // if (message->components_count > 0) {}
        // TODO attachments
        // if (message->attachments_count > 0) {}
        // flags
        if (message->flags)
            cJSON_AddItemToObject(json, "flags", cJSON_CreateNumber(message->flags));
    }

    char uri[100];
    sprintf(uri, "https://discord.com/api/channels/%ju/messages/%ju", channel_id, message_id);
    char *response;
    long res = request(uri, &response, json, REQUEST_PATCH, bot->websocket_client->token);
    d_log_err("Message Edited. Response: %ld\n", res);

    cJSON_Delete(json);
    free(response);
}

int discord_get_messages(bot_client_t *bot, uint64_t channel_id, struct discord_message ***message_array, int limit, uint64_t around, uint64_t before, uint64_t after) {
    char url[200];
    sprintf(url, "https://discord.com/api/channels/%ju/messages?", channel_id);
    char id_field[35];
    bool added_id = false;
    if (around) {
        sprintf(id_field, "around=%ju", around);
        added_id = true;
    } else if (before) {
        sprintf(id_field, "before=%ju", before);
        added_id = true;
    } else if (after) {
        sprintf(id_field, "after=%ju", after);
        added_id = true;
    }
    if (added_id)
        strncat(url, id_field, 35);
    if (limit) {
        char lim[15];
        if (added_id) // to add the & sign
            sprintf(lim, "&limit=%d", limit);
        else
            sprintf(lim, "limit=%d", limit);
        strncat(url, lim, 15);
    }
    char *response;
    long res = request(url, &response, NULL, REQUEST_GET, bot->websocket_client->token);

    // handle the response and parse the json into a message array
    int received = 0;
    cJSON *recv = cJSON_Parse(response);
    if (res != 200 || !cJSON_IsArray(recv))
        return 0;

    *message_array = (struct discord_message **)malloc(sizeof(struct discord_message *) * received);
    cJSON *c_message;
    struct discord_message *msg;
    cJSON_ArrayForEach(c_message, recv) {
        msg = (struct discord_message *)_d_json_to_message(c_message);
        (*message_array)[received++] = msg;
        if (msg)
            discord_cache_set_message(msg);
    }

    return received;
}

struct discord_message *discord_get_message(bot_client_t *bot, uint64_t channel_id, uint64_t message_id) {
    struct discord_message *msg = discord_cache_get_message(message_id);
    if (msg)
        return msg;
    return discord_fetch_message(bot, channel_id, message_id);
}

struct discord_message *discord_fetch_message(bot_client_t *bot, uint64_t channel_id, uint64_t message_id) {
    char url[100];
    sprintf(url, "https://discord.com/api/channels/%ju/messages/%ju", channel_id, message_id);
    char *response;
    long res = request(url, &response, NULL, REQUEST_GET, bot->websocket_client->token);
    cJSON *c_message = cJSON_Parse(response);
    if (res != 200 || !c_message)
        return NULL; // no result or no response
    struct discord_message *msg = (struct discord_message *)_d_json_to_message(c_message);
    if (msg)
        discord_cache_set_message(msg);
    return msg;
}
