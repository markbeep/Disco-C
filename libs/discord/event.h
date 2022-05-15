#ifndef EVENT
#define EVENT

#include "../utils/cJSON.h"
#include "disco.h"

void channel_test_send_message(bot_client_t *client, char *content);

/**
 * @brief All event websocket responses call this function.
 *
 * @param bot_client Active bot instance
 * @param data  The received data in a JSON struct
 * @param event The event that was called as a string
 */
void event_handle(bot_client_t *bot_client, cJSON *data, char *event);

#endif