#include "structures/component.h"
#include "disco.h"
#include "structures/emoji.h"

void *disco_create_component_struct_json(cJSON *data) {
    struct discord_component *comp = (struct discord_component *)calloc(1, sizeof(struct discord_component));
    cJSON *tmp = NULL;
    comp->type = get_int_from_json(data, "type", 0);
    switch (comp->type) {
    case COMPONENT_ACTION_ROW:
        comp->comp.action_row = (struct discord_action_row *)malloc(sizeof(struct discord_action_row));
        comp->comp.action_row->components_count = get_array_from_json(
            data,
            "components",
            (void ***)&comp->comp.action_row->components,
            sizeof(struct discord_component),
            &disco_create_component_struct_json);
        break;
    case COMPONENT_BUTTON:
        comp->comp.button = (struct discord_button *)calloc(1, sizeof(struct discord_button));
        comp->comp.button->style = (enum Discord_Button_Style)get_int_from_json(data, "style", 1);
        comp->comp.button->label = get_string_from_json(data, "label");
        tmp = cJSON_GetObjectItem(data, "emoji");
        if (tmp)
            comp->comp.button->emoji = disco_create_emoji_struct_json(tmp);
        comp->comp.button->custom_id = get_string_from_json(data, "custom_id");
        comp->comp.button->url = get_string_from_json(data, "url");
        comp->comp.button->disabled = get_bool_from_json(data, "disabled", 0);
        break;
    case COMPONENT_SELECT_MENU:
        comp->comp.select_menu = (struct discord_select_menu *)calloc(1, sizeof(struct discord_select_menu));
        comp->comp.select_menu->custom_id = get_string_from_json(data, "custom_id");
        comp->comp.select_menu->options_count = get_array_from_json(
            data,
            "options",
            (void ***)&comp->comp.select_menu->options,
            sizeof(struct discord_select_option),
            &disco_create_select_option_struct);
        comp->comp.select_menu->placeholder = get_string_from_json(data, "placeholder");
        comp->comp.select_menu->min_values = get_int_from_json(data, "min_values", 1);
        comp->comp.select_menu->max_values = get_int_from_json(data, "max_values", 1);
        comp->comp.select_menu->disabled = get_bool_from_json(data, "disabled", 0);
        break;
    case COMPONENT_TEXT_INPUT:
        comp->comp.text_input = (struct discord_text_input *)calloc(1, sizeof(struct discord_text_input));
        comp->comp.text_input->custom_id = get_string_from_json(data, "custom_id");
        comp->comp.text_input->style = (enum Discord_Text_Input_Style)get_int_from_json(data, "style", 1);
        comp->comp.text_input->label = get_string_from_json(data, "label");
        comp->comp.text_input->min_length = get_int_from_json(data, "min_length", 0);
        comp->comp.text_input->max_length = get_int_from_json(data, "max_length", 1);
        comp->comp.text_input->required = get_bool_from_json(data, "required", 0);
        comp->comp.text_input->value = get_string_from_json(data, "value");
        comp->comp.text_input->placeholder = get_string_from_json(data, "placeholder");
        break;
    default:
        return NULL;
    }
    return comp;
}
void disco_destroy_component(struct discord_component *c) {
    switch (c->type) {
    case COMPONENT_ACTION_ROW:
        for (int i = 0; i < c->comp.action_row->components_count; i++)
            disco_destroy_component(c->comp.action_row->components[i]);
        if (c->comp.action_row->components)
            free(c->comp.action_row->components);
        break;
    case COMPONENT_BUTTON:
        if (c->comp.button->label)
            free(c->comp.button->label);
        if (c->comp.button->emoji)
            disco_destroy_emoji(c->comp.button->emoji);
        if (c->comp.button->custom_id)
            free(c->comp.button->custom_id);
        if (c->comp.button->url)
            free(c->comp.button->url);
        break;
    case COMPONENT_SELECT_MENU:
        if (c->comp.select_menu->custom_id)
            free(c->comp.select_menu->custom_id);
        for (int i = 0; i < c->comp.select_menu->options_count; i++)
            disco_destroy_select_option(c->comp.select_menu->options[i]);
        if (c->comp.select_menu->options)
            free(c->comp.select_menu->options);
        if (c->comp.select_menu->placeholder)
            free(c->comp.select_menu->placeholder);
        break;
    case COMPONENT_TEXT_INPUT:
        if (c->comp.text_input->custom_id)
            free(c->comp.text_input->custom_id);
        if (c->comp.text_input->label)
            free(c->comp.text_input->label);
        if (c->comp.text_input->value)
            free(c->comp.text_input->value);
        if (c->comp.text_input->placeholder)
            free(c->comp.text_input->placeholder);
        break;
    default:
        break;
    }
    free(c);
}

void *disco_create_select_option_struct(cJSON *data) {
    struct discord_select_option *option = (struct discord_select_option *)calloc(1, sizeof(struct discord_select_option));
    option->label = get_string_from_json(data, "label");
    option->value = get_string_from_json(data, "value");
    option->description = get_string_from_json(data, "description");
    cJSON *tmp = cJSON_GetObjectItem(data, "emoji");
    if (tmp)
        option->emoji = disco_create_emoji_struct_json(tmp);
    option->_default = get_bool_from_json(data, "default", 0);
    return option;
}
void disco_destroy_select_option(struct discord_select_option *option) {
    char *to_free[3] = {option->label, option->value, option->description};
    for (int i = 0; i < 3; i++)
        if (to_free[i])
            free(to_free[i]);
    if (option->emoji)
        disco_destroy_emoji(option->emoji);
    free(option);
}

// TODO implement
void discord_fill_json_with_component(cJSON *json, struct discord_component *c) {
    (void)json;
    (void)c;
}
