#ifndef EVENT
#define EVENT

#include <cJSON/cJSON.h>
#include <discord/disco.h>

/**
 * @brief All event websocket responses call this function.
 *
 * @param bot_client Active bot instance
 * @param data  The received data in a JSON struct
 * @param event The event that was called as a string
 */
void event_handle(bot_client_t *bot_client, cJSON *data, char *event);

typedef struct event_pool_workload {
    bot_client_t *bot;
    cJSON *data;
} event_pool_workload_t;

#endif
