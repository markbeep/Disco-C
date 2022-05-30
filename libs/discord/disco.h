#ifndef DISCO
#define DISCO

#include "../utils/cJSON.h"
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
    void (*on_message_delete)(struct bot_client *, struct discord_message *);
    void (*on_error)(struct bot_client *, int code, char *message);
    void (*on_guild_status)(struct bot_client *, struct discord_guild *old_guild, struct discord_guild *new_guild);
    void (*on_guild_create)(struct bot_client *, struct discord_guild *);
    void (*on_channel_create)(struct bot_client *, struct discord_channel *);
    void (*on_voice_channel_select)(struct bot_client *, char *channel_id, char *guild_id);
} disco_event_callbacks_t;

typedef struct bot_client {
    websocket_client_t *websocket_client;
    disco_event_callbacks_t *callbacks;
    struct discord_user *user;
    t_pool_t *thread_pool;
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
