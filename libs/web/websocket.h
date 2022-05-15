#ifndef WEBSOCKET
#define WEBSOCKET

#include "../discord/disco.h"
#include <curl/curl.h>
#include <libwebsockets.h>

typedef struct bot_client bot_client_t;

typedef void (*callback_receive_fn)(struct bot_client *, char *, size_t);

struct websocket_callbacks {
    callback_receive_fn on_receive;
};

typedef struct websocket_client {
    struct websocket_callbacks *callbacks;
    struct lws_context *context;
    struct lws *wsi;
    // messages can come in chunks. this allows us to
    // put together the message content from multiple chunks
    char *content;
    // the size of the above content
    size_t size;
    // if the client is connected to the discord websocket
    int connected;
    // if a heartbeat thread has already been started
    int heartbeat_active;
} websocket_client_t;

int websocket_send(struct lws *wsi, char *data, size_t len);
int websocket_create(websocket_client_t *client, callback_receive_fn on_receive);
int websocket_connect(bot_client_t *bot_client);
void websocket_close(bot_client_t *bot_client);
void websocket_reconnect(bot_client_t *bot_client);
void free_websocket_client(bot_client_t *bot_client);
int websocket_test();

#endif