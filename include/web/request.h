
#ifndef DISCO_REQUEST
#define DISCO_REQUEST

#define VERBOSE 0

#include <cJSON/cJSON.h>
#include <curl/curl.h>
#include <discord/disco.h>
#include <string.h>

struct MemoryChunk {
    char *memory;
    size_t size;
};

enum Request_Type {
    REQUEST_GET,
    REQUEST_POST,
    REQUEST_PATCH,
    REQUEST_PUT,
    REQUEST_DELETE,
    REQUEST_UPDATE,
};

enum Discord_Request_Callback_Type {
    DISCORD_MESSAGE_CALLBACK,
    DISCORD_MULTIPLE_MESSAGE_CALLBACK,
};

struct request_callback {
    enum Discord_Request_Callback_Type type;
    void *cb_struct;
};

/**
 * @brief Performs a HTTP request to the specified URL
 *
 * @param url URL to append to Discord Gateway
 * @param content Pointer to JSON that should be sent. If there's
 *                nothing to send, it should be NULL.
 * @param request_type The type of request to execute.
 * @param token The bot token.
 * @param rc The callback to execute if there is any.
 * @return long HTTP response code
 */
void request(char *url, cJSON *content, enum Request_Type request_type, const char *token, bot_client_t *bot, struct request_callback *rc, char **files, int files_n);

// TODO add description
struct curl_slist *curl_setup_discord_header_files(CURL *h, const char *token, char **files, int files_n, char *json);

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
