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
    // if the client is active
    int active;
    // if a heartbeat thread has already been started
    int heartbeat_active;
    // if the connection should be reset
    int reconnect;
    pthread_t heartbeat_thread;
} websocket_client_t;

/**
 * @brief Sends data through the websocket connection
 *
 * @param wsi Websocket connection
 * @param data Data to send
 * @param len Length of data to send
 * @return int
 */
int websocket_send(struct lws *wsi, char *data, size_t len);

/**
 * @brief Sets up the websocket client
 *
 * @param client Pointer to a websocket client
 * @param on_receive Callback function for the receiving websocket packages
 * @return int
 */
int websocket_create(websocket_client_t *client, callback_receive_fn on_receive);

/**
 * @brief Connects to the Discord websocket
 *
 * @param bot_client Bot client instance
 * @return int
 */
int websocket_connect(bot_client_t *bot_client);

/**
 * @brief Closes the websocket connection and sends a
 * normal close reason to the websocket server reason.
 *
 * @param bot_client
 */
void websocket_close(bot_client_t *bot_client);

/**
 * @brief Reconnects to the websocket
 *
 * @param bot_client
 */
void websocket_reconnect(bot_client_t *bot_client);

/**
 * @brief Frees up all memory allocated with the bot_client struct
 *
 * @param bot_client
 */
void websocket_destroy_client(websocket_client_t *client);

int websocket_test();

#endif
