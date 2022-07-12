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
 * @brief High level abstraction which starts the bot.
 *
 * @param callbacks A callbacks object containing the callbacks to
 * the event functions.
 */
void discord_start_bot(discord_event_callbacks_t *callbacks);

/**
 * @brief Frees up memory for a bot instance.
 *
 * @param bot Bot instance
 */
void discord_free_bot(bot_client_t *bot);

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