#include "disco.h"
#include "../utils/cJSON.h"
#include "../web/gateway.h"
#include "../web/request.h"
#include <curl/curl.h>
#include <string.h>

void disco_start_bot(disco_event_callbacks_t *callbacks) {
    // LOG LEVEL
    int logs = LLL_USER | LLL_ERR | LLL_WARN;
    lws_set_log_level(logs, NULL);

    bot_client_t bot = {0};
    websocket_client_t client = {0};

    curl_global_init(CURL_GLOBAL_ALL);
    client.handle = curl_easy_init();
    struct curl_slist *list = curl_setup_discord_header(client.handle);

    bot.websocket_client = &client;
    bot.callbacks = callbacks;

    // creates the client
    websocket_create(&client, &gateway_on_receive);
    // connects the client to the discord websocket
    websocket_connect(&bot);
    // starts the event loop which is BLOCKING
    gateway_event_loop(&bot);

    curl_slist_free_all(list);
    curl_easy_cleanup(client.handle);
    curl_global_cleanup();
}

void disco_free_bot(bot_client_t *bot) {
    websocket_destroy_client(bot);
    if (bot->user)
        disco_destroy_user(bot->user);
    free(bot);
}

char *get_string_from_json(cJSON *data, const char *name) {
    cJSON *field = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsString(field))
        return NULL;
    return strndup(field->valuestring, 4096);
}

int get_bool_from_json(cJSON *data, const char *name, int def) {
    cJSON *field = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsBool(field))
        return def;
    if (field->valueint)
        return 1;
    return 0;
}

int get_int_from_json(cJSON *data, const char *name, int def) {
    cJSON *field = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsNumber(field))
        return def;
    return field->valueint;
}

int get_array_from_json(cJSON *data, const char *name, void ***array, size_t s, disco_struct_fn func) {
    cJSON *tmp = cJSON_GetObjectItem(data, name);
    int size = cJSON_GetArraySize(tmp);
    if (size <= 0)
        return 0;
    cJSON *cur = NULL;
    *array = (void **)malloc((size_t)size * s);
    int i = 0;
    cJSON_ArrayForEach(cur, tmp) {
        (*array)[i++] = func(cur);
    }
    return size;
}
