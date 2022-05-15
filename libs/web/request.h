
#ifndef REQUEST
#define REQUEST

#define VERBOSE 0

#include "../utils/cJSON.h"
#include <curl/curl.h>
#include <string.h>

struct MemoryChunk {
    char *memory;
    size_t size;
};

/**
 * @brief Performs a HTTP GET request to the specified URL
 *
 * @param uri URI to append to Discord Gateway
 * @param response Pointer to a pointer that will be
 *                 modified to point to point to the data
 * @param handle   Curl handle
 * @return CURLcode
 */
CURLcode request_get(char *uri, char **response);

/**
 * @brief Performs a HTTP POST request to the specified URL
 *
 * @param uri URI to append to Discord Gateway
 * @param response Pointer to a pointer that will be
 *                 modified to point to point to the data
 * @param handle   Curl handle
 * @return CURLcode
 */
CURLcode request_post(char *uri, char **response, cJSON *content);

/**
 * @brief Tests if request POST and GET work
 *
 * @return int 1 if it works, 0 else
 */
int request_test();

#endif