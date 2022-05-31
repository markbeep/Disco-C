#ifndef DISCO_CACHE
#define DISCO_CACHE

#include "../discord/structures/message.h"
#include "hashmap.h/hashmap.h"

/**
 * @brief Initializes all the caches. Should be called once when starting up the bot.
 *
 * @return int 0 if succeeded, 1 otherwise
 */
int disco_cache_init();

/**
 * @brief Cleans up the cache and frees all the structs it contains
 *
 */
void disco_cache_destroy();

/**
 * @brief Adds a message to the cache by ID. If it already exists in cache,
 * it frees and overwrites the previous entry.
 * @param message
 * @return int
 */
int disco_cache_set_message(struct discord_message *message);

/**
 * @brief Retreives a message by ID in the cache.
 *
 * @param id
 * @return struct discord_message* Pointer to the message struct.
 * NULL if the message is not in the cache.
 */
struct discord_message *disco_cache_get_message(char *id);

#endif
