#ifndef DISCO
#define DISCO

#include "../utils/cJSON/cJSON.h"
#include "../utils/t_pool.h"
#include "../web/websocket.h"
#include "structures/application.h"
#include "structures/attachment.h"
#include "structures/channel.h"
#include "structures/component.h"
#include "structures/guild.h"
#include "structures/message.h"
#include "structures/message_activity.h"
#include "structures/permission.h"
#include "structures/reaction.h"
#include "structures/role.h"
#include "structures/sticker.h"
#include "structures/thread.h"
#include "structures/user.h"

typedef struct websocket_client websocket_client_t;
typedef struct bot_client bot_client_t;
struct discord_message;

typedef struct disco_event_callbacks {
    void (*on_ready)(struct bot_client *);
    void (*on_resumed)(struct bot_client *);
    void (*on_message)(struct bot_client *, struct discord_message *message);
    void (*on_message_edit)(struct bot_client *, struct discord_message *old, struct discord_message *new);
    /**
     * @brief Function called if a message event is called. The message ID and channel ID are always given.
     * If the message was sent in a guild, the guild ID is also included. If the message was in cache before being
     * deleted, the message struct is also included.
     *
     */
    void (*on_message_delete)(struct bot_client *, char *message_id, char *channel_id, char *guild_id, struct discord_message *);
} disco_event_callbacks_t;

typedef struct bot_client {
    websocket_client_t *websocket_client;
    disco_event_callbacks_t *callbacks;
    struct discord_user *user;
    t_pool_t *thread_pool;
    long heartbeat_latency;
} bot_client_t;

/**
 * @brief High level abstraction which starts the bot.
 *
 * @param callbacks A callbacks object containing the callbacks to
 * the event functions.
 */
void disco_start_bot(disco_event_callbacks_t *callbacks);

/**
 * @brief Frees up memory for a bot instance.
 *
 * @param bot Bot instance
 */
void disco_free_bot(bot_client_t *bot);

char *get_string_from_json(cJSON *data, const char *name);
/**
 * @brief Get a bool from json object
 *
 * @param data
 * @param name
 * @param def The default value incase the field doesn't exist
 * @return int
 */
int get_bool_from_json(cJSON *data, const char *name, int def);
/**
 * @brief Get an int from json object
 *
 * @param data
 * @param name
 * @param def The default value incase the field doesn't exist
 * @return int
 */
int get_int_from_json(cJSON *data, const char *name, int def);

typedef void *(*disco_struct_fn)(cJSON *);
int get_array_from_json(cJSON *data, const char *name, void ***array, size_t s, disco_struct_fn func);

#endif
