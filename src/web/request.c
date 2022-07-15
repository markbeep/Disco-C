#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/disco_logging.h>
#include <utils/timer.h>
#include <web/request.h>

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

CURLcode request(char *url, char **response, cJSON *content, enum Request_Type request_type, const char *token) {
    // we create a new handle each call because we can't use the same handle over multiple threads
    CURL *handle = curl_easy_init();
    struct curl_slist *list = curl_setup_discord_header(handle, token);

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
    default:
        break;
    }

    struct MemoryChunk chunk;

    d_log_normal("REQUEST '%s' URL: %s\n", request_str, url);

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, request_str);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    char *content_p = NULL;
    if (content) {
        content_p = cJSON_Print(content);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, content_p);
    }

    CURLcode res;
    int sent_message = 0;
    do {
        chunk.memory = NULL;
        chunk.size = 0;
        res = curl_easy_perform(handle);
        *response = chunk.memory;

        // checks if we're being ratelimited, if yes it waits
        cJSON *res_json = cJSON_Parse(*response);
        cJSON *res_msg = cJSON_GetObjectItem(res_json, "message");
        if (cJSON_IsString(res_msg) && strncmp(res_msg->valuestring, "You are being rate limited.", 28) == 0) {
            cJSON *wait_ms = cJSON_GetObjectItem(res_json, "retry_after");
            if (cJSON_IsNumber(wait_ms)) {
                lwsl_notice("We are being ratelimited, waiting %d ms.\n", wait_ms->valueint);
                usleep((unsigned int)wait_ms->valueint * 1000u);
            }
        } else
            sent_message = 1;
        cJSON_Delete(res_json);
    } while (!sent_message);

    if (content_p)
        free(content_p);
    curl_slist_free_all(list);
    curl_easy_cleanup(handle);
    return res;
}

struct curl_slist *curl_setup_discord_header(CURL *handle, const char *token) {
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_ENCODING, "br, gzip, deflate");
    curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "br, gzip, deflate");
    char authorizationHeader[100];
    sprintf(authorizationHeader, "Authorization: Bot %s", token);
    struct curl_slist *list = NULL;
    list = curl_slist_append(list, authorizationHeader);
    list = curl_slist_append(list, "Accept: application/json");
    list = curl_slist_append(list, "Content-Type: application/json");
    list = curl_slist_append(list, "User-Agent: DiscordBot (v0.0.1)");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);
    return list;
}
