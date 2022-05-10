#include <curl/curl.h>
#include <libwebsockets.h>

#ifndef WEBSOCKET
#define WEBSOCKET

typedef struct client_websocket client_websocket_t;

typedef void (*callback_receive_fn)(struct client_websocket *, char *, size_t);

struct websocket_callbacks {
    callback_receive_fn on_receive;
};

typedef struct client_websocket {
    struct websocket_callbacks *callbacks;
    struct lws_context *context;
    struct lws *wsi;
    char *content;
    size_t size;
} client_websocket_t;

int websocket_send(struct lws *wsi, char *data, size_t len);
int websocket_close(client_websocket_t *client);
int websocket_create(client_websocket_t *client, callback_receive_fn on_receive);
int websocket_connect(client_websocket_t *client);
int websocket_test();

#endif