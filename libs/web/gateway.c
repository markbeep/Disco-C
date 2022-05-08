#include "gateway.h"
#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static int nclients = 1;
unsigned char msg[LWS_PRE + 128];
static int message_delay = 500000;
static int connection_delay = 100000;
static struct lws_context *context;
static const char *pro = "lws-minimal";
static int interrupted = 0, port = 443, ssl_connection = 1;

static int connect_client() {
    struct lws_client_connect_info i;

    memset(&i, 0, sizeof i);

    i.context = context;
    i.port = port;
    i.address = "gateway.discord.gg";
    i.path = "/?v=9&encoding=json";
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

    case LWS_CALLBACK_CLIENT_WRITEABLE:
        lwsl_user("Callback Client Writeable: %s\n", msg);
        m = lws_write(wsi, msg + LWS_PRE, 128, LWS_WRITE_TEXT);
        if (m < 128) {
            lwsl_err("sending message failed: %d < %d\n", m, n);
            return -1;
        }

        short r;
        if (lws_get_random(lws_get_context(wsi), &r, 2) == 2) {
            n = message_delay * 10000 * (r % 100);
            lwsl_debug("set timer on %d usecs\n", n);
            lws_set_timer_usecs(wsi, n);
        }
        break;

    case LWS_CALLBACK_TIMER:
        lws_callback_on_writable(wsi);
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        lwsl_user("RX: %s\n", (const char *)in);
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

    // memcpy(msg, "{\"op\":1,\"d\":null}", 18);
    memcpy(msg, "test msg", 9);
    // memset(msg, 'x', sizeof msg);
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