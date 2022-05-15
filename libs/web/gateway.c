#include "gateway.h"
#include "../../config.h"
#include "../discord/event.h"
#include "../utils/cJSON.h"
#include "websocket.h"
#include <pthread.h>
#include <unistd.h>

// sequence id
static int s = -1;
// time to wait between heartbeats
static unsigned int HEARTBEAT_INTERVAL = 10000;

static void gateway_handle_identify(websocket_client_t *client) {
    lwsl_user("TX: Sending gateway identify");
    char response[256];
    sprintf(response, "{\"op\":2, \"d\":{\"token\":\"%s\",\"intents\":513, \"properties\":{\"$os\":\"linux\",\"$browser\":\"Disco-C\",\"$device\":\"Disco-C\"}}}", DISCORD_TOKEN);
    websocket_send(client->wsi, response, strlen(response));

    if (!client->heartbeat_active) {
        client->heartbeat_active = 1;
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, gateway_heartbeat_loop, (void *)client);
    }
}

static void gateway_send_heartbeat(websocket_client_t *client) {
    lwsl_user("TX: Sending heartbeat");
    char response[128];
    if (s == -1)
        sprintf(response, "{\"op\":1, \"d\":null}");
    else
        sprintf(response, "{\"op\":1, \"d\":%d}", s);
    websocket_send(client->wsi, response, strlen(response));
}

static void gateway_handle_dispatch(bot_client_t *bot_client, cJSON *json) {
    // increases the sequence ID if the new value is greater
    cJSON *local_s = cJSON_GetObjectItemCaseSensitive(json, "s");
    if (cJSON_IsNumber(local_s) && local_s->valueint > s)
        s = local_s->valueint;

    cJSON *event_type = cJSON_GetObjectItemCaseSensitive(json, "t");
    cJSON *data = cJSON_GetObjectItemCaseSensitive(json, "d");
    event_handle(bot_client, data, event_type->valuestring);

    free(data);
    free(event_type);
    free(local_s);
}

void gateway_on_receive(bot_client_t *bot_client, char *data, size_t len) {
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
            gateway_handle_dispatch(bot_client, result);
            break;

        case DISCORD_RECONNECT:
            fprintf(stderr, "RECONNECT\n");
            break;

        case DISCORD_INVALID_SESSION:
            fprintf(stderr, "INVALID SESSION\n");
            break;

        case DISCORD_HELLO:
            fprintf(stderr, "HELLO: %s\n\n", data);
            gateway_handle_identify(bot_client->websocket_client);

            // gets the heartbeat interval out of the data adjusts
            cJSON *d = cJSON_GetObjectItemCaseSensitive(result, "d");
            if (cJSON_IsObject(d)) {
                cJSON *heartbeat_interval = cJSON_GetObjectItemCaseSensitive(d, "heartbeat_interval");
                if (cJSON_IsNumber(heartbeat_interval)) {
                    HEARTBEAT_INTERVAL = (unsigned int)heartbeat_interval->valueint;
                    fprintf(stderr, "Adjusted the heartbeat interval to %d\n", HEARTBEAT_INTERVAL);
                }
            }
            break;

        case DISCORD_HEARTBEAT:
            gateway_send_heartbeat(bot_client->websocket_client);
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

void *gateway_heartbeat_loop(void *vargp) {
    websocket_client_t *client = (websocket_client_t *)vargp;
    while (client->connected && client->heartbeat_active) {
        gateway_send_heartbeat(client);
        usleep(HEARTBEAT_INTERVAL * 1000);
    }
    return NULL;
}

void gateway_event_loop(bot_client_t *bot) {
    while (bot->websocket_client->connected) {
        lws_service(bot->websocket_client->context, 500);
    }
}