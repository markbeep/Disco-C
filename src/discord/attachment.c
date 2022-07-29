#include <discord/attachment.h>
#include <discord/disco.h>

void *_d_json_to_attachment(cJSON *data) {
    struct discord_attachment *attach = (struct discord_attachment *)calloc(1, sizeof(struct discord_attachment));
    attach->id = _d_get_long_from_string_json(data, "id", 0);
    attach->filename = _d_get_string_from_json(data, "filename");
    attach->description = _d_get_string_from_json(data, "description");
    attach->content_type = _d_get_string_from_json(data, "content_type");
    attach->size = _d_get_int_from_json(data, "size", -1);
    attach->url = _d_get_string_from_json(data, "url");
    attach->proxy_url = _d_get_string_from_json(data, "proxy_url");
    attach->height = _d_get_int_from_json(data, "height", -1);
    attach->width = _d_get_int_from_json(data, "width", -1);
    attach->ephemeral = _d_get_bool_from_json(data, "ephemeral", 0);
    return attach;
}

struct discord_attachment *_d_copy_attachment(struct discord_attachment *src) {
    if (!src)
        return NULL;
    struct discord_attachment *c = (struct discord_attachment *)calloc(1, sizeof(struct discord_attachment));
    memcpy(c, src, sizeof(struct discord_attachment));
    if (src->filename)
        c->filename = strndup(src->filename, 100);
    if (src->description)
        c->description = strndup(src->description, 1025);
    if (src->content_type)
        c->content_type = strndup(src->content_type, 50);
    if (src->url)
        c->url = strndup(src->url, 100);
    if (src->url)
        c->url = strndup(src->url, 2048);
    if (src->proxy_url)
        c->proxy_url = strndup(src->proxy_url, 2048);
    return c;
}

void discord_destroy_attachment(struct discord_attachment *attachment) {
    if (attachment->filename)
        free(attachment->filename);
    if (attachment->description)
        free(attachment->description);
    if (attachment->content_type)
        free(attachment->content_type);
    if (attachment->url)
        free(attachment->url);
    if (attachment->proxy_url)
        free(attachment->proxy_url);
    free(attachment);
}
