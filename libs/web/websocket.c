#include "websocket.h"
#include <curl/curl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static int port = 443, ssl_connection = 1;

static int websocket_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

    bot_client_t *bot_client = (bot_client_t *)user;
    websocket_client_t *client = NULL;
    if (bot_client)
        client = bot_client->websocket_client;

    switch (reason) {
    case LWS_CALLBACK_PROTOCOL_INIT:
        lwsl_user("Callback protocol init");
        break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_err("CLIENT CONNECTION ERROR: %s\n", in ? (char *)in : "(null)");
        websocket_close(bot_client);
        return -1;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        lwsl_user("%s: established connection, wsi = %p\n", __func__, (void *)wsi);
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        lwsl_user("Callback closed, in = %s\n", in ? (char *)in : "(null)");
        websocket_close(bot_client);
        return -1;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        // we add a EOL at the end of the input data
        if (!client) {
            lwsl_err("Client struct is unspecified in CLIENT RECEIVE callback");
            break;
        }
        client->content = (char *)realloc(client->content, client->size + len + 1);
        memcpy(client->content + client->size, in, len);
        client->content[client->size + len] = '\0';
        client->size += len;

        // calls the on_receive function
        if (lws_is_final_fragment(wsi)) {
            if (client && client->callbacks && client->callbacks->on_receive)
                client->callbacks->on_receive(bot_client, client->content, client->size);
            client->size = 0;
        }
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
        lwsl_notice("server initiated connection close: len = %lu, in = %u\n", (unsigned long)len, (*(uint16_t *)in));
        break;

    default:
        break;
    }

    return 0;
}

static const struct lws_protocols protocols[] = {
    {"discord-gateway", websocket_callback, 0, 0, 0, NULL, 0},
    LWS_PROTOCOL_LIST_TERM};

int websocket_create(websocket_client_t *client, callback_receive_fn on_receive) {
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN; // we run client side
    info.protocols = protocols;
    info.fd_limit_per_thread = (unsigned int)(1 + 1 + 1);

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }
    client->context = context;
    client->connected = 0;
    client->heartbeat_active = 0;
    client->callbacks = (struct websocket_callbacks *)malloc(sizeof(struct websocket_callbacks));
    client->callbacks->on_receive = on_receive;

    return 0;
}

int websocket_connect(bot_client_t *bot_client) {
    struct lws_client_connect_info i;
    memset(&i, 0, sizeof i);
    i.context = bot_client->websocket_client->context;
    i.port = port;
    i.address = "gateway.discord.gg";
    i.path = "/?v=9&encoding=json";
    // i.address = "socketsbay.com";
    // i.path = "/wss/v2/2/demo/";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = ssl_connection;
    i.protocol = "lws-minimal";
    i.local_protocol_name = "lws-minimal";
    i.userdata = bot_client;
    lwsl_notice("%s: connection %s:%d\n", __func__, i.address, i.port);

    bot_client->websocket_client->wsi = lws_client_connect_via_info(&i);
    bot_client->websocket_client->content = (char *)malloc(1);
    bot_client->websocket_client->size = 0;
    bot_client->websocket_client->connected = 1;

    return 0;
}

void websocket_destroy_client(bot_client_t *bot_client) {
    lws_context_destroy(bot_client->websocket_client->context);
    free(bot_client->websocket_client->callbacks);
    free(bot_client->websocket_client->wsi);
    free(bot_client->websocket_client);
}

int websocket_send(struct lws *wsi, char *data, size_t len) {

    lwsl_user("websocket_send operation\n");

    unsigned char cdata[LWS_PRE + len + 1];
    memcpy(&cdata[LWS_PRE], data, len);
    cdata[LWS_PRE + len] = '\0';

    int m = lws_write(wsi, &cdata[LWS_PRE], len, LWS_WRITE_TEXT);
    return m;
}

void websocket_reconnect(bot_client_t *bot_client) {
    websocket_close(bot_client);
    websocket_connect(bot_client);
}

void websocket_close(bot_client_t *bot_client) {
    websocket_client_t *client = bot_client->websocket_client;
    lws_set_timeout(client->wsi, PENDING_TIMEOUT_USER_REASON_BASE, LWS_TO_KILL_SYNC);
    lws_cancel_service(client->context);
    client->connected = 0;
    client->heartbeat_active = 0;
}

static void sigint_handler(int sig) {
    (void)sig;
}

int websocket_test() {
    int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;

    signal(SIGINT, sigint_handler);
    lws_set_log_level(logs, NULL);

    websocket_client_t *client = (websocket_client_t *)malloc(sizeof(struct websocket_client));
    websocket_create(client, NULL);

    while (client->connected)
        lws_service(client->context, 0);

    lwsl_notice("%s: exiting service loop. n = %d, connected = %d\n", __func__, n, client->connected);

    return 0;
}