#ifndef DISCO_COMMAND
#define DISCO_COMMAND

#include "channel.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Models
 * @{
 * @addtogroup Command
 * Command functions and structures
 * @{
 *
 */

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-types
enum Discord_Application_Command_Type {
    COMMAND_CHAT_INPUT = 1,
    COMMAND_USER = 2,
    COMMAND_MESSAGE = 3
};

// https://discord.com/developers/docs/reference#locales
struct discord_language_locales {
    char *da;
    char *de;
    char *en_GB;
    char *en_US;
    char *es_ES;
    char *fr;
    char *hr;
    char *it;
    char *lt;
    char *hu;
    char *nl;
    char *no;
    char *pl;
    char *pt_BR;
    char *ro;
    char *fi;
    char *sv_SE;
    char *vi;
    char *tr;
    char *cs;
    char *el;
    char *bg;
    char *ru;
    char *uk;
    char *hi;
    char *th;
    char *zh_CN;
    char *ja;
    char *zh_TW;
    char *ko;
};

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-option-choice-structure
struct discord_application_command_option_choice {
    char *name;
    struct discord_language_locales *name_localizations;
    union {
        char *str;
        double number;
    } value; // type of value depends on the option type
};

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-option-type
enum Discord_Application_Command_Option_Type {
    COMMAND_OPTION_SUB_COMMAND = 1,
    COMMAND_OPTION_SUB_COMMAND_GROUP = 2,
    COMMAND_OPTION_STRING = 3,
    COMMAND_OPTION_INTEGER = 4,
    COMMAND_OPTION_BOOLEAN = 5,
    COMMAND_OPTION_USER = 6,
    COMMAND_OPTION_CHANNEL = 7,
    COMMAND_OPTION_ROLE = 8,
    COMMAND_OPTION_MENTIONABLE = 9,
    COMMAND_OPTION_NUMBER = 10,
    COMMAND_OPTION_ATTACHMENT = 11
};

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-option-structure
struct discord_application_command_option {
    enum Discord_Application_Command_Option_Type type;
    char *name;
    struct discord_language_locales *name_localizations;
    char *description;
    struct discord_language_locales *description_localizations;
    bool required;
    struct discord_application_command_option_choice **choices;
    int choices_count;
    struct discord_application_command_option **options;
    int options_count;
    enum Discord_Channel_Type *channel_types;
    int channel_types_count;
    double min_value;
    double max_value;
    int min_length;
    int max_length;
    bool autocomplete;
};

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-structure
struct discord_application_command {
    int64_t *id;
    enum Discord_Application_Command_Type type;
    int64_t guild_id;
    char *name;
    struct discord_language_locales *name_localizations;
    char *description;
    struct discord_language_locales *description_localizations;
    struct discord_application_command_option **options;
    int options_count;
    char *default_member_permissions;
    bool dm_permission;
    bool default_permission;
    int64_t version;
};

/**
 * @brief Fills a JSON with the values of a langauge locales struct.
 *
 * @param json JSON struct to fill the values in.
 * @param locales struct to be used to fill in the JSON.
 */

void discord_fill_json_with_locales(cJSON *json, struct discord_language_locales *locales);
int discord_command_register(struct discord_application_command *command, const char *token);
int discord_command_update(struct discord_application_command *command, int64_t command_id, const char *token);
int discord_command_delete_global(int64_t command_id, const char *token);
int discord_command_delete_guild(int64_t guild_id, int64_t command_id, const char *token);

/** @} @} */
#endif
