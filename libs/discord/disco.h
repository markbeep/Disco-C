#ifndef DISCO
#define DISCO

#include "../web/websocket.h"

typedef struct websocket_client websocket_client_t;

typedef void (*disco_callback_fn)(int, char **);

typedef struct disco_callbacks {
    disco_callback_fn on_ready;
    disco_callback_fn on_message;
} disco_callbacks_t;

typedef struct bot_client {
    websocket_client_t *websocket_client;
    disco_callbacks_t *callbacks;
} bot_client_t;

void disco_start_bot(disco_callbacks_t *callbacks);
void disco_free_bot(bot_client_t *bot);

#endif