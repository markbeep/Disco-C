#ifndef DISCO
#define DISCO

#include "../utils/cJSON.h"
#include "../web/websocket.h"
#include "structures/message.h"
#include "structures/user.h"

typedef struct websocket_client websocket_client_t;
typedef struct bot_client bot_client_t;

typedef struct disco_event_callbacks {
    void (*on_ready)(struct bot_client *);
    void (*on_message)(struct bot_client *, struct discord_message *message);
} disco_event_callbacks_t;

typedef struct bot_client {
    websocket_client_t *websocket_client;
    disco_event_callbacks_t *callbacks;
    struct discord_user *user;
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
int get_bool_from_json(cJSON *data, const char *name);
int get_int_from_json(cJSON *data, const char *name);

typedef void *(*disco_struct_fn)(cJSON *);
int get_array_from_json(cJSON *data, const char *name, void ***array, size_t s, disco_struct_fn func);

#endif