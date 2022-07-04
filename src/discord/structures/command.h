#ifndef DISCO_COMMAND
#define DISCO_COMMAND

#include "channel.h"
#include <stdbool.h>
#include <stdint.h>

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-types
enum Application_Command_Type {
    COMMAND_CHAT_INPUT = 1,
    COMMAND_USER = 2,
    COMMAND_MESSAGE = 3
};

// https://discord.com/developers/docs/reference#locales
struct discord_language_locales {};

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-option-choice-structure
struct discord_application_command_option_choice {
    char *name;
    struct discord_language_locales *name_localizations;
    union {
        char *str;
        double d;
        int i;
    } value; // type of value depends on the option type
};

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-option-type
enum Application_Command_Option_Type {
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
    enum Application_Command_Type type;
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
    union {
        int i;
        double d;
    } min_value;
    union {
        int i;
        double d;
    } max_value;
    bool autocomplete;
};

// https://discord.com/developers/docs/interactions/application-commands#application-command-object-application-command-structure
struct discord_application_command_structure {
    int64_t *id;
    enum Application_Command_Type type;
    int64_t application_id;
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

#endif
