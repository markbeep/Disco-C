#include "structures/command.h"
#include "../../config.h"
#include "../utils/disco_logging.h"
#include "../web/request.h"
#include <cJSON/cJSON.h>
#include <stdio.h>
#include <stdlib.h>

static void add_localization(cJSON *json, char *name, struct discord_language_locales *locales) {
    (void)json;
    (void)locales;
}

static void add_choice(cJSON *json, struct discord_application_command_option_choice *choice) {
}

static void add_option(cJSON *json, struct discord_application_command_option *option) {
    cJSON_AddStringToObject(json, "name", option->name);
    cJSON_AddStringToObject(json, "description", option->description);
    if (option->name_localizations)
        add_localization(json, "name_localizations", option->name_localizations);
    if (option->description_localizations)
        add_localization(json, "description_localizations", option->description_localizations);
    cJSON_AddBoolToObject(json, "required", option->required);
    // choices
    cJSON *choices_array;
    if (option->choices_count > 0)
        choices_array = cJSON_AddArrayToObject(json, "choices");
    for (int i = 0; i < option->choices_count; i++) {
        cJSON *choice = cJSON_CreateObject();
        cJSON_AddItemToArray(choices_array, choice);
        add_choice(choice, option->choices[i]);
    }
    // options
    cJSON *options_array;
    struct discord_application_command_option *ordered_options[option->options_count];
    if (option->options_count > 0) {
        options_array = cJSON_AddArrayToObject(json, "options");
        // we have to reorder the options so that the required options are first
        int cur = 0;
        for (int i = 0; i < option->options_count; i++)
            if (option->options[i]->required)
                ordered_options[cur++] = option->options[i];
        for (int i = 0; i < option->options_count; i++)
            if (!option->options[i]->required)
                ordered_options[cur++] = option->options[i];
    }
    for (int i = 0; i < option->options_count; i++) {
        cJSON *option = cJSON_CreateObject();
        cJSON_AddItemToArray(options_array, option);
        add_option(option, ordered_options[i]);
    }
    if (option->channel_types_count > 0) {
        int channel_types_array[option->channel_types_count];
        for (int i = 0; i < option->channel_types_count; i++)
            channel_types_array[i] = (int)option->channel_types[i];
        cJSON *array = cJSON_CreateIntArray(channel_types_array, option->channel_types_count);
        cJSON_AddItemToObject(json, "channel_types", array);
    }
    if (option->type == COMMAND_OPTION_INTEGER)
        cJSON_AddNumberToObject(json, "min_value", option->min_value.i);
    else if (option->type == COMMAND_OPTION_NUMBER)
        cJSON_AddNumberToObject(json, "min_value", option->min_value.d);
    if (option->type == COMMAND_OPTION_INTEGER)
        cJSON_AddNumberToObject(json, "max_value", option->max_value.i);
    else if (option->type == COMMAND_OPTION_NUMBER)
        cJSON_AddNumberToObject(json, "max_value", option->max_value.d);
    cJSON_AddBoolToObject(json, "autocomplete", option->autocomplete);
}

static int format_json(cJSON *json, struct discord_application_command_structure *command) {
    // makes sure the name value is present
    if (!command->name) {
        d_log_err("Name value is missing for command\n");
        return 1;
    }

    cJSON_AddStringToObject(json, "name", command->name);
    if (command->type)
        cJSON_AddNumberToObject(json, "type", command->type);
    cJSON_AddStringToObject(json, "description", command->description);
    if (command->name_localizations)
        add_localization(json, "name_localizations", command->name_localizations);
    if (command->description_localizations)
        add_localization(json, "description_localizations", command->description_localizations);
    // options
    cJSON *options_array;
    struct discord_application_command_option *ordered_options[command->options_count];
    if (command->options_count > 0) {
        options_array = cJSON_AddArrayToObject(json, "options");
        // we have to reorder the options so that the required options are first
        int cur = 0;
        for (int i = 0; i < command->options_count; i++)
            if (command->options[i]->required)
                ordered_options[cur++] = command->options[i];
        for (int i = 0; i < command->options_count; i++)
            if (!command->options[i]->required)
                ordered_options[cur++] = command->options[i];
    }
    for (int i = 0; i < command->options_count; i++) {
        cJSON *option = cJSON_CreateObject();
        cJSON_AddItemToArray(options_array, option);
        add_option(option, ordered_options[i]);
    }
    if (command->default_member_permissions)
        cJSON_AddStringToObject(json, "default_member_permissions", command->default_member_permissions);
    cJSON_AddBoolToObject(json, "dm_permission", command->dm_permission);
    char version[20];
    if (command->version) {
        sprintf(version, "%ld", command->version);
        cJSON_AddStringToObject(json, "version", version);
    }

    return 0;
}

int discord_command_register_global(struct discord_application_command_structure *command) {
    cJSON *json = cJSON_CreateObject();
    format_json(json, command);
    char url[120];
    char *response;
    sprintf(url, "https://discord.com/api/v10/applications/%s/commands", APPLICATION_ID);
    CURLcode c = request(url, &response, json, REQUEST_POST);
    cJSON_Delete(json);
    free(response);
    if (c == CURLE_OK)
        return true;
    return false;
}

int discord_command_register_guild(struct discord_application_command_structure *command, int64_t guild_id) {
    cJSON *json = cJSON_CreateObject();
    format_json(json, command);
    char url[120];
    char *response;
    sprintf(url, "https://discord.com/api/v10/applications/%s/guilds/%ld/commands", APPLICATION_ID, guild_id);
    CURLcode c = request(url, &response, json, REQUEST_POST);
    cJSON_Delete(json);
    free(response);
    if (c == CURLE_OK)
        return true;
    return false;
}

int discord_command_update_global(struct discord_application_command_structure *command, int64_t guild_id, int64_t command_id) {
    cJSON *json = cJSON_CreateObject();
    format_json(json, command);
    char url[120];
    char *response;
    sprintf(url, "https://discord.com/api/v10/applications/%s/commands/%ld", APPLICATION_ID, command_id);
    CURLcode c = request(url, &response, json, REQUEST_PATCH);
    cJSON_Delete(json);
    free(response);
    if (c == CURLE_OK)
        return true;
    return false;
}

int discord_command_update_guild(struct discord_application_command_structure *command, int64_t guild_id, int64_t command_id) {
    cJSON *json = cJSON_CreateObject();
    format_json(json, command);
    char url[120];
    char *response;
    sprintf(url, "https://discord.com/api/v10/applications/%s/guilds/%ld/commands/%ld", APPLICATION_ID, guild_id, command_id);
    CURLcode c = request(url, &response, json, REQUEST_PATCH);
    cJSON_Delete(json);
    free(response);
    if (c == CURLE_OK)
        return true;
    return false;
}

int discord_command_delete_global(int64_t command_id) {
    char url[120];
    char *response;
    sprintf(url, "https://discord.com/api/v10/applications/%s/commands/%ld", APPLICATION_ID, command_id);
    CURLcode c = request(url, &response, NULL, REQUEST_DELETE);
    free(response);
    if (c == CURLE_OK)
        return true;
    return false;
}

int discord_command_delete_guild(int64_t guild_id, int64_t command_id) {
    char url[120];
    char *response;
    sprintf(url, "https://discord.com/api/v10/applications/%s/guilds/%ld/commands/%ld", APPLICATION_ID, guild_id, command_id);
    CURLcode c = request(url, &response, NULL, REQUEST_DELETE);
    free(response);
    if (c == CURLE_OK)
        return true;
    return false;
}
