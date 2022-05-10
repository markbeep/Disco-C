#include "../web/request.h"
#include "event.h"
#include <curl/curl.h>

void channel_test_send_message(client_websocket_t *client, char *content) {
    (void)content;
    (void)client;

    cJSON *json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "content", cJSON_CreateString(content));

    char *response;
    fprintf(stderr, "Sending message...\n");
    CURLcode res = request_post("/channels/944968090490380321/messages", &response, json);

    if (res != CURLE_OK) {
        fprintf(stderr, "%d: POST failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            fprintf(stderr, "Have no connection to host\n");
        return;
    }
    fprintf(stderr, "Message sent!\n");
    fprintf(stderr, "Response: char = %s\n", response);
}