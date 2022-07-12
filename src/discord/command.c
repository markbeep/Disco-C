#include <cJSON/cJSON.h>
#include <config.h>
#include <discord/command.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/disco_logging.h>
#include <web/request.h>

void discord_fill_json_with_locales(cJSON *json, struct discord_language_locales *locales) {
    if (locales->da)
        cJSON_AddStringToObject(json, "da", locales->da);
    if (locales->de)
        cJSON_AddStringToObject(json, "de", locales->de);
    if (locales->en_GB)
        cJSON_AddStringToObject(json, "en_GB", locales->en_GB);
    if (locales->en_US)
        cJSON_AddStringToObject(json, "en_US", locales->en_US);
    if (locales->es_ES)
        cJSON_AddStringToObject(json, "es_ES", locales->es_ES);
    if (locales->fr)
        cJSON_AddStringToObject(json, "fr", locales->fr);
    if (locales->hr)
        cJSON_AddStringToObject(json, "hr", locales->hr);
    if (locales->it)
        cJSON_AddStringToObject(json, "it", locales->it);
    if (locales->lt)
        cJSON_AddStringToObject(json, "lt", locales->lt);
    if (locales->hu)
        cJSON_AddStringToObject(json, "hu", locales->hu);
    if (locales->nl)
        cJSON_AddStringToObject(json, "nl", locales->nl);
    if (locales->no)
        cJSON_AddStringToObject(json, "no", locales->no);
    if (locales->pl)
        cJSON_AddStringToObject(json, "pl", locales->pl);
    if (locales->pt_BR)
        cJSON_AddStringToObject(json, "pt_BR", locales->pt_BR);
    if (locales->ro)
        cJSON_AddStringToObject(json, "ro", locales->ro);
    if (locales->fi)
        cJSON_AddStringToObject(json, "fi", locales->fi);
    if (locales->sv_SE)
        cJSON_AddStringToObject(json, "sv_SE", locales->sv_SE);
    if (locales->vi)
        cJSON_AddStringToObject(json, "vi", locales->vi);
    if (locales->tr)
        cJSON_AddStringToObject(json, "tr", locales->tr);
    if (locales->cs)
        cJSON_AddStringToObject(json, "cs", locales->cs);
    if (locales->el)
        cJSON_AddStringToObject(json, "el", locales->el);
    if (locales->bg)
        cJSON_AddStringToObject(json, "bg", locales->bg);
    if (locales->ru)
        cJSON_AddStringToObject(json, "ru", locales->ru);
    if (locales->uk)
        cJSON_AddStringToObject(json, "uk", locales->uk);
    if (locales->hi)
        cJSON_AddStringToObject(json, "hi", locales->hi);
    if (locales->th)
        cJSON_AddStringToObject(json, "th", locales->th);
    if (locales->zh_CN)
        cJSON_AddStringToObject(json, "zh_CN", locales->zh_CN);
    if (locales->ja)
        cJSON_AddStringToObject(json, "ja", locales->ja);
    if (locales->zh_TW)
        cJSON_AddStringToObject(json, "zh_TW", locales->zh_TW);
    if (locales->ko)
        cJSON_AddStringToObject(json, "ko", locales->ko);
}

static void add_choice(cJSON *json, struct discord_application_command_option_choice *choice, enum Discord_Application_Command_Option_Type type) {
    cJSON_AddStringToObject(json, "name", choice->name);
    if (choice->name_localizations) {
        cJSON *locales = cJSON_AddObjectToObject(json, "name_localizations");
        discord_fill_json_with_locales(locales, choice->name_localizations);
    }
    if (type == COMMAND_OPTION_INTEGER || type == COMMAND_OPTION_NUMBER)
        cJSON_AddNumberToObject(json, "value", choice->value.number);
}

static void add_option(cJSON *json, struct discord_application_command_option *option) {
    cJSON_AddStringToObject(json, "name", option->name);
    cJSON_AddStringToObject(json, "description", option->description);
    if (option->name_localizations) {
        cJSON *locales = cJSON_AddObjectToObject(json, "name_localizations");
        discord_fill_json_with_locales(locales, option->name_localizations);
    }
    if (option->description_localizations) {
        cJSON *locales = cJSON_AddObjectToObject(json, "description_localizations");
        discord_fill_json_with_locales(locales, option->description_localizations);
    }
    cJSON_AddBoolToObject(json, "required", option->required);
    // choices
    cJSON *choices_array;
    if (option->choices_count > 0)
        choices_array = cJSON_AddArrayToObject(json, "choices");
    for (int i = 0; i < option->choices_count; i++) {
        cJSON *choice = cJSON_CreateObject();
        cJSON_AddItemToArray(choices_array, choice);
        add_choice(choice, option->choices[i], option->type);
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
        cJSON *opt = cJSON_CreateObject();
        cJSON_AddItemToArray(options_array, opt);
        add_option(opt, ordered_options[i]);
    }
    if (option->channel_types_count > 0) {
        int channel_types_array[option->channel_types_count];
        for (int i = 0; i < option->channel_types_count; i++)
            channel_types_array[i] = (int)option->channel_types[i];
        cJSON *array = cJSON_CreateIntArray(channel_types_array, option->channel_types_count);
        cJSON_AddItemToObject(json, "channel_types", array);
    }
    if (option->type == COMMAND_OPTION_INTEGER || option->type == COMMAND_OPTION_NUMBER) {
        cJSON_AddNumberToObject(json, "min_value", option->min_value);
        cJSON_AddNumberToObject(json, "max_value", option->max_value);
    }
    if (option->type == COMMAND_OPTION_STRING) {
        cJSON_AddNumberToObject(json, "min_length", option->min_length);
        cJSON_AddNumberToObject(json, "max_length", option->max_length);
    }
    cJSON_AddBoolToObject(json, "autocomplete", option->autocomplete);
}

static int format_json(cJSON *json, struct discord_application_command *command) {
    // makes sure the name value is present
    if (!command->name) {
        d_log_err("Name value is missing for command\n");
        return 1;
    }

    cJSON_AddStringToObject(json, "name", command->name);
    if (command->type)
        cJSON_AddNumberToObject(json, "type", command->type);
    cJSON_AddStringToObject(json, "description", command->description);
    if (command->name_localizations) {
        cJSON *locales = cJSON_AddObjectToObject(json, "name_localizations");
        discord_fill_json_with_locales(locales, command->name_localizations);
    }
    if (command->description_localizations) {
        cJSON *locales = cJSON_AddObjectToObject(json, "description_localizations");
        discord_fill_json_with_locales(locales, command->description_localizations);
    }
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

int discord_command_register(struct discord_application_command *command) {
    cJSON *json = cJSON_CreateObject();
    format_json(json, command);
    char url[120];
    char *response;
    if (command->guild_id)
        sprintf(url, "https://discord.com/api/v10/applications/%s/guilds/%ld/commands", APPLICATION_ID, command->guild_id);
    else
        sprintf(url, "https://discord.com/api/v10/applications/%s/commands", APPLICATION_ID);
    CURLcode c = request(url, &response, json, REQUEST_POST);
    cJSON_Delete(json);
    d_log_notice("Command Response: %s\n", response);
    free(response);
    if (c == CURLE_OK)
        return 1;
    return 0;
}

int discord_command_update(struct discord_application_command *command, int64_t command_id) {
    cJSON *json = cJSON_CreateObject();
    format_json(json, command);
    char url[120];
    char *response;
    if (command->guild_id)
        sprintf(url, "https://discord.com/api/v10/applications/%s/guilds/%ld/commands/%ld", APPLICATION_ID, command->guild_id, command_id);
    else
        sprintf(url, "https://discord.com/api/v10/applications/%s/commands/%ld", APPLICATION_ID, command_id);
    CURLcode c = request(url, &response, json, REQUEST_PATCH);
    cJSON_Delete(json);
    free(response);
    if (c == CURLE_OK)
        return 1;
    return 0;
}

int discord_command_delete_global(int64_t command_id) {
    char url[120];
    char *response;
    sprintf(url, "https://discord.com/api/v10/applications/%s/commands/%ld", APPLICATION_ID, command_id);
    CURLcode c = request(url, &response, NULL, REQUEST_DELETE);
    free(response);
    if (c == CURLE_OK)
        return 1;
    return 0;
}

int discord_command_delete_guild(int64_t guild_id, int64_t command_id) {
    char url[120];
    char *response;
    sprintf(url, "https://discord.com/api/v10/applications/%s/guilds/%ld/commands/%ld", APPLICATION_ID, guild_id, command_id);
    CURLcode c = request(url, &response, NULL, REQUEST_DELETE);
    free(response);
    if (c == CURLE_OK)
        return 1;
    return 0;
}
