#ifndef DISCO_STICKER
#define DISCO_STICKER

#include "user.h"
#include <cJSON/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Sticker
 * Sticker functions and structures
 * @{
 *
 */

enum Discord_Sticker_Format { STICKER_FORMAT_PNG = 1,
                              STICKER_FORMAT_APNG = 2,
                              STICKER_FORMAT_LOTTIE = 3 };

// https://discord.com/developers/docs/resources/sticker#sticker-item-object
struct discord_message_sticker_item {
    char *id;
    char *name;
    enum Discord_Sticker_Format format_type;
};

/**
 * @brief Creates a sticker struct item with the given JSON.
 *
 * @param data
 * @return void*
 */
void *_d_json_to_sticker_item(cJSON *data);

/**
 * @brief Destroys the sticker item structure and frees the pointer.
 *
 * @param sticker
 */
void discord_destroy_message_sticker(struct discord_message_sticker_item *message_sticker);

enum Discord_Sticker_Type { STICKER_TYPE_STANDARD = 1,
                            STICKER_TYPE_GUILD = 2 };

// https://discord.com/developers/docs/resources/sticker#sticker-object
struct discord_sticker {
    char *id;
    uint64_t pack_id;
    char *name;
    char *description;
    char *tags;
    enum Discord_Sticker_Type type;
    enum Discord_Sticker_Format format_type;
    bool available;
    char *guild_id;
    struct discord_user *user;
    int sort_value;
};

/**
 * @brief Creates a sticker struct with the given JSON.
 *
 * @param data
 * @return void*
 */
void *_d_json_to_sticker(cJSON *data);

/**
 * @brief Copies a sticker structure.
 *
 * @param src Sticker to copy.
 * @return struct discord_sticker*
 */
struct discord_sticker *_d_copy_sticker(struct discord_sticker *src);

/**
 * @brief Destroys the sticker structure and frees the pointer.
 *
 * @param sticker
 */
void discord_destroy_sticker(struct discord_sticker *sticker);

/** @} @} */
#endif
