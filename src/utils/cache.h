#ifndef DISCO_CACHE
#define DISCO_CACHE

#include "../discord/structures/channel.h"
#include "../discord/structures/guild.h"
#include "../discord/structures/message.h"
#include <hashmap.h/hashmap.h>

enum Disco_Cache_Type { DISCO_CHANNEL_CACHE,
                        DISCO_MESSAGE_CACHE,
                        DISCO_GUILD_CACHE };

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
struct discord_message *disco_cache_get_message(int64_t id);

/**
 * @brief Removes a message from the cache and
 * deallocates the message from memory
 *
 * @param id ID of the message to remove
 */
void disco_cache_delete_message(int64_t id);

/**
 * @brief Adds a channel to the cache by ID. If it already exists in cache,
 * it frees and overwrites the previous entry.
 * @param channel
 * @return int
 */
int disco_cache_set_channel(struct discord_channel *channel);

/**
 * @brief Retreives a channel by ID in the cache.
 *
 * @param id
 * @return struct discord_channel* Pointer to the channel struct.
 * NULL if the channel is not in the cache.
 */
struct discord_channel *disco_cache_get_channel(int64_t id);

/**
 * @brief Removes a channel from the cache and
 * deallocates the channel from memory
 *
 * @param id ID of the channel to remove
 */
void disco_cache_delete_channel(int64_t id);

/**
 * @brief Adds a guild to the cache by ID. If it already exists in cache,
 * it frees and overwrites the previous entry.
 * @param guild
 * @return int
 */
int disco_cache_set_guild(struct discord_guild *guild);

/**
 * @brief Retreives a guild by ID in the cache.
 *
 * @param id
 * @return struct discord_guild* Pointer to the guild struct.
 * NULL if the guild is not in the cache.
 */
struct discord_guild *disco_cache_get_guild(int64_t id);

/**
 * @brief Removes a guild from the cache and
 * deallocates the guild from memory
 *
 * @param id ID of the guild to remove
 */
void disco_cache_delete_guild(int64_t id);

#endif
