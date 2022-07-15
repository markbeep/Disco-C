
#ifndef DISCO_REQUEST
#define DISCO_REQUEST

#define VERBOSE 0

#include <cJSON/cJSON.h>
#include <curl/curl.h>
#include <string.h>

struct MemoryChunk {
    char *memory;
    size_t size;
};

enum Request_Type { REQUEST_GET,
                    REQUEST_POST,
                    REQUEST_PATCH,
                    REQUEST_PUT,
                    REQUEST_DELETE,
                    REQUEST_UPDATE };

/**
 * @brief Performs a HTTP request to the specified URL
 *
 * @param url URL to append to Discord Gateway
 * @param response Pointer to a pointer that will be
 *                 modified to point to point to the data
 * @param content Pointer to JSON that should be sent. If there's
 *                nothing to send, it should be NULL.
 * @param request_type The type of request to execute
 * @return CURLcode
 */
CURLcode request(char *url, char **response, cJSON *content, enum Request_Type request_type, const char *token);

/**
 * @brief Adds the correct Discord headers to a CURL handle
 *
 * @param handle
 * @return struct curl_slist*
 */
struct curl_slist *curl_setup_discord_header(CURL *handle, const char *token);

/**
 * @brief Tests if request POST and GET work
 *
 * @return int 1 if it works, 0 else
 */
int request_test(void);

#endif
