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
        bot_client->websocket_client->connected = 0;
        bot_client->websocket_client->active = 0;
        lwsl_err("CLIENT CONNECTION ERROR: %s\n", in ? (char *)in : "(null)");
        websocket_close(bot_client);
        return -1;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        bot_client->websocket_client->connected = 1;
        lwsl_user("%s: established connection, wsi = %p\n", __func__, (void *)wsi);
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        lwsl_user("Callback closed, in = %s\n", in ? (char *)in : "(null)");
        if (bot_client->websocket_client->reconnect) {
            websocket_connect(bot_client);
            bot_client->websocket_client->reconnect = 0;
        } else { // internet failed or the websocket was closed on purpose
            bot_client->websocket_client->active = 0;
            if (!bot_client->websocket_client->success_login)
                lwsl_err("Connection closed. Probably the Discord token is invalid.\n");
            else
                lwsl_err("Connection closed.\n");
            websocket_close(bot_client);
            bot_client->websocket_client->connected = 0;
            client->success_login = 0;
        }
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
            free(client->content);
            client->content = (char *)malloc(1);
        }
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
        lwsl_notice("server initiated connection close: len = %lu, in = %u\n", (unsigned long)len, (*(uint16_t *)in));
        break;

    default:
        lwsl_notice("other callback message\n");
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
    client->active = 1;
    client->heartbeat_active = 0;
    client->callbacks = (struct websocket_callbacks *)malloc(sizeof(struct websocket_callbacks));
    client->callbacks->on_receive = on_receive;
    client->success_login = 0;

    return 0;
}

int websocket_connect(bot_client_t *bot_client) {
    struct lws_client_connect_info i;
    memset(&i, 0, sizeof i);
    i.context = bot_client->websocket_client->context;
    i.port = port;
    i.address = "gateway.discord.gg";
    i.path = "/?v=9&encoding=json";
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
    bot_client->websocket_client->active = 1;

    return 0;
}

void websocket_destroy_client(websocket_client_t *client) {
    // closes the connection if there is one
    if (client->connected)
        lws_set_timeout(client->wsi, LWS_CLOSE_STATUS_NORMAL, LWS_TO_KILL_ASYNC);
    lws_context_destroy(client->context);
    client->active = 0;
    client->heartbeat_active = 0;
    free(client->callbacks);
    free(client->content);
    if (client->heartbeat_active)
        pthread_join(client->heartbeat_thread, NULL);
}

int websocket_send(struct lws *wsi, char *data, size_t len) {

    unsigned char cdata[LWS_PRE + len + 1];
    memcpy(&cdata[LWS_PRE], data, len);
    cdata[LWS_PRE + len] = '\0';

    int m = lws_write(wsi, &cdata[LWS_PRE], len, LWS_WRITE_TEXT);
    return m;
}

void websocket_reconnect(bot_client_t *bot_client) {
    lwsl_notice("Reconnecting");
    bot_client->websocket_client->reconnect = 1;
    websocket_close(bot_client);
}

void websocket_close(bot_client_t *bot_client) {
    websocket_client_t *client = bot_client->websocket_client;
    if (client->heartbeat_active) {
        client->heartbeat_active = 0;
        pthread_join(client->heartbeat_thread, NULL);
    }
    lws_set_timeout(client->wsi, LWS_CLOSE_STATUS_NORMAL, LWS_TO_KILL_ASYNC);
    lws_cancel_service(client->context);
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

    while (client->active)
        lws_service(client->context, 0);

    lwsl_notice("%s: exiting service loop. n = %d, connected = %d\n", __func__, n, client->active);

    return 0;
}
