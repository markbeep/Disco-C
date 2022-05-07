#include "libs/web/request.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    curl_global_init(CURL_GLOBAL_ALL);

    char *url = "https://eow6esktcy50ov3.m.pipedream.net";
    CURL *handle = curl_easy_init();
    char *result;
    CURLcode res;

    // GET REQUEST TEST
    res = request_get(url, &result, handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "%d: GET failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            fprintf(stderr, "Have no connection to host\n");
    } else {
        printf("%s\n", result);
    }
    // curl_easy_cleanup(handle);

    // POST REQUEST TEST
    res = request_post(url, handle, result);
    if (res != CURLE_OK) {
        fprintf(stderr, "%d: POST failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            fprintf(stderr, "Have no connection to host\n");
    } else {
        printf("%s\n", result);
    }

    free(result);
}