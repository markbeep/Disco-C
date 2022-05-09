#include "gateway.h"
#include "../../config.h"
#include "../utils/cJSON.h"
#include "websocket.h"

static void on_receive(client_websocket_t *client_websocket, char *data, size_t len) {
    (void)client_websocket;
    (void)len;
    lwsl_user("%s: %s\n", __func__, data);
    cJSON *result = cJSON_Parse(data);
    if (!result) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            fprintf(stderr, "Error or no JSON format: %s\n", error_ptr);
        }
        goto json_cleanup;
    }
    cJSON *op = cJSON_GetObjectItemCaseSensitive(result, "op");
    if (cJSON_IsNumber(op)) {
        fprintf(stderr, "Received opcode: %d\n", op->valueint);

        } else {
        fprintf(stderr, "JSON missing opcode\n");
    }

json_cleanup:
    free(result);
}

void gateway_heartbeat_loop(client_websocket_t *client) {
    (void)client;
}

void gateway_event_loop(client_websocket_t *client) {
    while (1) {
        lws_service(client->context, 0);
    }
}

int gateway_test() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    client_websocket_t *client = (client_websocket_t *)malloc(sizeof(struct client_websocket));
    create_client_websocket(client, &on_receive);

    gateway_event_loop(client);

    return 0;
}