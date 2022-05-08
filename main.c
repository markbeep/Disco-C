#include "libs/web/gateway.h"
#include "libs/web/request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    curl_global_init(CURL_GLOBAL_ALL);

    websocket_test();

    curl_global_cleanup();
}