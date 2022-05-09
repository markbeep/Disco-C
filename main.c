#include "libs/web/gateway.h"
#include "libs/web/request.h"
#include "libs/web/websocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    curl_global_init(CURL_GLOBAL_ALL);

    gateway_test();

    curl_global_cleanup();
}