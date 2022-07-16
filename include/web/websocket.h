#ifndef DISCO_WEBSOCKET
#define DISCO_WEBSOCKET

#include <curl/curl.h>
#include <discord/disco.h>
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
    // if the client is connected to the Discord websocket
    int connected;
    // if the client was able to successfully login with the Discord token
    int success_login;
    // if the client is ready to be used
    int active;
    // if a heartbeat thread has already been started
    int heartbeat_active;
    // if the connection should be reset
    int reconnect;
    // if the websocket should be exited
    int exit;
    pthread_t heartbeat_thread;
    const char *token;
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
 * @brief Sends the initial handshake to connect to the Discord websocket
 * The client is only marked as "connected" once an answer from Discord
 * is received.
 *
 * @param bot_client Bot client instance
 * @return int
 */
int websocket_connect(bot_client_t *bot_client);

/**
 * @brief Exits the websocket connection completely
 *
 * @param bot_client
 */
void websocket_exit(bot_client_t *bot_client);

/**
 * @brief Requests the websocket to close
 * If the exit flag is not set this will result in a reconnect.
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
 * @param client Websocket client that should be destroyed
 */
void websocket_destroy_client(websocket_client_t *client);

int websocket_test(void);

#endif
