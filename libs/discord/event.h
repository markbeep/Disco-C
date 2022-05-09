#include "../utils/cJSON.h"
#include "../web/websocket.h"

void channel_test_send_message(client_websocket_t *client, char *content);
void event_handle(client_websocket_t *client, cJSON *data, char *event);