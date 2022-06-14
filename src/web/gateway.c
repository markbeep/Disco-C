#include "gateway.h"
#include "../../config.h"
#include "../discord/event.h"
#include "../utils/disco_logging.h"
#include "websocket.h"
#include <cJSON/cJSON.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

// sequence id
static int s = -1;
// time to wait between heartbeats
// gets set dynamically upon receiving the HELLO event
static unsigned int HEARTBEAT_INTERVAL = 10000;
static struct timeval last_hearbeat;

static void gateway_handle_identify(websocket_client_t *client) {
    lwsl_user("TX: Sending gateway identify\n");
    char response[256];
    sprintf(response, "{\"op\":2, \"d\":{\"token\":\"%s\",\"intents\":513, \"properties\":{\"$os\":\"linux\",\"$browser\":\"Disco-C\",\"$device\":\"Disco-C\"}}}", DISCORD_TOKEN);
    websocket_send(client->wsi, response, strnlen(response, 256));

    if (!client->heartbeat_active) {
        d_log_notice("Creating heartbeat thread\n");
        client->heartbeat_active = 1;
        pthread_create(&client->heartbeat_thread, NULL, gateway_heartbeat_loop, (void *)client);
    }
}

static void gateway_send_heartbeat(websocket_client_t *client) {
    lwsl_user("TX: Sending heartbeat\n");
    char response[128];
    if (s == -1)
        sprintf(response, "{\"op\":1, \"d\":null}");
    else
        sprintf(response, "{\"op\":1, \"d\":%d}", s);
    websocket_send(client->wsi, response, strnlen(response, 128));
    gettimeofday(&last_hearbeat, NULL);
}

static void gateway_handle_dispatch(bot_client_t *bot, cJSON *json) {
    // increases the sequence ID if the new value is greater
    cJSON *local_s = cJSON_GetObjectItemCaseSensitive(json, "s");
    if (cJSON_IsNumber(local_s) && local_s->valueint > s)
        s = local_s->valueint;

    // freed after the event is handled
    cJSON *event_type = cJSON_GetObjectItemCaseSensitive(json, "t");
    // freed once the event struct is created
    cJSON *data = cJSON_GetObjectItemCaseSensitive(json, "d");
    event_handle(bot, data, event_type->valuestring);
}

void gateway_on_receive(bot_client_t *bot, char *data, size_t len) {
    (void)len;

    cJSON *result = cJSON_Parse(data);
    if (!result) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            d_log_err("Error or no JSON format at: %s\n", error_ptr);
        }
        goto json_cleanup;
    }
    cJSON *op = cJSON_GetObjectItemCaseSensitive(result, "op");
    if (cJSON_IsNumber(op)) {
        d_log_notice("Received opcode: %d\n", op->valueint);
        switch (op->valueint) {
        case DISCORD_DISPATCH:
            bot->websocket_client->success_login = 1;
            d_log_notice("Received DISPATCH \n");
            d_log_debug("Dispatch data: %s\n", data);

            gateway_handle_dispatch(bot, result);
            break;

        case DISCORD_RECONNECT:
            d_log_notice("Received RECONNECT\n");
            break;

        case DISCORD_INVALID_SESSION:
            d_log_notice("Received INVALID SESSION\n");
            websocket_reconnect(bot);
            break;

        case DISCORD_HELLO:
            d_log_notice("Received HELLO: %s\n\n", data);
            gateway_handle_identify(bot->websocket_client);

            // gets the heartbeat interval out of the data adjusts
            cJSON *d = cJSON_GetObjectItemCaseSensitive(result, "d");
            if (cJSON_IsObject(d)) {
                cJSON *heartbeat_interval = cJSON_GetObjectItemCaseSensitive(d, "heartbeat_interval");
                if (cJSON_IsNumber(heartbeat_interval)) {
                    HEARTBEAT_INTERVAL = (unsigned int)heartbeat_interval->valueint;
                    d_log_notice("Adjusted the heartbeat interval to %d\n", HEARTBEAT_INTERVAL);
                }
            }
            break;

        case DISCORD_HEARTBEAT:
            gateway_send_heartbeat(bot->websocket_client);
            break;

        case DISCORD_HEARTBEAT_ACK:
            d_log_notice("Received HEARTBEAT ACK\n");
            struct timeval heartbeat_recv;
            gettimeofday(&heartbeat_recv, NULL);
            bot->heartbeat_latency = (heartbeat_recv.tv_sec - last_hearbeat.tv_sec) * 1000 + (heartbeat_recv.tv_usec - last_hearbeat.tv_usec) / 1000;
            break;

        default:
            break;
        }
    } else {
        d_log_err("JSON missing opcode\n");
    }

json_cleanup:
    cJSON_Delete(result);
}

void *gateway_heartbeat_loop(void *vargp) {
    websocket_client_t *client = (websocket_client_t *)vargp;
    int64_t t = HEARTBEAT_INTERVAL;
    // every 100 ms the client checks if it's time to end the loop
    useconds_t slp = 1e5; // 100ms
    while (client->active && client->heartbeat_active) {
        t += 1;
        if (t * 100 >= HEARTBEAT_INTERVAL) {
            gateway_send_heartbeat(client);
            t = 0;
        }
        usleep(slp);
    }
    d_log_debug("Heartbeat loop closing\n");
    return NULL;
}

void gateway_event_loop(bot_client_t *bot) {
    while (bot->websocket_client->active) {
        lws_service(bot->websocket_client->context, 0);
    }
}
