#include <discord/commands/message_send.h>
#include <discord/interaction.h>
#include <discord/message.h>
#include <string.h>
#include <utils/disco_logging.h>
#include <web/request.h>

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
            sprintf(tmp, "%ld", allowed_mentions->roles[i]);
            cJSON_AddItemToArray(roles, cJSON_CreateString(tmp));
        }
    }
    if (allowed_mentions->users_count > 0) {
        cJSON *users = cJSON_AddArrayToObject(allowed_mentions_obj, "users");
        for (int i = 0; i < allowed_mentions->users_count; i++) {
            char tmp[20];
            sprintf(tmp, "%ld", allowed_mentions->users[i]);
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
        sprintf(tmp, "%ld", ref->message_id);
        cJSON_AddItemToObject(ref_obj, "message_id", cJSON_CreateString(tmp));
    }
    if (ref->channel_id) {
        sprintf(tmp, "%ld", ref->channel_id);
        cJSON_AddItemToObject(ref_obj, "channel_id", cJSON_CreateString(tmp));
    }
    if (ref->guild_id) {
        sprintf(tmp, "%ld", ref->guild_id);
        cJSON_AddItemToObject(ref_obj, "guild_id", cJSON_CreateString(tmp));
    }
    if (ref->fail_if_not_exists)
        cJSON_AddItemToObject(ref_obj, "fail_if_not_exists", cJSON_CreateTrue());
    else
        cJSON_AddItemToObject(ref_obj, "fail_if_not_exists", cJSON_CreateFalse());
    return ref_obj;
}

void discord_fill_json_with_message(cJSON *json, char *content, struct discord_create_message *message) {
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
                sprintf(tmp, "%ld", message->sticker_ids[i]);
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
    discord_fill_json_with_message(json, content, message);
    d_log_debug("Snd msg: %s\n", cJSON_Print(json));

    char uri[80];
    sprintf(uri, "https://discord.com/api/channels/%ld/messages", channel_id);
    printf("%s\n", uri);
    char *response;
    CURLcode res = request(uri, &response, json, REQUEST_POST, bot->websocket_client->token);
    struct discord_message *sent_message = NULL;
    if (res != CURLE_OK) {
        d_log_err("%d: POST failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            d_log_err("Have no connection to host\n");
    } else {
        d_log_debug("Message sent!\n");
        d_log_debug("Response: char = %s\n", response);

        if (return_struct) { // only if a struct is requested to be returned
            cJSON *res_json = cJSON_Parse(response);
            sent_message = (struct discord_message *)discord_create_message_struct_json(res_json);
            cJSON_Delete(res_json);
        }
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
    sprintf(uri, "https://discord.com/api/channels/%ld/messages/%ld", channel_id, message_id);
    char *response;
    CURLcode res = request(uri, &response, json, REQUEST_PATCH, bot->websocket_client->token);
    if (res != CURLE_OK) {
        d_log_err("%d: PATCH failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            d_log_err("Have no connection to host\n");
        return;
    }
    d_log_debug("Message sent!\n");
    d_log_debug("Response: char = %s\n", response);

    cJSON_Delete(json);
    free(response);
}

void *discord_create_message_struct_json(cJSON *data) {
    struct discord_message *msg = (struct discord_message *)calloc(1, sizeof(struct discord_message));
    cJSON *tmp_json = NULL;

    msg->id = get_long_from_string_json(data, "id", 0);
    msg->channel_id = get_long_from_string_json(data, "channel_id", 0);
    msg->guild_id = get_long_from_string_json(data, "guild_id", 0);
    msg->webhook_id = get_long_from_string_json(data, "webhook_id", 0);
    if (!msg->webhook_id) {
        // only creates a user if its not a webhook
        cJSON *user = cJSON_GetObjectItem(data, "author");
        if (user) {
            msg->user = (struct discord_user *)discord_create_user_struct_json(user);
        }
        // if theres additionally a member, it creates that as well
        cJSON *member = cJSON_GetObjectItem(data, "member");
        if (member) {
            msg->member = (struct discord_member *)discord_create_member_struct_json(member, msg->user);
        }
    }
    msg->content = get_string_from_json(data, "content");
    // gives us the guarantee that if the message content is empty,
    // the pointer is NULL
    if (msg->content && strnlen(msg->content, 1) == 0)
        msg->content = NULL;
    msg->timestamp = get_string_from_json(data, "timestamp");
    msg->tts = get_bool_from_json(data, "tts", 0);
    msg->mention_everyone = get_bool_from_json(data, "mention_everyone", 0);

    // mentions array
    tmp_json = cJSON_GetObjectItem(data, "mentions");
    msg->mentions_count = cJSON_GetArraySize(tmp_json);
    if (msg->mentions_count > 0) {
        msg->mentions = (struct discord_member **)malloc((size_t)msg->mentions_count * sizeof(struct discord_members *));
        int i = 0;
        cJSON *cur = NULL;
        cJSON_ArrayForEach(cur, tmp_json) {
            msg->mentions[i++] = (struct discord_member *)discord_create_member_struct_json(cur, NULL);
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
    msg->mention_channels_count = get_array_from_json(data, "mention_channels", (void ***)&msg->mention_channels, sizeof(struct discord_channel_mention), &discord_create_channel_mention_struct_json);

    // attachments
    msg->attachments_count = get_array_from_json(data, "attachments", (void ***)&msg->attachments, sizeof(struct discord_attachment), &discord_create_attachment_struct_json);

    // reactions
    msg->reactions_count = get_array_from_json(data, "reactions", (void ***)&msg->reactions, sizeof(struct discord_reaction), &discord_create_reaction_struct_json);

    msg->nonce = get_string_from_json(data, "nonce");
    msg->pinned = get_bool_from_json(data, "pinned", 0);
    msg->type = get_int_from_json(data, "type", 0);

    // activity
    tmp_json = cJSON_GetObjectItem(data, "activity");
    if (tmp_json)
        msg->activity = (struct discord_message_activity *)discord_create_message_activity_struct_json(tmp_json);

    // application
    tmp_json = cJSON_GetObjectItem(data, "application");
    if (tmp_json)
        msg->application = (struct discord_application *)discord_create_application_struct_json(tmp_json);
    msg->application_id = get_long_from_string_json(data, "application_id", 0);

    // message_reference
    tmp_json = cJSON_GetObjectItem(data, "message_reference");
    if (tmp_json)
        msg->message_reference = (struct discord_message_reference *)discord_create_message_reference_struct_json(tmp_json);

    msg->flags = get_int_from_json(data, "flags", 0);

    // referenced_message
    tmp_json = cJSON_GetObjectItem(data, "referenced_message");
    if (tmp_json && !cJSON_IsNull(tmp_json))
        msg->referenced_message = (struct discord_message *)discord_create_message_struct_json(tmp_json);

    // interaction
    tmp_json = cJSON_GetObjectItem(data, "interaction");
    if (tmp_json)
        msg->interaction = (struct discord_interaction *)discord_create_interaction_struct_json(tmp_json);

    // components
    msg->components_count = get_array_from_json(data, "components", (void ***)&msg->components, sizeof(struct discord_component), &discord_create_component_struct_json);

    // sticker_items
    msg->sticker_items_count = get_array_from_json(data, "sticker_items", (void ***)&msg->sticker_items, sizeof(struct discord_message_sticker_item), &discord_create_message_sticker_item_struct_json);

    // stickers
    msg->stickers_count = get_array_from_json(data, "stickers", (void ***)&msg->stickers, sizeof(struct discord_sticker), &discord_create_sticker_struct_json);

    return msg;
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

void *discord_create_message_reference_struct_json(cJSON *data) {
    struct discord_message_reference *msg = (struct discord_message_reference *)calloc(1, sizeof(struct discord_message_reference));
    msg->message_id = get_long_from_string_json(data, "message_id", 0);
    msg->channel_id = get_long_from_string_json(data, "channel_id", 0);
    msg->guild_id = get_long_from_string_json(data, "guild_id", 0);
    return msg;
}
void discord_destroy_message_reference(struct discord_message_reference *message) {
    free(message);
}
