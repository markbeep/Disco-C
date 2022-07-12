#ifndef DISCO_EMBED
#define DISCO_EMBED

#include <cJSON/cJSON.h>
#include <stdbool.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Embed
 * Embed functions and structures
 * @{
 *
 */

struct discord_embed_footer {
    char *text;
    char *icon_url;
    char *proxy_icon_url;
};

/**
 * @brief Creates an embed_footer structure from a given JSON
 *
 * @param data
 * @return void* discord_embed_footer struct
 */
void *discord_create_embed_footer_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param footer
 */
void discord_destroy_embed_footer(struct discord_embed_footer *footer);

// video, image and thumbnail are all the same format
struct discord_embed_media {
    char *url;
    char *proxy_url;
    int height;
    int width;
};

/**
 * @brief Creates an embed_media structure from a given JSON
 *
 * @param data
 * @return void* discord_embed_media struct
 */
void *discord_create_embed_media_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param media
 */
void discord_destroy_embed_media(struct discord_embed_media *media);

struct discord_embed_provider {
    char *name;
    char *url;
};

/**
 * @brief Creates an embed_provider structure from a given JSON
 *
 * @param data
 * @return void* discord_embed_provider struct
 */
void *discord_create_embed_provider_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param provider
 */
void discord_destroy_embed_provider(struct discord_embed_provider *provider);

struct discord_embed_author {
    char *name;
    char *url;
    char *icon_url;
    char *proxy_icon_url;
};

/**
 * @brief Creates an embed_author structure from a given JSON
 *
 * @param data
 * @return void* discord_embed_author struct
 */
void *discord_create_embed_author_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param author
 */
void discord_destroy_embed_author(struct discord_embed_author *author);

struct discord_embed_field {
    char *name;
    char *value;
    bool _inline; // called "inline" in the Discord docs
};

/**
 * @brief Creates an embed_field structure from a given JSON
 *
 * @param data
 * @return void* discord_embed_field struct
 */
void *discord_create_embed_field_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param field
 */
void discord_destroy_embed_field(struct discord_embed_field *field);

// https://discord.com/developers/docs/resources/channel#embed-object
struct discord_embed {
    char *title;
    char *type; // should always be "rich" for generic embed
    char *description;
    char *url;
    char *timestamp;
    int color;
    struct discord_embed_footer *footer;
    struct discord_embed_media *image;
    struct discord_embed_media *thumbnail;
    struct discord_embed_media *video;
    struct discord_embed_provider *provider;
    struct discord_embed_author *author;
    struct discord_embed_field **fields;
    int fields_count;
};

/**
 * @brief Creates an embed structure from a given JSON
 *
 * @param data
 * @return void* discord_embed struct
 */
void *discord_create_embed_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param embed
 */
void discord_destroy_embed(struct discord_embed *embed);

/** @} @} */
#endif
