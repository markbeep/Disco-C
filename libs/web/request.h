#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERBOSE 0

struct MemoryChunk {
    char *memory;
    size_t size;
};

/**
 * @brief Performs a HTTP GET request to the specified URL
 *
 * @param url URL to request from
 * @param response Pointer to a pointer that will be
 *                 modified to point to point to the data
 * @param handle   Curl handle
 * @return CURLcode
 */
CURLcode request_get(char *url, char **response, CURL *handle);

/**
 * @brief Performs a HTTP POST request to the specified URL
 *
 * @param url URL to request to
 * @param response Pointer to a pointer that will be
 *                 modified to point to point to the data
 * @param handle   Curl handle
 * @return CURLcode
 */
CURLcode request_post(char *url, char **response, CURL *handle);