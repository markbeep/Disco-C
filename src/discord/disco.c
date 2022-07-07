#include "disco.h"
#include "../utils/cache.h"
#include "../utils/disco_logging.h"
#include "../web/gateway.h"
#include "../web/request.h"
#include <cJSON/cJSON.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

void disco_start_bot(disco_event_callbacks_t *callbacks) {
    // LOG LEVEL
    int logs = LLL_USER | LLL_ERR | LLL_WARN;
    lws_set_log_level(logs, NULL);

    bot_client_t bot = {0};
    websocket_client_t client = {0};

    curl_global_init(CURL_GLOBAL_ALL);

    bot.websocket_client = &client;
    bot.callbacks = callbacks;
    bot.thread_pool = t_pool_init(t_process_count());

    // inits the cache
    if (0 != disco_cache_init(1000, 1000, 1000, NULL)) {
        d_log_err("Cache initialization failed\n");
        exit(1);
    }

    // creates the client
    websocket_create(&client, &gateway_on_receive);
    // connects the client to the discord websocket
    websocket_connect(&bot);

    // starts the event loop which is BLOCKING
    gateway_event_loop(&bot);

    // cleanup
    // closes and waits for all threads
    t_pool_destroy(bot.thread_pool);
    websocket_destroy_client(&client);
    // if there is a bot user, destroy it
    if (bot.user)
        disco_destroy_user(bot.user);
    curl_global_cleanup();
    disco_cache_destroy();
}

char *get_string_from_json(cJSON *data, const char *name) {
    cJSON *field = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsString(field))
        return NULL;
    return strndup(field->valuestring, 4096);
}

bool get_bool_from_json(cJSON *data, const char *name, int default_) {
    cJSON *field = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsBool(field))
        return default_;
    if (field->valueint)
        return true;
    return false;
}

int get_int_from_json(cJSON *data, const char *name, int default_) {
    cJSON *field = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsNumber(field))
        return default_;
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

uint64_t get_long_from_string_json(cJSON *data, const char *name, uint64_t default_) {
    cJSON *tmp = cJSON_GetObjectItem(data, name);
    if (cJSON_IsNumber(tmp))
        return (uint64_t)tmp->valueint;
    if (!cJSON_IsString(tmp))
        return default_;
    return (uint64_t)strtoll(tmp->valuestring, NULL, 10);
}

double get_double_from_string_json(cJSON *data, const char *name, double default_) {
    cJSON *tmp = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsNumber(tmp))
        return default_;
    return tmp->valuedouble;
}
