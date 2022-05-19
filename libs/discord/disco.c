#include "disco.h"
#include "../utils/cJSON.h"
#include "../web/gateway.h"

void disco_start_bot(disco_event_callbacks_t *callbacks) {
    bot_client_t *bot = (bot_client_t *)calloc(1, sizeof(struct bot_client));
    websocket_client_t *client = (websocket_client_t *)malloc(sizeof(struct websocket_client));

    bot->websocket_client = client;
    bot->callbacks = callbacks;

    // creates the client
    websocket_create(client, &gateway_on_receive);
    // connects the client to the discord websocket
    websocket_connect(bot);
    // starts the event loop which is BLOCKING
    gateway_event_loop(bot);

    disco_free_bot(bot);
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
    size_t len = strnlen(field->valuestring, 4096); // max length any single discord string can be
    char *str = (char *)malloc(len + 1);            // extra for '\0'
    strncpy(str, field->valuestring, len);
    str[len] = '\0'; // saftey net to make sure all strings end
    return str;
}

int get_bool_from_json(cJSON *data, const char *name) {
    cJSON *field = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsBool(field))
        return -1;
    if (field->valueint)
        return 1;
    return 0;
}

int get_int_from_json(cJSON *data, const char *name) {
    cJSON *field = cJSON_GetObjectItem(data, name);
    if (!cJSON_IsNumber(field))
        return 0;
    return field->valueint;
}

int get_array_from_json(cJSON *data, const char *name, void ***array, size_t s, disco_struct_fn func) {
    cJSON *tmp = cJSON_GetObjectItem(data, name);
    int size = cJSON_GetArraySize(tmp);
    if (size <= 0)
        return 0;
    cJSON *cur = NULL;
    *array = (void **)malloc(size * s);
    int i = 0;
    cJSON_ArrayForEach(cur, tmp) {
        (*array)[i++] = func(cur);
    }
    return size;
}
