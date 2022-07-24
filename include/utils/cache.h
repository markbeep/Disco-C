#ifndef DISCO_CACHE
#define DISCO_CACHE

#include <discord/channel.h>
#include <discord/guild.h>
#include <discord/message.h>
#include <hashmap.h/hashmap.h>

enum Disco_Cache_Type { DISCO_CHANNEL_CACHE,
                        DISCO_MESSAGE_CACHE,
                        DISCO_GUILD_CACHE };

/**
 * @brief Function that will be called if the max cache size is
 * reached.
 *
 * This allows the user to choose what do with the structure however they want.
 * The user can choose to simply destroy the structure or they can also add the
 * structure back into the cache based on a condition (Note this can result in an
 * infinite loop if one chooses to add the element back in the queue resulting in
 * a new element being removed).
 *
 * @param type Type of structure
 * @param structure Pointer to the actual structure
 */
typedef void (*cache_destroy_callback_fn)(enum Disco_Cache_Type type, void *structure);

/**
 * @brief Initializes all the caches.
 *
 * Should be called once when starting up the bot.
 * Once the max cache size is reached, the oldest messages/channels/guilds will
 * start to be cleaned up. Any time an object is edited on Discord, its "TTL" will be
 * renewed in the cache.
 *
 * The minimum cache size is 2, because edits require both the old and new object
 * in the cache. Any number lower than 2 will result in the cache being set to 2.
 *
 * @param message_cache_size Maximum amount of messages the cache can hold (> 1)
 * @param channel_cache_size Maximum amount of channels the cache can hold (> 1)
 * @param guild_cache_size Maximum amount of guilds the cache can hold (> 1)
 * @param callback Callback that is called whenever a structure is taken out of the cache.
 * It is up to the user if the structure should be freed or what happens to it. Keep it NULL to
 * have the structures be freed up.
 * @return int 0 if succeeded, 1 otherwise
 */
int discord_cache_init(int message_cache_size, int channel_cache_size, int guild_cache_size, cache_destroy_callback_fn callback);

/**
 * @brief Cleans up the cache and frees all the structs it contains.
 *
 */
void discord_cache_destroy(void);

/**
 * @brief Adds a message to the cache by ID. If it already exists in cache,
 * it frees and overwrites the previous entry.
 * @param message
 * @return int
 */
int discord_cache_set_message(struct discord_message *message);

/**
 * @brief Retrieves a message by ID in the cache.
 *
 * @param id
 * @return struct discord_message* Pointer to the message struct.
 * NULL if the message is not in the cache.
 */
struct discord_message *discord_cache_get_message(uint64_t id);

/**
 * @brief Removes a message from the cache and
 * deallocates the message from memory
 *
 * @param id ID of the message to remove
 */
void discord_cache_delete_message(uint64_t id);

/**
 * @brief Adds a channel to the cache by ID. If it already exists in cache,
 * it frees and overwrites the previous entry.
 * @param channel
 * @return int
 */
int discord_cache_set_channel(struct discord_channel *channel);

/**
 * @brief Retrieves a channel by ID in the cache.
 *
 * @param id
 * @return struct discord_channel* Pointer to the channel struct.
 * NULL if the channel is not in the cache.
 */
struct discord_channel *discord_cache_get_channel(uint64_t id);

/**
 * @brief Removes a channel from the cache and
 * deallocates the channel from memory
 *
 * @param id ID of the channel to remove
 */
void discord_cache_delete_channel(uint64_t id);

/**
 * @brief Adds a guild to the cache by ID. If it already exists in cache,
 * it frees and overwrites the previous entry.
 * @param guild
 * @return int
 */
int discord_cache_set_guild(struct discord_guild *guild);

/**
 * @brief Retrieves a guild by ID in the cache.
 *
 * @param id
 * @return struct discord_guild* Pointer to the guild struct.
 * NULL if the guild is not in the cache.
 */
struct discord_guild *discord_cache_get_guild(uint64_t id);

/**
 * @brief Removes a guild from the cache and
 * deallocates the guild from memory
 *
 * @param id ID of the guild to remove
 */
void discord_cache_delete_guild(uint64_t id);

#endif
