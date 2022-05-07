#include "request.h"

/**
 * @brief Callback function to write receiving data into a memory buffer
 *
 * @param data Pointer to receiving memory
 * @param s Size (should always be 1)
 * @param l Size of receving memory
 * @param userp Pointer to memory chunk
 * @return size_t Amount written
 */
size_t write_data(void *data, size_t s, size_t l, void *userp) {
    size_t realsize = s * l;
    struct MemoryChunk *mem = (struct MemoryChunk *)userp;

    char *p = (char *)realloc(mem->memory, mem->size + realsize + 1);

    mem->memory = p;
    memcpy(&(mem->memory[mem->size]), data, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

CURLcode request_get(char *url, char **response, CURL *handle) {
    if (handle == NULL)
        handle = curl_easy_init();

    struct MemoryChunk chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    if (VERBOSE) {
        curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
    }

    CURLcode res = curl_easy_perform(handle);

    *response = chunk.memory;
    return res;
}

CURLcode request_post(char *url, char **response, CURL *handle) {
    if (handle == NULL)
        handle = curl_easy_init();

    curl_mime *multipart = curl_mime_init(handle);
    curl_mimepart *part = curl_mime_addpart(multipart);
    curl_mime_name(part, "content");
    curl_mime_data(part, "this is a message", CURL_ZERO_TERMINATED);

    struct MemoryChunk chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(handle, CURLOPT_MIMEPOST, multipart);

    CURLcode res = curl_easy_perform(handle);
    curl_mime_free(multipart);

    *response = chunk.memory;
    return res;
}