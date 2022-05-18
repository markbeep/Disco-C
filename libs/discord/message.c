#include "./structures/message.h"
#include "../web/request.h"
#include "./structures/channel.h"
#include <curl/curl.h>

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
    if (field->is_inline)
        cJSON_AddItemToObject(field_obj, "inline", cJSON_CreateTrue());
    else
        cJSON_AddItemToObject(field_obj, "inline", cJSON_CreateFalse());
    cJSON_AddItemToArray(fields, field_obj);
}

static cJSON *create_embed(struct discord_embed *embed) {
    cJSON *embed_obj = cJSON_CreateObject();
    if (embed->title)
        cJSON_AddItemToObject(embed_obj, "title", cJSON_CreateString(embed->title));
    if (embed->description)
        cJSON_AddItemToObject(embed_obj, "description", cJSON_CreateString(embed->title));
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
        for (int i = 0; i < 10; i++) {
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
        for (int i = 0; i < allowed_mentions->roles_count; i++)
            cJSON_AddItemToArray(roles, cJSON_CreateString(allowed_mentions->roles[i]));
    }
    if (allowed_mentions->users_count > 0) {
        cJSON *users = cJSON_AddArrayToObject(allowed_mentions_obj, "users");
        for (int i = 0; i < allowed_mentions->users_count; i++)
            cJSON_AddItemToArray(users, cJSON_CreateString(allowed_mentions->users[i]));
    }
    if (allowed_mentions->replied_user)
        cJSON_AddItemToObject(allowed_mentions_obj, "replied_user", cJSON_CreateTrue());
    else
        cJSON_AddItemToObject(allowed_mentions_obj, "replied_user", cJSON_CreateFalse());
    return allowed_mentions_obj;
}

static cJSON *create_message_reference(struct discord_message_reference *ref) {
    cJSON *ref_obj = cJSON_CreateObject();
    if (ref->message_id)
        cJSON_AddItemToObject(ref_obj, "message_id", cJSON_CreateString(ref->message_id));
    if (ref->channel_id)
        cJSON_AddItemToObject(ref_obj, "channel_id", cJSON_CreateString(ref->channel_id));
    if (ref->guild_id)
        cJSON_AddItemToObject(ref_obj, "guild_id", cJSON_CreateString(ref->guild_id));
    if (ref->fail_if_not_exists)
        cJSON_AddItemToObject(ref_obj, "fail_if_not_exists", cJSON_CreateTrue());
    else
        cJSON_AddItemToObject(ref_obj, "fail_if_not_exists", cJSON_CreateFalse());
    return ref_obj;
}

void disco_channel_send_message(bot_client_t *bot, char *content, char *channel_id, struct discord_create_message *message) {
    (void)bot;

    char uri[48];
    sprintf(uri, "/channels/%s/messages", channel_id);
    cJSON *json = cJSON_CreateObject();

    // content
    if (content && strnlen(content, 1) > 0) {
        cJSON_AddItemToObject(json, "content", cJSON_CreateString(content));
    }
    if (message) {
        // embeds
        if (message->embeds_count > 0) {
            cJSON *embeds = cJSON_AddArrayToObject(json, "embeds");
            for (int i = 0; i < message->embeds_count; i++) {
                cJSON_AddItemToArray(embeds, create_embed(message->embeds[i]));
            }
        }
        // allowed mentions
        if (message->allowed_mentions)
            cJSON_AddItemToObject(json, "allowed_mentions", create_allowed_mentions(message->allowed_mentions));
        // message reference
        if (message->message_reference)
            cJSON_AddItemToObject(json, "message_reference", create_message_reference(message->message_reference));
        // TODO discord_component
        // if (message->components_count > 0) {}
        // stickers
        if (message->sticker_ids_count > 0) {
            cJSON *stickers = cJSON_CreateArray();
            for (int i = 0; i < message->sticker_ids_count; i++)
                cJSON_AddItemToArray(stickers, cJSON_CreateString(message->sticker_ids[i]));
            cJSON_AddItemToObject(json, "sticker_ids", stickers);
        }
        // TODO attachments
        // if (message->attachments_count > 0) {}
        // flags
        if (message->flags)
            cJSON_AddItemToObject(json, "flags", cJSON_CreateNumber(message->flags));
    }

    char *response;
    CURLcode res = request_post(uri, &response, json);
    if (res != CURLE_OK) {
        fprintf(stderr, "%d: POST failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            fprintf(stderr, "Have no connection to host\n");
        return;
    }
    fprintf(stderr, "Message sent!\n");
    fprintf(stderr, "Response: char = %s\n", response);

    cJSON_Delete(json);
    free(response);
}