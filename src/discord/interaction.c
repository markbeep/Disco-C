#include <discord/disco.h>
#include <discord/interaction.h>
#include <stdlib.h>
#include <utils/disco_logging.h>
#include <web/request.h>

void *_d_json_to_interaction_data_option(cJSON *data) {
    struct discord_interaction_data_option *opt = (struct discord_interaction_data_option *)calloc(1, sizeof(struct discord_interaction_data_option));
    opt->name = _d_get_string_from_json(data, "name");
    opt->type = (enum Discord_Application_Command_Option_Type)_d_get_int_from_json(data, "type", 0);
    if (opt->type == COMMAND_OPTION_STRING)
        opt->value.string_ = _d_get_string_from_json(data, "value");
    else if (opt->type == COMMAND_OPTION_INTEGER)
        opt->value.int_ = _d_get_int_from_json(data, "value", 0);
    else if (opt->type == COMMAND_OPTION_NUMBER)
        opt->value.double_ = _d_get_double_from_string_json(data, "value", 0.);
    opt->options_count = _d_get_array_from_json(data, "options", (void ***)&opt->options, sizeof(struct discord_interaction_data_option), &_d_json_to_interaction_data_option);
    opt->focused = _d_get_bool_from_json(data, "focused", 0);
    return opt;
}

struct discord_interaction_data_option *_d_copy_interaction_data_option(struct discord_interaction_data_option *src) {
    if (!src)
        return NULL;
    struct discord_interaction_data_option *c = (struct discord_interaction_data_option *)calloc(1, sizeof(struct discord_interaction_data_option));
    memcpy(c, src, sizeof(struct discord_interaction_data_option));
    if (src->name)
        c->name = strndup(src->name, 50);
    if (c->value_type == VALUE_STRING && src->value.string_)
        c->value.string_ = strndup(src->value.string_, 120);
    if (c->options_count > 0) {
        c->options = (struct discord_interaction_data_option **)malloc(c->options_count * sizeof(struct discord_interaction_data_option *));
        for (int i = 0; i < c->options_count; i++) {
            c->options[i] = _d_copy_interaction_data_option(src->options[i]);
        }
    }
    return c;
}

void discord_destroy_interaction_data_option(struct discord_interaction_data_option *o) {
    if (o->name)
        free(o->name);
    if (o->type == COMMAND_OPTION_STRING && o->value.string_)
        free(o->value.string_);
    for (int i = 0; i < o->options_count; i++)
        discord_destroy_interaction_data_option(o->options[i]);
    free(o->options);
    free(o);
}

void *_d_json_to_interaction_data(cJSON *data) {
    struct discord_interaction_data *d = (struct discord_interaction_data *)calloc(1, sizeof(struct discord_interaction_data));
    d->id = _d_get_long_from_string_json(data, "id", 0);
    d->name = _d_get_string_from_json(data, "name");
    d->type = (enum Discord_Application_Command_Type)_d_get_int_from_json(data, "type", 1);
    d->options_count = _d_get_array_from_json(data, "options", (void ***)&d->options, sizeof(struct discord_interaction_data_option), &_d_json_to_interaction_data_option);
    d->guild_id = _d_get_long_from_string_json(data, "guild_id", 0);
    d->target_id = _d_get_long_from_string_json(data, "target_id", 0);

    d->custom_id = _d_get_string_from_json(data, "custom_id");                                                                                           // message/modal
    d->values_count = _d_get_array_from_json(data, "values", (void ***)&d->values, sizeof(struct discord_select_option), &_d_json_to_select_option);     // message
    d->components_count = _d_get_array_from_json(data, "components", (void ***)&d->components, sizeof(struct discord_component), &_d_json_to_component); // modal
    return d;
}

struct discord_interaction_data *_d_copy_interaction_data(struct discord_interaction_data *src) {
    if (!src)
        return NULL;
    struct discord_interaction_data *c = (struct discord_interaction_data *)calloc(1, sizeof(struct discord_interaction_data));
    memcpy(c, src, sizeof(struct discord_interaction_data));
    if (src->name)
        c->name = strndup(src->name, 100);
    if (c->options_count > 0) {
        c->options = (struct discord_interaction_data_option **)malloc(c->options_count * sizeof(struct discord_interaction_data_option *));
        for (int i = 0; i < c->options_count; i++) {
            c->options[i] = _d_copy_interaction_data_option(src->options[i]);
        }
    }
    if (src->custom_id)
        c->custom_id = strndup(src->custom_id, 50);
    if (c->values_count > 0) {
        c->values = (struct discord_interaction_data_option **)malloc(c->values_count * sizeof(struct discord_interaction_data_option *));
        for (int i = 0; i < c->values_count; i++) {
            c->values[i] = _d_copy_interaction_data_option(src->values[i]);
        }
    }
    if (c->components_count > 0) {
        c->components = (struct discord_component **)malloc(c->components_count * sizeof(struct discord_component *));
        for (int i = 0; i < c->components_count; i++) {
            c->components[i] = _d_copy_component(src->components[i]);
        }
    }
    return c;
}

void discord_destroy_interaction_data(struct discord_interaction_data *interaction) {
    if (interaction->name)
        free(interaction->name);
    for (int i = 0; i < interaction->options_count; i++)
        discord_destroy_interaction_data_option(interaction->options[i]);
    free(interaction->options);

    if (interaction->custom_id)
        free(interaction->custom_id);                                    // message/modal
    for (int i = 0; i < interaction->values_count; i++)                  // message
        discord_destroy_interaction_data_option(interaction->values[i]); // message
    free(interaction->values);                                           // message
    for (int i = 0; i < interaction->components_count; i++)              // modal
        discord_destroy_component(interaction->components[i]);           // modal
    free(interaction->components);                                       // modal
    free(interaction);
}

void *_d_json_to_interaction(cJSON *data) {
    cJSON *tmp;
    struct discord_interaction *inter = (struct discord_interaction *)calloc(1, sizeof(struct discord_interaction));
    inter->id = _d_get_long_from_string_json(data, "id", 0);
    inter->application_id = _d_get_long_from_string_json(data, "application_id", 0);
    inter->type = (enum Discord_Interaction_Type)_d_get_int_from_json(data, "type", 1);
    tmp = cJSON_GetObjectItem(data, "data");
    if (tmp)
        inter->data = _d_json_to_interaction_data(tmp);
    inter->guild_id = _d_get_long_from_string_json(data, "guild_id", 0);
    inter->channel_id = _d_get_long_from_string_json(data, "channel_id", 0);
    tmp = cJSON_GetObjectItem(data, "user");
    if (tmp)
        inter->user = _d_json_to_user(tmp);
    tmp = cJSON_GetObjectItem(data, "member");
    if (tmp)
        inter->member = _d_json_to_member(tmp, inter->user);
    inter->token = _d_get_string_from_json(data, "token");
    inter->version = _d_get_int_from_json(data, "version", 0);
    tmp = cJSON_GetObjectItem(data, "message");
    if (tmp)
        inter->message = _d_json_to_message(tmp);
    inter->app_permissions = _d_get_string_from_json(data, "app_permissions");
    inter->locale = _d_get_string_from_json(data, "locale");
    inter->guild_locale = _d_get_string_from_json(data, "guild_locale");

    return inter;
}

struct discord_interaction *_d_copy_interaction(struct discord_interaction *src) {
    if (!src)
        return NULL;
    struct discord_interaction *c = (struct discord_interaction *)calloc(1, sizeof(struct discord_interaction));
    memcpy(c, src, sizeof(struct discord_interaction));
    c->data = _d_copy_interaction_data(src->data);
    c->user = _d_copy_user(src->user);
    c->member = _d_copy_member(src->member, c->user);
    if (src->token)
        c->token = strndup(src->token, 100);
    c->message = _d_copy_message(src->message);
    if (src->app_permissions)
        c->app_permissions = strndup(src->app_permissions, 50);
    if (src->locale)
        c->locale = strndup(src->locale, 10);
    if (src->guild_locale)
        c->guild_locale = strndup(src->guild_locale, 10);
    return c;
}

void discord_destroy_interaction(struct discord_interaction *interaction) {
    if (interaction->data)
        discord_destroy_interaction_data(interaction->data);
    if (interaction->member)
        discord_destroy_member(interaction->member);
    else if (interaction->user) // only delete solely user if there's no member
        discord_destroy_user(interaction->user);
    if (interaction->token)
        free(interaction->token);
    if (interaction->message)
        discord_destroy_message(interaction->message);
    if (interaction->app_permissions)
        free(interaction->app_permissions);
    if (interaction->locale)
        free(interaction->locale);
    if (interaction->guild_locale)
        free(interaction->guild_locale);
    free(interaction);
}

void discord_send_interaction(bot_client_t *bot, struct discord_interaction_callback *cb, struct discord_interaction *recv) {
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
        _d_create_message_to_json(data, cb->data.message.content, &msg);
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
                _d_component_to_json(comp, cb->data.modal.components[i]);
            }
        }
        break;

    case DISCORD_CALLBACK_PONG: // ignore PONG
        break;
    default:
        break;
    }

    // send the payload
    // there's no documented max token length
    size_t token_len = strnlen(recv->token, 500);
    char *uri = (char *)malloc(80 + token_len);
    sprintf(uri, "https://discord.com/api/v10/interactions/%ju/%s/callback", recv->id, recv->token);
    request(uri, json, REQUEST_POST, bot->websocket_client->token, NULL, NULL);

    // free up allocated stuff
    cJSON_Delete(json);
    free(uri);
}
