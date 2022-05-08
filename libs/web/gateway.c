#include "gateway.h"
#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static int nclients = 1;
static int message_delay = 500000;
static int connection_delay = 100000;
static struct lws_context *context;
static const char *pro = "lws-minimal";
static int interrupted = 0, port = 443, ssl_connection = 1;

int websocket_send(struct lws *wsi, char *data, size_t len) {

    lwsl_user("websocket_send\n");

    unsigned char cdata[LWS_PRE + len + 1];
    memcpy(&cdata[LWS_PRE], data, len);
    cdata[LWS_PRE + len] = '\0';

    int m = lws_write(wsi, &cdata[LWS_PRE], len, LWS_WRITE_TEXT);
    return m;
}

static int connect_client() {
    struct lws_client_connect_info i;

    memset(&i, 0, sizeof i);

    i.context = context;
    i.port = port;
    i.address = "gateway.discord.gg";
    i.path = "/?v=9&encoding=json";
    // i.address = "socketsbay.com";
    // i.path = "/wss/v2/2/demo/";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = ssl_connection;
    i.protocol = pro;
    i.local_protocol_name = pro;
    lwsl_notice("%s: connection %s:%d\n", __func__, i.address, i.port);
    if (!lws_client_connect_via_info(&i))
        return 1;

    return 0;
}

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    int m = 0, n = 0;

    switch (reason) {
    case LWS_CALLBACK_PROTOCOL_INIT:
        lwsl_user("Callback protocol init");
        for (n = 0; n < nclients; n++)
            connect_client();
        break;

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_err("CLIENT CONNECTION ERROR: %s\n", in ? (char *)in : "(null)");
        if (--nclients == 0)
            interrupted = 1;
        break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        lws_callback_on_writable(wsi);
        lwsl_user("%s: established connection, wsi = %p\n", __func__, (void *)wsi);
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        lwsl_user("Callback closed\n");
        if (--nclients == 0)
            interrupted = 1;
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        lwsl_user("RX: %s\n", (const char *)in);
        websocket_send(wsi, "{\"op\":1,\"d\":null}", 18);
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
    {"discord-gateway", callback, 4096, 4096, 0, NULL, 0},
    LWS_PROTOCOL_LIST_TERM};

static void sigint_handler(int sig) {
    (void)sig;
    interrupted = 1;
}

int websocket_test() {
    (void)connection_delay;

    struct lws_context_creation_info info;
    int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;

    signal(SIGINT, sigint_handler);
    lws_set_log_level(logs, NULL);

    memset(&info, 0, sizeof info);
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN; // we run client side
    info.protocols = protocols;

    info.fd_limit_per_thread = (unsigned int)(1 + nclients + 1);

    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }

    while (n >= 0 && !interrupted)
        n = lws_service(context, 0);

    lwsl_notice("%s: exiting service loop. n = %d, interrupted = %d\n", __func__, n, interrupted);

    lws_context_destroy(context);

    return 0;
}