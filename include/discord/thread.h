#ifndef DISCO_THREAD
#define DISCO_THREAD

#include <cJSON/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Thread
 * Thread functions and structures
 * @{
 *
 */

/** https://discord.com/developers/docs/resources/channel#thread-metadata-object */
struct discord_thread_metadata {
    bool archived;
    int auto_archive_duration;
    char *archive_timestamp;
    bool locked;
    bool invitable;
    char *create_timestamp;
};

/**
 * @brief Creates a thread metadata structure from a given JSON
 *
 * @param data
 * @return void* discord_thread_metadata struct
 */
void *_d_json_to_thread_metadata(cJSON *data);

/**
 * @brief Copies a thread metadata structure.
 *
 * @param src Thread metadata to copy.
 * @return struct discord_thread_metadata*
 */
struct discord_thread_metadata *_d_copy_thread_metadata(struct discord_thread_metadata *src);

/**
 * @brief Destroys the given structure and frees the pointer.
 *
 * @param thread
 */
void discord_destroy_thread_metadata(struct discord_thread_metadata *thread);

/** https://discord.com/developers/docs/resources/channel#thread-member-object */
struct discord_thread_member {
    uint64_t id;
    uint64_t user_id;
    char *join_timestamp;
    int flags;
};

/**
 * @brief Creates a thread member structure from a given JSON
 *
 * @param data
 * @return void* discord_thread_member struct
 */
void *_d_json_to_thread_member(cJSON *data);

/**
 * @brief Copies a thread member structure.
 *
 * @param src Thread member to copy.
 * @return struct discord_thread_member*
 */
struct discord_thread_member *_d_copy_thread_member(struct discord_thread_member *src);

/**
 * @brief Destroys the given structure and frees the pointer.
 *
 * @param member
 */
void discord_destroy_thread_member(struct discord_thread_member *member);

/** @} @} */
#endif
