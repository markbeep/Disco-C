#ifndef DISCO_ATTACHMENT
#define DISCO_ATTACHMENT

#include "../../utils/cJSON/cJSON.h"

// https://discord.com/developers/docs/resources/channel#attachment-object
struct discord_attachment {
    char *id;
    char *filename;
    char *description;
    char *content_type;
    int size;
    char *url;
    char *proxy_url;
    int height;
    int width;
    int ephemeral;
};

/**
 * @brief Creates an attachment structure from a given JSON
 *
 * @param data
 * @return void* discord_attachment struct
 */
void *disco_create_attachment_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param attachment
 */
void disco_destroy_attachment(struct discord_attachment *attachment);

#endif
