#ifndef DISCO
#define DISCO

#include <cJSON/cJSON.h>
#include <discord/application.h>
#include <discord/attachment.h>
#include <discord/channel.h>
#include <discord/component.h>
#include <discord/guild.h>
#include <discord/interaction.h>
#include <discord/message.h>
#include <discord/message_activity.h>
#include <discord/permission.h>
#include <discord/reaction.h>
#include <discord/role.h>
#include <discord/sticker.h>
#include <discord/thread.h>
#include <discord/user.h>
#include <stdbool.h>
#include <stdint.h>
#include <utils/t_pool.h>
#include <web/websocket.h>

typedef struct websocket_client websocket_client_t;
typedef struct bot_client bot_client_t;

// definitions so that the compiler stops complaining
// that they don't exist
struct discord_message;
struct discord_interaction;

/**
 * @addtogroup Client
 * The main Discord client that will be used everywhere.
 * @{
 */

typedef struct discord_event_callbacks {
    void (*on_ready)(bot_client_t *);
    void (*on_resumed)(bot_client_t *);
    void (*on_message)(bot_client_t *, struct discord_message *message);
    void (*on_message_edit)(bot_client_t *, struct discord_message *old, struct discord_message *new);
    /**
     * @brief Function called if a message event is called. The message ID and channel ID are always given.
     * If the message was sent in a guild, the guild ID is also included. If the message was in cache before being
     * deleted, the message struct is also included.
     *
     */
    void (*on_message_delete)(bot_client_t *, uint64_t message_id, uint64_t channel_id, uint64_t guild_id, struct discord_message *);
    void (*on_channel_create)(bot_client_t *, struct discord_channel *channel);
    void (*on_channel_update)(bot_client_t *, struct discord_channel *old, struct discord_channel *new);
    void (*on_channel_delete)(bot_client_t *, uint64_t channel_id, uint64_t guild_id, uint64_t parent_id, enum Discord_Channel_Type type, struct discord_channel *);
    void (*on_interaction)(bot_client_t *, struct discord_interaction *);
} discord_event_callbacks_t;

struct bot_client {
    websocket_client_t *websocket_client;
    discord_event_callbacks_t *callbacks;
    struct discord_user *user;
    t_pool_t *thread_pool;
    long heartbeat_latency;
};

/**
 * @brief Discord configs which will be set when starting the bot.
 * Set settings as 0 to use the default.
 */
struct discord_config {
    int message_cache_size; ///< Amount of messages to store in the cache (default 1000)
    int channel_cache_size; ///< Amount of channels to store in the cache (default 1000)
    int guild_cache_size;   ///< Amount of guilds to store in the cache (default 1000)
};

/**
 * @brief High level abstraction which starts the bot.
 *
 * @param callbacks A callbacks object containing the callbacks to
 * the event functions.
 * @param token Bot token as a string
 * @param config Optional additional config settings. Can be NULL to resort to the default configs.
 */
void discord_start_bot(discord_event_callbacks_t *callbacks, const char *token, struct discord_config *config);

/**
 * @brief Frees up memory for a bot instance.
 *
 * @param bot Bot instance
 */
void discord_free_bot(bot_client_t *bot);

/** @} */ // closes the client group

char *get_string_from_json(cJSON *data, const char *name);
/**
 * @brief Get a bool from json object
 *
 * @param data
 * @param name
 * @param default_ The default value incase the field doesn't exist
 * @return int
 */
bool get_bool_from_json(cJSON *data, const char *name, int default_);
/**
 * @brief Get an int from json object
 *
 * @param data
 * @param name
 * @param default_ The default value incase the field doesn't exist
 * @return bool
 */
int get_int_from_json(cJSON *data, const char *name, int default_);

typedef void *(*discord_struct_fn)(cJSON *);
int get_array_from_json(cJSON *data, const char *name, void ***array, size_t s, discord_struct_fn func);

uint64_t get_long_from_string_json(cJSON *data, const char *name, uint64_t default_);

double get_double_from_string_json(cJSON *data, const char *name, double default_);

#endif