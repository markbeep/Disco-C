#ifndef GATEWAY
#define GATEWAY

#include "../discord/disco.h"
#include "../discord/event.h"
#include "../utils/cJSON.h"
#include "websocket.h"

#define DISCORD_DISPATCH 0
#define DISCORD_HEARTBEAT 1
#define DISCORD_IDENTIFY 2
#define DISCORD_PRESENCE_UPDATE 3
#define DISCORD_VOICE_STATE_UPDATE 4
#define DISCORD_RESUME 6
#define DISCORD_RECONNECT 7
#define DISCORD_REQUEST_GUILD_MEMBERS 8
#define DISCORD_INVALID_SESSION 9
#define DISCORD_HELLO 10
#define DISCORD_HEARTBEAT_ACK 11

/**
 * @brief Callback function for when a websocket package is received.
 *
 * @param bot_client A bot instance
 * @param data  The data that was received
 * @param len  The length of the data
 */
void gateway_on_receive(bot_client_t *bot_client, char *data, size_t len);

/**
 * @brief Initiates the heartbeat loop. Intended to be run on a seperate thread.
 *
 * @param vargp Pointer to the active websocket client
 * @return void*
 */
void *gateway_heartbeat_loop(void *vargp);

/**
 * @brief Initiates the event loop. This is the main loop that needs to be run
 * for the bot to work.
 *
 * @param bot Active bot instance
 */
void gateway_event_loop(bot_client_t *bot);

#endif