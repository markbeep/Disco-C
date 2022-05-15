#include "libs/discord/disco.h"
#include "libs/discord/event.h"
#include "libs/web/gateway.h"
#include "libs/web/request.h"
#include "libs/web/websocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    // LOG LEVEL
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    // init to 0. Without this some errors could show up
    disco_callbacks_t callbacks = {0};

    // starts the bot. This function blocks
    disco_start_bot(&callbacks);

    return 0;
}