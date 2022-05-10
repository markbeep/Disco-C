#include "gateway.h"
#include "../../config.h"
#include "../discord/event.h"
#include "../utils/cJSON.h"
#include "websocket.h"

static int s = 0; // sequence id

static void gateway_handle_identify(client_websocket_t *client) {
    lwsl_user("TX: Sending gateway identify");
    char response[256];
    sprintf(response, "{\"op\":2, \"d\":{\"token\":\"%s\",\"intents\":513, \"properties\":{\"$os\":\"linux\",\"$browser\":\"Disco-C\",\"$device\":\"Disco-C\"}}}", DISCORD_TOKEN);
    websocket_send(client->wsi, response, strlen(response));
}

static void gateway_handle_dispatch(client_websocket_t *client, cJSON *json) {
    // increases the sequence ID if the new value is greater
    cJSON *local_s = cJSON_GetObjectItemCaseSensitive(json, "s");
    if (cJSON_IsNumber(local_s) && local_s->valueint > s)
        s = local_s->valueint;

    cJSON *event_type = cJSON_GetObjectItemCaseSensitive(json, "t");
    cJSON *data = cJSON_GetObjectItemCaseSensitive(json, "d");
    event_handle(client, data, event_type->valuestring);

    free(data);
    free(event_type);
    free(local_s);
}

static void on_receive(client_websocket_t *client, char *data, size_t len) {
    (void)len;

    cJSON *result = cJSON_Parse(data);
    if (!result) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            fprintf(stderr, "Error or no JSON format at: %s\n", error_ptr);
        }
        goto json_cleanup;
    }
    cJSON *op = cJSON_GetObjectItemCaseSensitive(result, "op");
    if (cJSON_IsNumber(op)) {
        fprintf(stderr, "Received opcode: %d ", op->valueint);

        switch (op->valueint) {
        case DISCORD_DISPATCH:
            fprintf(stderr, "DISPATCH: %s\n\n", data);
            gateway_handle_dispatch(client, result);
            break;

        case DISCORD_RECONNECT:
            fprintf(stderr, "RECONNECT\n");
            break;

        case DISCORD_INVALID_SESSION:
            fprintf(stderr, "INVALID SESSION\n");
            break;

        case DISCORD_HELLO:
            fprintf(stderr, "HELLO\n");
            gateway_handle_identify(client);
            break;

        case DISCORD_HEARTBEAT_ACK:
            fprintf(stderr, "HEARTBEAT ACK\n");
            break;

        default:
            break;
        }
    } else {
        fprintf(stderr, "JSON missing opcode\n");
    }

    free(op);

json_cleanup:
    free(result);
}

void gateway_heartbeat_loop(client_websocket_t *client) {
    (void)client;
}

void gateway_event_loop(client_websocket_t *client) {
    while (client->connected) {
        lws_service(client->context, 500);
    }
}

int gateway_test() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    client_websocket_t *client = (client_websocket_t *)malloc(sizeof(struct client_websocket));
    websocket_create(client, &on_receive);
    websocket_connect(client);

    gateway_event_loop(client);

    return 0;
}