#ifndef DISCO
#define DISCO

#include "../web/websocket.h"
#include "structures/user.h"

typedef struct websocket_client websocket_client_t;
typedef struct bot_client bot_client_t;

typedef void (*disco_callback_fn)(struct bot_client *, int, char **);

typedef struct disco_event_callbacks {
    disco_callback_fn on_ready;
    disco_callback_fn on_message;
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

#endif