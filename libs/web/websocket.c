#include "websocket.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>

static int interrupted = 0, port = 443, ssl_connection = 1;

static int websocket_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

    client_websocket_t *client = (client_websocket_t *)user;
    char cdata[len + 1];

    switch (reason) {
    case LWS_CALLBACK_PROTOCOL_INIT:
        lwsl_user("Callback protocol init");
        break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_err("CLIENT CONNECTION ERROR: %s\n", in ? (char *)in : "(null)");
        interrupted = 1;
        break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        lws_callback_on_writable(wsi);
        lwsl_user("%s: established connection, wsi = %p\n", __func__, (void *)wsi);
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        lwsl_user("Callback closed\n");
        interrupted = 1;
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        // we add a EOL at the end of the input data
        lwsl_user("RX: %s\n", (const char *)in);
        memcpy(cdata, in, len);
        cdata[len] = '\0';

        // calls the on_receive function
        if (client && client->callbacks && client->callbacks->on_receive)
            client->callbacks->on_receive(client, cdata, len);
        break;

    case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
        lwsl_notice("server initiated connection close: len = %lu, in = %s\n", (unsigned long)len, (char *)in);
        return 0;

    default:
        break;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static const struct lws_protocols protocols[] = {
    {"discord-gateway", websocket_callback, 1024, 1024, 0, NULL, 0},
    LWS_PROTOCOL_LIST_TERM};

int create_client_websocket(client_websocket_t *client, callback_receive_fn on_receive) {
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

    struct lws_client_connect_info i;
    memset(&i, 0, sizeof i);
    i.context = context;
    i.port = port;
    // i.address = "gateway.discord.gg";
    // i.path = "/?v=9&encoding=json";
    i.address = "socketsbay.com";
    i.path = "/wss/v2/2/demo/";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = ssl_connection;
    i.protocol = "lws-minimal";
    i.local_protocol_name = "lws-minimal";
    i.userdata = client;
    lwsl_notice("%s: connection %s:%d\n", __func__, i.address, i.port);

    client->wsi = lws_client_connect_via_info(&i);
    client->context = context;
    client->callbacks = (struct websocket_callbacks *)malloc(sizeof(struct websocket_callbacks));
    client->callbacks->on_receive = on_receive;

    return 0;
}

void free_client_websocket(client_websocket_t *client_websocket) {
    lws_context_destroy(client_websocket->context);
    free(client_websocket->wsi);
    free(client_websocket);
}

int websocket_send(struct lws *wsi, char *data, size_t len) {

    lwsl_user("websocket_send\n");

    unsigned char cdata[LWS_PRE + len + 1];
    memcpy(&cdata[LWS_PRE], data, len);
    cdata[LWS_PRE + len] = '\0';

    int m = lws_write(wsi, &cdata[LWS_PRE], len, LWS_WRITE_TEXT);
    return m;
}

static void sigint_handler(int sig) {
    (void)sig;
    interrupted = 1;
}

int websocket_test() {
    int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;

    signal(SIGINT, sigint_handler);
    lws_set_log_level(logs, NULL);

    client_websocket_t *client = (client_websocket_t *)malloc(sizeof(struct client_websocket));
    fprintf(stderr, "BEFORE START\n");
    create_client_websocket(client, NULL);

    while (!interrupted)
        lws_service(client->context, 0);

    lwsl_notice("%s: exiting service loop. n = %d, interrupted = %d\n", __func__, n, interrupted);

    return 0;
}