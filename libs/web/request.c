#include "request.h"
#include "../../config.h"
#include "../utils/disco_logging.h"
#include "../utils/timer.h"
#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static char *DISCORD_REQUEST_URL = "https://discord.com/api";

/**
 * @brief Callback function to write receiving data into a memory buffer
 *
 * @param data Pointer to receiving memory
 * @param s Size (should always be 1)
 * @param l Size of receving memory
 * @param userp Pointer to memory chunk
 * @return size_t Amount written
 */
static size_t write_data(void *data, size_t s, size_t l, void *userp) {
    size_t realsize = s * l;
    struct MemoryChunk *mem = (struct MemoryChunk *)userp;

    char *p = (char *)realloc(mem->memory, mem->size + realsize + 1);

    mem->memory = p;
    memcpy(&(mem->memory[mem->size]), data, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

CURLcode request(char *uri, char **response, cJSON *content, enum Request_Type request_type, CURL *handle) {
    char *request_str = NULL;
    switch (request_type) {
    case REQUEST_GET:
        request_str = "GET";
        break;
    case REQUEST_POST:
        request_str = "POST";
        break;
    case REQUEST_PATCH:
        request_str = "PATCH";
        break;
    case REQUEST_DELETE:
        request_str = "DELETE";
        break;
    case REQUEST_PUT:
        request_str = "PUT";
        break;
    case REQUEST_UPDATE:
        request_str = "UPDATE";
        break;
    }

    struct MemoryChunk chunk;

    // the plus one is because of the 0 char
    size_t len = strnlen(DISCORD_REQUEST_URL, 64), uri_len = strnlen(uri, 64);
    char url[len + uri_len + 1];
    memcpy(url, DISCORD_REQUEST_URL, len);
    memcpy(url + len, uri, uri_len);
    url[len + uri_len] = '\0';

    d_log_normal("REQUEST '%s' URL: %s\n", request_str, url);

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, request_str);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    if (content)
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, cJSON_Print(content));

    CURLcode res;
    int sent_message = 0;
    do {
        chunk.memory = malloc(1);
        chunk.size = 0;
        res = curl_easy_perform(handle);
        *response = chunk.memory;

        // checks if we're being ratelimited, if yes it waits
        cJSON *res_json = cJSON_Parse(*response);
        cJSON *res_msg = cJSON_GetObjectItem(res_json, "message");
        if (cJSON_IsString(res_msg) && strncmp(res_msg->valuestring, "You are being rate limited.", 28) == 0) {
            cJSON *wait_ms = cJSON_GetObjectItem(res_json, "retry_after");
            if (cJSON_IsNumber(wait_ms)) {
                lwsl_notice("We are being ratelimited, waiting %d ms.", wait_ms->valueint);
                usleep((unsigned int)wait_ms->valueint * 1000u);
            }
        } else
            sent_message = 1;
        cJSON_Delete(res_json);
    } while (!sent_message);
    return res;
}

struct curl_slist *curl_setup_discord_header(CURL *handle) {
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_ENCODING, "br, gzip, deflate");
    curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "br, gzip, deflate");
    char authorizationHeader[100];
    sprintf(authorizationHeader, "Authorization: Bot %s", DISCORD_TOKEN);
    struct curl_slist *list = NULL;
    list = curl_slist_append(list, authorizationHeader);
    list = curl_slist_append(list, "Accept: application/json");
    list = curl_slist_append(list, "Content-Type: application/json");
    list = curl_slist_append(list, "User-Agent: DiscordBot (v0.0.1)");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);
    return list;
}

int request_test() {
    d_log_normal("Testing HTTP requests...\n");

    char *url = "https://www.google.com/";
    char *result;
    CURLcode res;

    // GET REQUEST TEST
    res = request(url, &result, NULL, REQUEST_GET, NULL);
    if (res != CURLE_OK) {
        d_log_err("%d: GET failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            d_log_err("Have no connection to host\n");
        return 1;
    }
    d_log_err("- GET request worked successfully\n");

    // POST REQUEST TEST
    res = request(url, &result, NULL, REQUEST_POST, NULL);
    if (res != CURLE_OK) {
        d_log_err("%d: POST failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            d_log_err("Have no connection to host\n");
        return 1;
    }
    d_log_notice("- POST request worked successfully\n");

    free(result);

    return 0;
}