#ifndef DISCO_ATTACHMENT
#define DISCO_ATTACHMENT

#include <cJSON/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Attachment
 * Attachment functions and structures
 * @{
 *
 */

/** https://discord.com/developers/docs/resources/channel#attachment-object */
struct discord_attachment {
    uint64_t id;
    char *filename;
    char *description;
    char *content_type;
    int size;
    char *url;
    char *proxy_url;
    int height;
    int width;
    bool ephemeral;
};

/**
 * @brief Creates an attachment structure from a given JSON
 *
 * @param data
 * @return void* discord_attachment struct
 */
void *_d_json_to_attachment(cJSON *data);

/**
 * @brief Copies an attachment structure.
 *
 * @param attachment Attachment to copy.
 * @return struct discord_attachment*
 */
struct discord_attachment *_d_copy_attachment(struct discord_attachment *attachment);

/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param attachment
 */
void discord_destroy_attachment(struct discord_attachment *attachment);

/** @} @} */
#endif
