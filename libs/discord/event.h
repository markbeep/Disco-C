#ifndef EVENT
#define EVENT

#include "../utils/cJSON.h"
#include "../web/websocket.h"
#include "disco.h"

void channel_test_send_message(bot_client_t *client, char *content);
void event_handle(bot_client_t *client, cJSON *data, char *event);

#endif