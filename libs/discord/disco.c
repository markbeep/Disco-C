#include "disco.h"
#include "../web/gateway.h"

void disco_start_bot(disco_callbacks_t *callbacks) {
    bot_client_t *bot = (bot_client_t *)malloc(sizeof(struct bot_client));
    websocket_client_t *client = (websocket_client_t *)malloc(sizeof(struct websocket_client));

    bot->websocket_client = client;
    bot->callbacks = callbacks;

    // creates the client
    websocket_create(client, &gateway_on_receive);
    // connects the client to the discord websocket
    websocket_connect(bot);
    // starts the event loop which is BLOCKING
    gateway_event_loop(bot);

    disco_free_bot(bot);
}

void disco_free_bot(bot_client_t *bot) {
    websocket_close(bot);
    free_websocket_client(bot);
    free(bot);
}