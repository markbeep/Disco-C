#include "structures/interaction.h"
#include "../utils/disco_logging.h"
#include "../web/request.h"
#include "disco.h"
#include <stdlib.h>

void *disco_create_interaction_data_option_struct_json(cJSON *data) {
    struct discord_interaction_data_option *opt = (struct discord_interaction_data_option *)calloc(1, sizeof(struct discord_interaction_data_option));
    opt->name = get_string_from_json(data, "name");
    opt->type = (enum Discord_Application_Command_Option_Type)get_int_from_json(data, "type", 0);
    if (opt->type == COMMAND_OPTION_STRING)
        opt->value.string_ = get_string_from_json(data, "value");
    else if (opt->type == COMMAND_OPTION_INTEGER)
        opt->value.int_ = get_int_from_json(data, "value", 0);
    else if (opt->type == COMMAND_OPTION_NUMBER)
        opt->value.double_ = get_double_from_string_json(data, "value", 0.);
    opt->options_count = get_array_from_json(data, "options", (void ***)&opt->options, sizeof(struct discord_interaction_data_option), &disco_create_interaction_data_option_struct_json);
    opt->focused = get_bool_from_json(data, "focused", 0);
    return opt;
}

void disco_destroy_interaction_data_option(struct discord_interaction_data_option *o) {
    if (o->name)
        free(o->name);
    if (o->type == COMMAND_OPTION_STRING && o->value.string_)
        free(o->value.string_);
    for (int i = 0; i < o->options_count; i++)
        disco_destroy_interaction_data_option(o->options[i]);
    free(o->options);
    free(o);
}

void *disco_create_interaction_data_struct_json(cJSON *data) {
    struct discord_interaction_data *d = (struct discord_interaction_data *)calloc(1, sizeof(struct discord_interaction_data));
    d->id = get_long_from_string_json(data, "id", 0);
    d->name = get_string_from_json(data, "name");
    d->type = (enum Discord_Application_Command_Type)get_int_from_json(data, "type", 1);
    d->options_count = get_array_from_json(data, "options", (void ***)&d->options, sizeof(struct discord_interaction_data_option), &disco_create_interaction_data_option_struct_json);
    d->guild_id = get_long_from_string_json(data, "guild_id", 0);
    d->target_id = get_long_from_string_json(data, "target_id", 0);

    d->custom_id = get_string_from_json(data, "custom_id");                                                                                                         // message/modal
    d->values_count = get_array_from_json(data, "values", (void ***)&d->values, sizeof(struct discord_select_option), &disco_create_select_option_struct);          // message
    d->components_count = get_array_from_json(data, "components", (void ***)&d->components, sizeof(struct discord_component), &disco_create_component_struct_json); // modal
    return d;
}

void disco_destroy_interaction_data(struct discord_interaction_data *interaction) {
    if (interaction->name)
        free(interaction->name);
    for (int i = 0; i < interaction->options_count; i++)
        disco_destroy_interaction_data_option(interaction->options[i]);
    free(interaction->options);

    if (interaction->custom_id)
        free(interaction->custom_id);                                  // message/modal
    for (int i = 0; i < interaction->values_count; i++)                // message
        disco_destroy_interaction_data_option(interaction->values[i]); // message
    free(interaction->values);                                         // message
    for (int i = 0; i < interaction->components_count; i++)            // modal
        disco_destroy_component(interaction->components[i]);           // modal
    free(interaction->components);                                     // modal
    free(interaction);
}

void *disco_create_interaction_struct_json(cJSON *data) {
    cJSON *tmp;
    struct discord_interaction *inter = (struct discord_interaction *)calloc(1, sizeof(struct discord_interaction));
    inter->id = get_long_from_string_json(data, "id", 0);
    inter->application_id = get_long_from_string_json(data, "application_id", 0);
    inter->type = (enum Discord_Interaction_Type)get_int_from_json(data, "type", 1);
    tmp = cJSON_GetObjectItem(data, "data");
    if (tmp)
        inter->data = disco_create_interaction_data_struct_json(tmp);
    inter->guild_id = get_long_from_string_json(data, "guild_id", 0);
    inter->channel_id = get_long_from_string_json(data, "channel_id", 0);
    tmp = cJSON_GetObjectItem(data, "user");
    if (tmp)
        inter->user = disco_create_user_struct_json(tmp);
    tmp = cJSON_GetObjectItem(data, "member");
    if (tmp)
        inter->member = disco_create_member_struct_json(tmp, inter->user);
    inter->token = get_string_from_json(data, "token");
    inter->version = get_int_from_json(data, "version", 0);
    tmp = cJSON_GetObjectItem(data, "message");
    if (tmp)
        inter->message = disco_create_message_struct_json(tmp);
    inter->app_permissions = get_string_from_json(data, "app_permissions");
    inter->locale = get_string_from_json(data, "locale");
    inter->guild_locale = get_string_from_json(data, "guild_locale");

    return inter;
}

void disco_destroy_interaction(struct discord_interaction *interaction) {
    if (interaction->data)
        disco_destroy_interaction_data(interaction->data);
    if (interaction->user)
        disco_destroy_user(interaction->user);
    if (interaction->member)
        disco_destroy_member(interaction->member);
    if (interaction->token)
        free(interaction->token);
    if (interaction->message)
        disco_destroy_message(interaction->message);
    if (interaction->app_permissions)
        free(interaction->app_permissions);
    if (interaction->locale)
        free(interaction->locale);
    if (interaction->guild_locale)
        free(interaction->guild_locale);
    free(interaction);
}

void disco_send_interaction(struct discord_interaction_callback *cb, struct discord_interaction *recv) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "type", (double)cb->type);

    cJSON *data = cJSON_AddObjectToObject(json, "data"), *array;
    struct discord_create_message msg = {0};
    switch (cb->type) {
    case DISCORD_CALLBACK_CHANNEL_MESSAGE_WITH_SOURCE:
    case DISCORD_CALLBACK_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE:
    case DISCORD_CALLBACK_DEFERRED_UPDATE_MESSAGE:
    case DISCORD_CALLBACK_UPDATE_MESSAGE:
        // we turn the interaction data into a message to fill in the json
        msg.tts = cb->data.message.tts;
        msg.embeds = cb->data.message.embeds;
        msg.embeds_count = cb->data.message.embeds_count;
        msg.allowed_mentions = cb->data.message.allowed_mentions;
        msg.flags = cb->data.message.flags;
        msg.components = cb->data.message.components;
        msg.components_count = cb->data.message.components_count;
        msg.attachments = cb->data.message.attachments;
        msg.attachments_count = cb->data.message.attachments_count;
        discord_fill_json_with_message(data, cb->data.message.content, &msg);
        break;

    case DISCORD_CALLBACK_APPLICATION_COMMAND_AUTOCOMPLETE_RESULT:
        if (cb->data.autocomplete.choices_count > 0)
            array = cJSON_AddArrayToObject(data, "choices");
        for (int i = 0; i < cb->data.autocomplete.choices_count; i++) {
            cJSON *choice = cJSON_CreateObject();
            cJSON_AddItemToArray(array, choice);
            cJSON_AddStringToObject(choice, "name", cb->data.autocomplete.choices[i]->name);
            if (cb->data.autocomplete.choices[i]->name_localizations) {
                cJSON *locales = cJSON_AddObjectToObject(choice, "name_localizations");
                discord_fill_json_with_locales(locales, cb->data.autocomplete.choices[i]->name_localizations);
            }
            switch (recv->data->options[i]->type) {
            case COMMAND_OPTION_INTEGER:
            case COMMAND_OPTION_NUMBER:
                cJSON_AddNumberToObject(choice, "value", cb->data.autocomplete.choices[i]->value.number);
                break;
            case COMMAND_OPTION_STRING:
                cJSON_AddStringToObject(choice, "value", cb->data.autocomplete.choices[i]->value.str);
                break;
            default: // simply ignore
                break;
            }
        }
        break;

    case DISCORD_CALLBACK_MODAL:
        cJSON_AddStringToObject(data, "custom_id", cb->data.modal.custom_id);
        cJSON_AddStringToObject(data, "title", cb->data.modal.title);
        if (cb->data.modal.components_count > 0) {
            cJSON *components = cJSON_AddArrayToObject(data, "components");
            for (int i = 0; i < cb->data.modal.components_count; i++) {
                cJSON *comp = cJSON_CreateObject();
                cJSON_AddItemToArray(components, comp);
                discord_fill_json_with_component(comp, cb->data.modal.components[i]);
            }
        }
        break;

    case DISCORD_CALLBACK_PONG: // ignore PONG
        break;
    }

    // send the payload
    // there's no documented max token length
    size_t token_len = strnlen(recv->token, 500);
    char *uri = (char *)malloc(80 + token_len);
    sprintf(uri, "https://discord.com/api/v10/interactions/%ld/%s/callback", recv->id, recv->token);
    char *response;
    CURLcode res = request(uri, &response, json, REQUEST_POST);
    if (res != CURLE_OK) {
        d_log_err("%d: POST failed: %s\n", res, curl_easy_strerror(res));
        if (res == CURLE_COULDNT_RESOLVE_HOST)
            d_log_err("Have no connection to host\n");
    } else {
        d_log_debug("Interaction sent!\n");
        d_log_debug("Response: char = %s\n", response);
    }

    // free up allocated stuff
    cJSON_Delete(json);
    free(uri);
    free(response);
}
