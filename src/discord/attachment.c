#include "structures/attachment.h"
#include "disco.h"

void *disco_create_attachment_struct_json(cJSON *data) {
    struct discord_attachment *attach = (struct discord_attachment *)calloc(1, sizeof(struct discord_attachment));
    attach->id = get_long_from_string_json(data, "id", 0);
    attach->filename = get_string_from_json(data, "filename");
    attach->description = get_string_from_json(data, "description");
    attach->content_type = get_string_from_json(data, "content_type");
    attach->size = get_int_from_json(data, "size", -1);
    attach->url = get_string_from_json(data, "url");
    attach->proxy_url = get_string_from_json(data, "proxy_url");
    attach->height = get_int_from_json(data, "height", -1);
    attach->width = get_int_from_json(data, "width", -1);
    attach->ephemeral = get_bool_from_json(data, "ephemeral", 0);
    return attach;
}

void disco_destroy_attachment(struct discord_attachment *attachment) {
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
