#include <discord/disco.h>
#include <discord/embed.h>

void *discord_create_embed_struct_json(cJSON *data) {
    struct discord_embed *embed = (struct discord_embed *)calloc(1, sizeof(struct discord_embed));
    cJSON *tmp = NULL;

    embed->title = _d_get_string_from_json(data, "title");
    embed->type = _d_get_string_from_json(data, "type");
    embed->description = _d_get_string_from_json(data, "description");
    embed->url = _d_get_string_from_json(data, "url");
    embed->timestamp = _d_get_string_from_json(data, "timestamp");
    embed->color = _d_get_int_from_json(data, "color", 0);

    // adds the sub-structs of the embed
    tmp = cJSON_GetObjectItem(data, "footer");
    if (tmp)
        embed->footer = (struct discord_embed_footer *)_d_json_to_embed(tmp);
    tmp = cJSON_GetObjectItem(data, "image");
    if (tmp)
        embed->image = (struct discord_embed_media *)discord_create_embed_media_struct_json(tmp);
    tmp = cJSON_GetObjectItem(data, "thumbnail");
    if (tmp)
        embed->thumbnail = (struct discord_embed_media *)discord_create_embed_media_struct_json(tmp);
    tmp = cJSON_GetObjectItem(data, "video");
    if (tmp)
        embed->video = (struct discord_embed_media *)discord_create_embed_media_struct_json(tmp);
    tmp = cJSON_GetObjectItem(data, "provider");
    if (tmp)
        embed->provider = (struct discord_embed_provider *)discord_create_embed_provider_struct_json(tmp);
    tmp = cJSON_GetObjectItem(data, "author");
    if (tmp)
        embed->author = (struct discord_embed_author *)discord_create_embed_author_struct_json(tmp);
    embed->fields_count = _d_get_array_from_json(data, "fields", (void ***)&embed->fields, sizeof(struct discord_embed_field), &discord_create_embed_field_struct_json);
    return embed;
}
void discord_destroy_embed(struct discord_embed *embed) {
    if (embed->title)
        free(embed->title);
    if (embed->type)
        free(embed->type);
    if (embed->description)
        free(embed->description);
    if (embed->url)
        free(embed->url);
    if (embed->timestamp)
        free(embed->title);
    if (embed->footer)
        discord_destroy_embed_footer(embed->footer);
    if (embed->image)
        discord_destroy_embed_media(embed->image);
    if (embed->thumbnail)
        discord_destroy_embed_media(embed->thumbnail);
    if (embed->video)
        discord_destroy_embed_media(embed->video);
    if (embed->provider)
        discord_destroy_embed_provider(embed->provider);
    if (embed->author)
        discord_destroy_embed_author(embed->author);
    for (int i = 0; i < embed->fields_count; i++)
        discord_destroy_embed_field(embed->fields[i]);
    if (embed->fields)
        free(embed->fields);
    free(embed);
}

void *_d_json_to_embed(cJSON *data) {
    struct discord_embed_footer *footer = (struct discord_embed_footer *)calloc(1, sizeof(struct discord_embed_footer));
    footer->text = _d_get_string_from_json(data, "text");
    footer->icon_url = _d_get_string_from_json(data, "icon_url");
    footer->proxy_icon_url = _d_get_string_from_json(data, "proxy_icon_url");
    return footer;
}
void discord_destroy_embed_footer(struct discord_embed_footer *footer) {
    if (footer->text)
        free(footer->text);
    if (footer->icon_url)
        free(footer->icon_url);
    if (footer->proxy_icon_url)
        free(footer->proxy_icon_url);
    free(footer);
}

void *discord_create_embed_media_struct_json(cJSON *data) {
    struct discord_embed_media *media = (struct discord_embed_media *)calloc(1, sizeof(struct discord_embed_media));
    media->url = _d_get_string_from_json(data, "url");
    media->proxy_url = _d_get_string_from_json(data, "url");
    media->height = _d_get_int_from_json(data, "height", -1);
    media->width = _d_get_int_from_json(data, "width", -1);
    return media;
}
void discord_destroy_embed_media(struct discord_embed_media *media) {
    if (media->url)
        free(media->url);
    if (media->proxy_url)
        free(media->proxy_url);
    free(media);
}

void *discord_create_embed_provider_struct_json(cJSON *data) {
    struct discord_embed_provider *prov = (struct discord_embed_provider *)calloc(1, sizeof(struct discord_embed_provider));
    prov->name = _d_get_string_from_json(data, "name");
    prov->url = _d_get_string_from_json(data, "url");
    return prov;
}
void discord_destroy_embed_provider(struct discord_embed_provider *provider) {
    if (provider->name)
        free(provider->name);
    if (provider->url)
        free(provider->url);
    free(provider);
}

void *discord_create_embed_author_struct_json(cJSON *data) {
    struct discord_embed_author *author = (struct discord_embed_author *)calloc(1, sizeof(struct discord_embed_author));
    author->name = _d_get_string_from_json(data, "name");
    author->url = _d_get_string_from_json(data, "url");
    author->icon_url = _d_get_string_from_json(data, "icon_url");
    author->proxy_icon_url = _d_get_string_from_json(data, "proxy_icon_url");
    return author;
}
void discord_destroy_embed_author(struct discord_embed_author *author) {
    if (author->name)
        free(author->name);
    if (author->icon_url)
        free(author->icon_url);
    if (author->url)
        free(author->url);
    if (author->proxy_icon_url)
        free(author->proxy_icon_url);
    free(author);
}

void *discord_create_embed_field_struct_json(cJSON *data) {
    struct discord_embed_field *field = (struct discord_embed_field *)calloc(1, sizeof(struct discord_embed_field));
    field->name = _d_get_string_from_json(data, "name");
    field->value = _d_get_string_from_json(data, "value");
    field->_inline = _d_get_bool_from_json(data, "inline", 0);
    return field;
}
void discord_destroy_embed_field(struct discord_embed_field *field) {
    if (field->name)
        free(field->name);
    if (field->value)
        free(field->value);
    free(field);
}
