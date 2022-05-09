#include "../web/request.h"
#include "event.h"
#include <curl/curl.h>

void channel_test_send_message(client_websocket_t *client, char *content) {
    (void)content;

    curl_mime *multipart = curl_mime_init(client->handle);
    curl_mimepart *part = curl_mime_addpart(multipart);
    curl_mime_name(part, "content");
    curl_mime_data(part, "this was sent via C code", CURL_ZERO_TERMINATED);
    char *response;
    fprintf(stderr, "Sending message...\n");
    request_post("/channels/944968090490380321/message", &response, client->handle, multipart);
    fprintf(stderr, "Message sent!\n");

    curl_mime_free(multipart);
}