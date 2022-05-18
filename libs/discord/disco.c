#include "disco.h"
#include "../web/gateway.h"
#include "structures/user.h"

void disco_start_bot(disco_event_callbacks_t *callbacks) {
    bot_client_t *bot = (bot_client_t *)calloc(1, sizeof(struct bot_client));
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
    websocket_destroy_client(bot);
    if (bot->user)
        disco_destroy_user(bot->user);
    free(bot);
}