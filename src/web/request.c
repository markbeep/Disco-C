#include <libwebsockets.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

long request(char *url, char **response, cJSON *content, enum Request_Type request_type, const char *token) {
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
    bool sent_message = false;
    int iterations = 0;
    long http = 0;
    cJSON *res_json, *wait_ms; // response json, rate limit timeout json field
    do {
        if (iterations >= 5) {
            if (res == 0) {
                d_log_err("CURL failed. Code: %d\n", (int)res);
            } else if (http == 502) {
                d_log_err("GATEWAY UNAVAILABLE. There was not a gateway available to process the request.\n");
            }
            break;
        }

        chunk.memory = NULL;
        chunk.size = 0;
        res = curl_easy_perform(handle);
        *response = chunk.memory;
        // gets the http code
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http);
        switch (http) {
        case 200:
        case 201:
        case 204:
        case 304:
            sent_message = true;
            break;
        case 400:
            d_log_err("BAD REQUEST. Request was improperly formatted.\n");
            sent_message = true;
            break;
        case 401:
            d_log_err("UNAUTHORIZED. The Authorization header was missing or invalid.\n");
            sent_message = true;
            break;
        case 403:
            d_log_err("FORBIDDEN. The Authorization token you passed did not have permission to the resource.\n");
            sent_message = true;
            break;
        case 404:
            d_log_err("NOT FOUND. The resource at the location specified doesn't exist.\n");
            sent_message = true;
            break;
        case 405:
            d_log_err("METHOD NOT ALLOWED. The HTTP method used is not valid for the location specified.\n");
            sent_message = true;
            break;
        case 429: // simply retry after waiting the time
            res_json = cJSON_Parse(*response);
            wait_ms = cJSON_GetObjectItem(res_json, "retry_after");
            lwsl_notice("TOO MANY REQUESTS. We are being rate limited, waiting %d ms.\n", wait_ms->valueint);
            if (cJSON_IsNumber(wait_ms)) {
                usleep((unsigned int)wait_ms->valueint * 1000u);
            }
            cJSON_Delete(res_json);
            break;
        case 0:   // if CURL fails we get 0
        case 502: // we simply retry again after a few seconds
            usleep((1 << iterations) * 1000000u);
            iterations++;
            break;
        default:
            d_log_err("Unhandled HTTP response code: %ld\n", http);
            sent_message = true;
        }
    } while (!sent_message);

    if (content_p)
        free(content_p);
    curl_slist_free_all(list);
    curl_easy_cleanup(handle);
    return http;
}

struct curl_slist *curl_setup_discord_header(CURL *handle, const char *token) {
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
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
