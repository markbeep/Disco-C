#include "request.h"
#include "../../config.h"
#include <stdio.h>
#include <stdlib.h>

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

CURLcode request(char *uri, char **response, cJSON *content, enum Request_Type request_type) {
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

    curl_global_init(CURL_GLOBAL_ALL);

    CURL *handle = curl_easy_init();

    struct MemoryChunk chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    // the plus one is because of the 0 char
    size_t len = strnlen(DISCORD_REQUEST_URL, 64), uri_len = strnlen(uri, 64);
    char url[len + uri_len + 1];
    memcpy(url, DISCORD_REQUEST_URL, len);
    memcpy(url + len, uri, uri_len);
    url[len + uri_len] = '\0';

    fprintf(stderr, "REQUEST '%s' URL: %s\n", request_str, url);

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, request_str);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    if (content)
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, cJSON_Print(content));

    char authorizationHeader[100];
    sprintf(authorizationHeader, "Authorization: Bot %s", DISCORD_TOKEN);
    struct curl_slist *list = NULL;
    list = curl_slist_append(list, authorizationHeader);
    list = curl_slist_append(list, "Accept: application/json");
    list = curl_slist_append(list, "Content-Type: application/json");
    list = curl_slist_append(list, "User-Agent: DiscordBot (null, v0.0.1)");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);

    CURLcode res = curl_easy_perform(handle);
    *response = chunk.memory;

    curl_slist_free_all(list);
    curl_easy_cleanup(handle);
    curl_global_cleanup();
    return res;
}

int request_test() {
    fprintf(stderr, "Testing HTTP requests...\n");

    char *url = "https://www.google.com/";
    char *result;
    CURLcode res;

    // GET REQUEST TEST
    res = request(url, &result, NULL, REQUEST_GET);
    if (res != CURLE_OK) {
        fprintf(stderr, "%d: GET failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            fprintf(stderr, "Have no connection to host\n");
        return 1;
    }
    fprintf(stderr, "- GET request worked successfully\n");

    // POST REQUEST TEST
    res = request(url, &result, NULL, REQUEST_POST);
    if (res != CURLE_OK) {
        fprintf(stderr, "%d: POST failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            fprintf(stderr, "Have no connection to host\n");
        return 1;
    }
    fprintf(stderr, "- POST request worked successfully\n");

    free(result);

    return 0;
}