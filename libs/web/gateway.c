#include "gateway.h"
#include <libwebsockets.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static int interrupted = 0, rx_seen = 0, test = 1;
static struct lws *client_wsi;

static int callback_receive(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_err("CLIENT CONNECTION ERROR: %s\n", in ? (char *)in : "(null)");
        client_wsi = NULL;
        break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        lwsl_user("%s: established\n", __func__);
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        lwsl_user("RX: %s\n", (const char *)in);
        unsigned char *m = (unsigned char *)malloc(9 * sizeof(unsigned char));
        unsigned char *o = "{\"op\":1}";
        memcpy(m, o, 9);
        lws_write(wsi, m, 9, LWS_WRITE_TEXT);
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        lwsl_user("Callback closed\n");
        break;

    default:
        break;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static const struct lws_protocols protocols[] = {
    {"discord-gateway",
     callback_receive,
     0, 0, 0, NULL, 0},
    LWS_PROTOCOL_LIST_TERM};

static void sigint_handler(int sig) {
    interrupted = 1;
}

int websocket_test() {
    struct lws_context_creation_info info;
    struct lws_client_connect_info i;
    struct lws_context *context;
    const char *p;
    int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;

    signal(SIGINT, sigint_handler);
    lws_set_log_level(logs, NULL);

    memset(&info, 0, sizeof info);
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;
    info.timeout_secs = 10;
    info.connect_timeout_secs = 30;

    info.fd_limit_per_thread = 1 + 1 + 1;

    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 0;
    }

    memset(&i, 0, sizeof i);
    i.context = context;
    i.port = 443;
    i.address = "gateway.discord.gg";
    i.path = "/?v=9&encoding=json";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = LCCSCF_USE_SSL;
    i.protocol = protocols[0].name;
    i.pwsi = &client_wsi;

    lws_client_connect_via_info(&i);

    while (n >= 0 && client_wsi && !interrupted) {
        n = lws_service(context, 0);
    }

    lws_context_destroy(context);
    lwsl_user("Completed %s\n", rx_seen > 10 ? "OK" : "Failed");

    return rx_seen <= 10;
}