#ifndef DISCO_INTERACTION
#define DISCO_INTERACTION

#include "command.h"
#include "component.h"
#include "message.h"
#include <cJSON/cJSON.h>
#include <discord/disco.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Interaction
 * Interaction functions and structures
 * @{
 *
 */

enum Discord_Interaction_Type { INTERACTION_PING = 1,
                                INTERACTION_APPLICATION_COMMAND = 2,
                                INTERACTION_MESSAGE_COMPONENT = 3,
                                INTERACTION_APPLICATION_COMMAND_AUTOCOMPLETE = 4,
                                INTERACTION_MODAL_SUBMIT = 5 };

enum Value_Type { VALUE_STRING,
                  VALUE_INTEGER,
                  VALUE_DOUBLE };

/** https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-interaction-data-option-structure */
struct discord_interaction_data_option {
    char *name;
    enum Discord_Application_Command_Option_Type type;
    enum Value_Type value_type;
    union {
        char *string_;
        int int_;
        double double_;
    } value;
    struct discord_interaction_data_option **options;
    int options_count;
    bool focused;
};

/**
 * @brief Creates an interaction data option structure from a given JSON.
 *
 * @param data
 * @return void* discord_interaction_data_option struct
 */
void *_d_json_to_interaction_data_option(cJSON *data);

/**
 * @brief Copies an interaction data option structure.
 *
 * @param src Interaction data option to copy.
 * @return struct discord_interaction_data_option*
 */
struct discord_interaction_data_option *_d_copy_interaction_data_option(struct discord_interaction_data_option *src);

/**
 * @brief Destroys the given structure and frees the pointer.
 *
 * @param interaction
 */
void discord_destroy_interaction_data_option(struct discord_interaction_data_option *interaction);

/** https://discord.com/developers/docs/interactions/receiving-and-responding#interaction-object-interaction-data */
struct discord_interaction_data {
    // base application command data structure
    uint64_t id;
    char *name;
    enum Discord_Application_Command_Type type;
    // resolved data was skipped
    struct discord_interaction_data_option **options;
    int options_count;
    uint64_t guild_id;
    uint64_t target_id;

    char *custom_id;                                 // message/modal
    enum Discord_Component_Type component_type;      // message
    struct discord_interaction_data_option **values; // message
    int values_count;                                // message
    struct discord_component **components;           // modal
    int components_count;                            // modal
};

/**
 * @brief Creates an interaction data structure from a given JSON.
 *
 * @param data
 * @return void* discord_interaction_data struct
 */
void *_d_json_to_interaction_data(cJSON *data);

/**
 * @brief Copies an interaction data structure.
 *
 * @param src Interaction data to copy.
 * @return struct discord_interaction_data*
 */
struct discord_interaction_data *_d_copy_interaction_data(struct discord_interaction_data *src);

/**
 * @brief Destroys the given structure and frees the pointer.
 *
 * @param interaction
 */
void discord_destroy_interaction_data(struct discord_interaction_data *interaction);

/** https://discord.com/developers/docs/interactions/receiving-and-responding#interaction-object-interaction-structure */
struct discord_interaction {
    uint64_t id;
    uint64_t application_id;
    enum Discord_Interaction_Type type;
    struct discord_interaction_data *data;
    uint64_t guild_id;
    uint64_t channel_id;
    struct discord_member *member;
    struct discord_user *user;
    char *token;
    int version;
    struct discord_message *message;
    char *app_permissions;
    char *locale;
    char *guild_locale;
};

/**
 * @brief Creates an interaction structure from a given JSON.
 *
 * @param data
 * @return void*
 */
void *_d_json_to_interaction(cJSON *data);

/**
 * @brief Copies an interaction structure.
 *
 * @param src Interaction to copy.
 * @return struct discord_interaction*
 */
struct discord_interaction *_d_copy_interaction(struct discord_interaction *src);

/**
 * @brief Destroys the given structure and frees the pointer.
 *
 * @param interaction
 */
void discord_destroy_interaction(struct discord_interaction *interaction);

/** https://discord.com/developers/docs/interactions/receiving-and-responding#interaction-response-object-interaction-callback-type */
enum Discord_Interaction_Callback_Type {
    DISCORD_CALLBACK_PONG = 1,
    DISCORD_CALLBACK_CHANNEL_MESSAGE_WITH_SOURCE = 4,
    DISCORD_CALLBACK_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE = 5,
    DISCORD_CALLBACK_DEFERRED_UPDATE_MESSAGE = 6,
    DISCORD_CALLBACK_UPDATE_MESSAGE = 7,
    DISCORD_CALLBACK_APPLICATION_COMMAND_AUTOCOMPLETE_RESULT = 8,
    DISCORD_CALLBACK_MODAL = 9
};

/** https://discord.com/developers/docs/interactions/receiving-and-responding#interaction-response-object-interaction-response-structure */
struct discord_interaction_callback {
    enum Discord_Interaction_Callback_Type type;
    union {
        struct {
            bool tts;
            char *content;
            struct discord_embed **embeds;
            int embeds_count;
            struct discord_allowed_mentions *allowed_mentions;
            int flags;
            struct discord_component **components;
            int components_count;
            struct discord_attachment **attachments;
            int attachments_count;
        } message;
        struct {
            struct discord_application_command_option_choice **choices;
            int choices_count;
        } autocomplete;
        struct {
            char *custom_id;
            char *title;
            struct discord_component **components;
            int components_count;
        } modal;
    } data;
};

// TODO make JSON fill functions public (for message, embeds, etc.)
void discord_fill_json_interaction_callback(cJSON *json, struct discord_interaction_callback *callback);

void discord_send_interaction(bot_client_t *bot, struct discord_interaction_callback *cb, struct discord_interaction *received_interaction);

/** @} @} */
#endif
