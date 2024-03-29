#include <discord/disco.h>
#include <libwebsockets.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utils/disco_logging.h>
#include <utils/t_pool.h>
#include <web/request.h>

static void d_curl_setup_discord_header_files(CURL *h, const char *token, char **files, int files_n, char *json, struct curl_slist *list, struct curl_slist *content_l, struct curl_slist **attachments_l, curl_mime *mime);
static struct curl_slist *d_curl_setup_discord_header(CURL *handle, const char *token);

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

struct request_work_node {
    char *url;
    char *json;
    enum Request_Type request_type;
    char *token;
    struct request_callback *rc;
    bot_client_t *bot;
    int iteration;
    long http;
    CURLcode res;
    CURL *handle;
    char **files;
    int files_n;
};

static void request_t_pool(void *w, CURL *handle) {
    struct request_work_node *n = (struct request_work_node *)w;

    struct curl_slist *list = NULL, **attachments_l = NULL, *content_l = NULL;
    curl_mime *mime = NULL;
    if (n->files_n == 0) {
        list = d_curl_setup_discord_header(handle, n->token);
        if (n->json) {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, n->json);
        } else {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "");
        }
    } else {
        attachments_l = malloc(sizeof(struct curl_slist *) * n->files_n);
        d_curl_setup_discord_header_files(handle, n->token, n->files, n->files_n, n->json, list, content_l, attachments_l, mime);
    }

    char *request_str = NULL;
    switch (n->request_type) {
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

    d_log_info("REQUEST '%s' URL: %s\n", request_str, n->url);

    curl_easy_setopt(handle, CURLOPT_URL, n->url);
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, request_str);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);

    // SENDING PART
    bool sent_message = false;
    int iterations = 0;
    char *response;
    cJSON *res_json = NULL, *wait_ms; // response json, rate limit timeout json field
    if (n->iteration >= 5) {
        if (n->res == 0) {
            d_log_err("CURL failed. Code: %d\n", (int)n->res);
        } else if (n->http == 502) {
            d_log_err("GATEWAY UNAVAILABLE. There was not a gateway available to process the request.\n");
        }
        sent_message = true;
    } else {
        chunk.memory = NULL;
        chunk.size = 0;
        n->res = curl_easy_perform(handle);
        response = chunk.memory;
        // gets the http code
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &n->http);
        if (n->http >= 200 && n->http <= 299) {
            sent_message = true;

        } else if (n->http == 400) {
            d_log_err("BAD REQUEST. Request was improperly formatted.\n");
            sent_message = true;
        } else if (n->http == 401) {
            d_log_err("UNAUTHORIZED. The Authorization header was missing or invalid.\n");
            sent_message = true;
        } else if (n->http == 403) {
            d_log_err("FORBIDDEN. The Authorization token you passed did not have permission to the resource.\n");
            sent_message = true;
        } else if (n->http == 404) {
            d_log_err("NOT FOUND. The resource at the location specified doesn't exist.\n");
            sent_message = true;
        } else if (n->http == 405) {
            d_log_err("METHOD NOT ALLOWED. The HTTP method used is not valid for the location specified.\n");
            sent_message = true;
        } else if (n->http == 429) {
            res_json = cJSON_Parse(response);
            wait_ms = cJSON_GetObjectItem(res_json, "retry_after");
            lwsl_debug("TOO MANY REQUESTS. We are being rate limited, waiting %.3f s.\n", wait_ms->valuedouble);
            if (cJSON_IsNumber(wait_ms)) {
                struct timeval t0;
                gettimeofday(&t0, NULL);
                t0.tv_sec += (time_t)wait_ms->valuedouble;
                t0.tv_usec += (time_t)(wait_ms->valuedouble * 1e6) % (time_t)1e6;
                // check for overflow
                if (t0.tv_usec > 1000000) {
                    t0.tv_sec++;
                    t0.tv_usec -= (time_t)1000000;
                }
                t_pool_add_work(n->bot->thread_pool, &request_t_pool, (void *)n, t0);
            }
            cJSON_Delete(res_json);
            res_json = NULL;
        } else if (n->http == 0 || (n->http >= 500 && n->http <= 599) || n->http == 409) { // if CURL fails we get 0
            lwsl_notice("Received a %ld error\n", n->http);
            usleep((1 << iterations) * 1000000u);
            struct timeval t0;
            gettimeofday(&t0, NULL);
            t0.tv_sec += (time_t)1 << n->iteration;
            n->iteration++;
            t_pool_add_work(n->bot->thread_pool, &request_t_pool, (void *)n, t0);
        } else {
            d_log_err("Unhandled HTTP response code: %ld\n", n->http);
            sent_message = true;
        }
    }

    // CLEANUP
    curl_easy_reset(handle);
    if (list)
        curl_slist_free_all(list);
    if (content_l)
        curl_slist_free_all(content_l);
    if (attachments_l) {
        for (int i = 0; i < n->files_n; i++)
            curl_slist_free_all(attachments_l[i]);
        free(attachments_l);
    }
    if (mime)
        curl_mime_free(mime);
    if (n->files_n > 0) {
        for (int i = 0; i < n->files_n; i++)
            free(n->files[i]);
        free(n->files);
    }

    // if the request wasn't sent, we don't call a callback
    if (!sent_message)
        return;

    // declarations for the switch cases
    if (response) {
        res_json = cJSON_Parse(response);
        free(response);
    }
    struct discord_message *msg;
    // handle the callbacks
    if (n->rc) {
        switch (n->rc->type) {
        case DISCORD_MESSAGE_CALLBACK:;
            msg = (struct discord_message *)_d_json_to_message(res_json);
            struct discord_message_cb *cb = (struct discord_message_cb *)n->rc->cb_struct;
            cb->cb(n->bot, msg, cb->data);
            break;
        case DISCORD_MULTIPLE_MESSAGE_CALLBACK:;
            struct discord_multiple_message_cb *cb_mul = (struct discord_multiple_message_cb *)n->rc->cb_struct;
            if (!cJSON_IsArray(res_json)) {
                cb_mul->cb(n->bot, 0, NULL, cb_mul->data);
            } else {
                int received = 0;
                struct discord_message **message_array = (struct discord_message **)malloc(sizeof(struct discord_message *) * received);
                cJSON *c_message;
                cJSON_ArrayForEach(c_message, res_json) {
                    msg = (struct discord_message *)_d_json_to_message(c_message);
                    message_array[received++] = msg;
                    if (msg)
                        discord_cache_set_message(msg);
                }
                cb_mul->cb(n->bot, received, message_array, cb_mul->data);
            }

        default:
            break;
        }
        free(n->rc);
    }

    // CLEANUP
    if (res_json)
        cJSON_Delete(res_json);
    if (n->json)
        free(n->json);
    free(n->url);
    free(n->token);
    free(n);
}

void request(char *url, cJSON *content, enum Request_Type request_type, const char *token, bot_client_t *bot, struct request_callback *rc, char **files, int files_n) {
    // we create a new handle each call because we can't use the same handle over multiple threads
    struct request_work_node *n = (struct request_work_node *)malloc(sizeof(struct request_work_node));
    n->url = strndup(url, 2050);
    n->json = NULL;
    if (content)
        n->json = cJSON_Print(content);
    n->request_type = request_type;
    n->token = strndup(token, 200);
    n->rc = rc;
    n->bot = bot;
    n->iteration = 0;
    n->files = files;
    n->files_n = files_n;
    struct timeval t0 = {0};
    t_pool_add_work(bot->thread_pool, &request_t_pool, (void *)n, t0);
}

/**
 * @brief Setups the request headers and body with attachments.
 * list, content_l, attachments_l and mime all need to be freed.
 * @param h CURL handle.
 * @param token Bot token.
 * @param files Array of filenames.
 * @param files_n File amounts.
 * @param json String of the json body-
 * @param list Main headers.
 * @param content_l Content headers.
 * @param attachments_l Attachments headers.
 * @param mime Mime part.
 * @return struct curl_slist*
 */
static void d_curl_setup_discord_header_files(CURL *h, const char *token, char **files, int files_n, char *json, struct curl_slist *list, struct curl_slist *content_l, struct curl_slist **attachments_l, curl_mime *mime) {
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, write_data);
    char authorizationHeader[100];
    sprintf(authorizationHeader, "Authorization: Bot %s", token);
    list = curl_slist_append(list, authorizationHeader);
    list = curl_slist_append(list, "Accept: application/json");
    list = curl_slist_append(list, "Content-Type: multipart/form-data");
    list = curl_slist_append(list, "User-Agent: DiscordBot (v0.0.1)");
    curl_easy_setopt(h, CURLOPT_HTTPHEADER, list);

    curl_mimepart *part;
    mime = curl_mime_init(h);
    // add json content
    part = curl_mime_addpart(mime);
    content_l = curl_slist_append(content_l, "Content-Type: application/json");
    content_l = curl_slist_append(content_l, "Content-Disposition: form-data; name=\"payload_json\"");
    curl_mime_headers(part, content_l, 0);
    curl_mime_data(part, json, strnlen(json, 20000));

    // add files
    for (int i = 0; i < files_n; i++) {
        char disposition[200];
        sprintf(disposition, "Content-Disposition: form-data; name=\"files[%d]\"; filename=\"%s\"", i, files[i]);
        attachments_l[i] = curl_slist_append(attachments_l[i], disposition);
        part = curl_mime_addpart(mime);
        curl_mime_headers(part, attachments_l[i], 0);
        // add the binary data
        curl_mime_filedata(part, files[i]);
    }

    curl_easy_setopt(h, CURLOPT_MIMEPOST, mime);
}

/**
 * @brief Adds the correct Discord headers to a CURL handle
 *
 * @param handle CURL handle.
 * @param token Bot token.
 * @return struct curl_slist*
 */
static struct curl_slist *d_curl_setup_discord_header(CURL *h, const char *token) {
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, write_data);
    char authorizationHeader[100];
    sprintf(authorizationHeader, "Authorization: Bot %s", token);
    struct curl_slist *list = NULL;
    list = curl_slist_append(list, authorizationHeader);
    list = curl_slist_append(list, "Accept: application/json");
    list = curl_slist_append(list, "Content-Type: application/json");
    list = curl_slist_append(list, "User-Agent: DiscordBot (v0.0.1)");
    curl_easy_setopt(h, CURLOPT_HTTPHEADER, list);
    return list;
}
