#include "./definitions/channel.h"
#include "../web/request.h"
#include "./definitions/message.h"
#include "event.h"
#include <curl/curl.h>

void channel_send_message(bot_client_t *bot, struct discord_create_message *message, char *channel_id) {
    (void)bot;

    char uri[48];
    sprintf(uri, "/channels/%s/messages", channel_id);
    cJSON *json = cJSON_CreateObject();
    // content
    if (message->content) {
        cJSON_AddItemToObject(json, "content", cJSON_CreateString(message->content));
    }
    // embeds
    if (message->embeds_count > 0) {
        cJSON *embeds = cJSON_AddArrayToObject(json, "embeds");
        for (int i = 0; i < message->embeds_count; i++) {
            cJSON *embed_obj = cJSON_CreateObject();
            // TODO actually create the embed object
            // cJSON_AddItemToObject(embed_obj, "title", message->embeds[i]->title);
            // cJSON_AddItemToObject(embed_obj, "description", message->embeds[i]->title);
            cJSON_AddItemToArray(embeds, embed_obj);
        }
    }

    char *response;
    CURLcode res = request_post(uri, &response, json);
    if (res != CURLE_OK) {
        fprintf(stderr, "%d: POST failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            fprintf(stderr, "Have no connection to host\n");
        return;
    }
    fprintf(stderr, "Message sent!\n");
    fprintf(stderr, "Response: char = %s\n", response);

    free(response);
}

void channel_test_send_message(bot_client_t *bot_client, char *content) {
    (void)content;
    (void)bot_client;

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

    free(response);
}