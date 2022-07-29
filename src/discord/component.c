#include <discord/component.h>
#include <discord/disco.h>
#include <discord/emoji.h>

void *_d_json_to_component(cJSON *data) {
    struct discord_component *comp = (struct discord_component *)calloc(1, sizeof(struct discord_component));
    cJSON *tmp = NULL;
    comp->type = _d_get_int_from_json(data, "type", 0);
    switch (comp->type) {
    case COMPONENT_ACTION_ROW:
        comp->comp.action_row = (struct discord_action_row *)malloc(sizeof(struct discord_action_row));
        comp->comp.action_row->components_count = _d_get_array_from_json(
            data,
            "components",
            (void ***)&comp->comp.action_row->components,
            sizeof(struct discord_component),
            &_d_json_to_component);
        break;
    case COMPONENT_BUTTON:
        comp->comp.button = (struct discord_button *)calloc(1, sizeof(struct discord_button));
        comp->comp.button->style = (enum Discord_Button_Style)_d_get_int_from_json(data, "style", 1);
        comp->comp.button->label = _d_get_string_from_json(data, "label");
        tmp = cJSON_GetObjectItem(data, "emoji");
        if (tmp)
            comp->comp.button->emoji = _d_json_to_emoji(tmp);
        comp->comp.button->custom_id = _d_get_string_from_json(data, "custom_id");
        comp->comp.button->url = _d_get_string_from_json(data, "url");
        comp->comp.button->disabled = _d_get_bool_from_json(data, "disabled", 0);
        break;
    case COMPONENT_SELECT_MENU:
        comp->comp.select_menu = (struct discord_select_menu *)calloc(1, sizeof(struct discord_select_menu));
        comp->comp.select_menu->custom_id = _d_get_string_from_json(data, "custom_id");
        comp->comp.select_menu->options_count = _d_get_array_from_json(
            data,
            "options",
            (void ***)&comp->comp.select_menu->options,
            sizeof(struct discord_select_option),
            &_d_json_to_select_option);
        comp->comp.select_menu->placeholder = _d_get_string_from_json(data, "placeholder");
        comp->comp.select_menu->min_values = _d_get_int_from_json(data, "min_values", 1);
        comp->comp.select_menu->max_values = _d_get_int_from_json(data, "max_values", 1);
        comp->comp.select_menu->disabled = _d_get_bool_from_json(data, "disabled", 0);
        break;
    case COMPONENT_TEXT_INPUT:
        comp->comp.text_input = (struct discord_text_input *)calloc(1, sizeof(struct discord_text_input));
        comp->comp.text_input->custom_id = _d_get_string_from_json(data, "custom_id");
        comp->comp.text_input->style = (enum Discord_Text_Input_Style)_d_get_int_from_json(data, "style", 1);
        comp->comp.text_input->label = _d_get_string_from_json(data, "label");
        comp->comp.text_input->min_length = _d_get_int_from_json(data, "min_length", 0);
        comp->comp.text_input->max_length = _d_get_int_from_json(data, "max_length", 1);
        comp->comp.text_input->required = _d_get_bool_from_json(data, "required", 0);
        comp->comp.text_input->value = _d_get_string_from_json(data, "value");
        comp->comp.text_input->placeholder = _d_get_string_from_json(data, "placeholder");
        break;
    default:
        break;
    }
    return comp;
}

struct discord_component *_d_copy_component(struct discord_component *src) {
    if (!src)
        return NULL;
    struct discord_component *c = (struct discord_component *)malloc(sizeof(struct discord_component));
    c->type = src->type;
    switch (c->type) {
    case COMPONENT_ACTION_ROW:
        c->comp.action_row = (struct discord_action_row *)malloc(sizeof(struct discord_action_row));
        c->comp.action_row->components_count = src->comp.action_row->components_count;
        if (c->comp.action_row->components_count > 0) {
            c->comp.action_row->components = (struct discord_component **)malloc(c->comp.action_row->components_count * sizeof(struct discord_component *));
            for (int i = 0; i < c->comp.action_row->components_count; i++)
                c->comp.action_row->components[i] = _d_copy_component(src->comp.action_row->components[i]);
        }
        break;
    case COMPONENT_BUTTON:
        c->comp.button = (struct discord_button *)malloc(sizeof(struct discord_button));
        c->comp.button->style = src->comp.button->style;
        if (src->comp.button->label)
            c->comp.button->label = strndup(src->comp.button->label, 101);
        c->comp.button->emoji = _d_copy_emoji(src->comp.button->emoji);
        if (src->comp.button->custom_id)
            c->comp.button->custom_id = strndup(src->comp.button->custom_id, 101);
        if (src->comp.button->url)
            c->comp.button->url = strndup(src->comp.button->url, 2050);
        c->comp.button->disabled = src->comp.button->disabled;
        break;
    case COMPONENT_SELECT_MENU:
        c->comp.select_menu = (struct discord_select_menu *)malloc(sizeof(struct discord_select_menu));
        memcpy(c->comp.select_menu, src->comp.select_menu, sizeof(struct discord_select_menu));
        if (src->comp.select_menu->custom_id)
            c->comp.select_menu->custom_id = strndup(src->comp.select_menu->custom_id, 101);
        if (c->comp.select_menu->options_count > 0) {
            c->comp.select_menu->options = (struct discord_select_option **)malloc(c->comp.select_menu->options_count * sizeof(struct discord_select_option *));
            for (int i = 0; i < c->comp.select_menu->options_count; i++)
                c->comp.select_menu->options[i] = _d_copy_select_option(src->comp.select_menu->options[i]);
        }
        if (src->comp.select_menu->placeholder)
            c->comp.select_menu->placeholder = strndup(src->comp.select_menu->placeholder, 151);
        break;
    case COMPONENT_TEXT_INPUT:
        c->comp.text_input = (struct discord_text_input *)malloc(sizeof(struct discord_text_input));
        memcpy(c->comp.select_menu, src->comp.select_menu, sizeof(struct discord_text_input));
        if (src->comp.text_input->custom_id)
            c->comp.text_input->custom_id = strndup(src->comp.text_input->custom_id, 101);
        if (src->comp.text_input->label)
            c->comp.text_input->label = strndup(src->comp.text_input->label, 50);
        if (src->comp.text_input->value)
            c->comp.text_input->value = strndup(src->comp.text_input->value, 4001);
        if (src->comp.text_input->placeholder)
            c->comp.text_input->placeholder = strndup(src->comp.text_input->placeholder, 101);
        break;
    default:
        break;
    }
    return c;
}

void discord_destroy_component(struct discord_component *c) {
    switch (c->type) {
    case COMPONENT_ACTION_ROW:
        for (int i = 0; i < c->comp.action_row->components_count; i++)
            discord_destroy_component(c->comp.action_row->components[i]);
        if (c->comp.action_row->components)
            free(c->comp.action_row->components);
        break;
    case COMPONENT_BUTTON:
        if (c->comp.button->label)
            free(c->comp.button->label);
        if (c->comp.button->emoji)
            discord_destroy_emoji(c->comp.button->emoji);
        if (c->comp.button->custom_id)
            free(c->comp.button->custom_id);
        if (c->comp.button->url)
            free(c->comp.button->url);
        break;
    case COMPONENT_SELECT_MENU:
        if (c->comp.select_menu->custom_id)
            free(c->comp.select_menu->custom_id);
        for (int i = 0; i < c->comp.select_menu->options_count; i++)
            discord_destroy_select_option(c->comp.select_menu->options[i]);
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

void *_d_json_to_select_option(cJSON *data) {
    struct discord_select_option *option = (struct discord_select_option *)calloc(1, sizeof(struct discord_select_option));
    option->label = _d_get_string_from_json(data, "label");
    option->value = _d_get_string_from_json(data, "value");
    option->description = _d_get_string_from_json(data, "description");
    cJSON *tmp = cJSON_GetObjectItem(data, "emoji");
    if (tmp)
        option->emoji = _d_json_to_emoji(tmp);
    option->_default = _d_get_bool_from_json(data, "default", 0);
    return option;
}

struct discord_select_option *_d_copy_select_option(struct discord_select_option *src) {
    if (!src)
        return NULL;
    struct discord_select_option *c = (struct discord_select_option *)malloc(sizeof(struct discord_select_option));
    if (src->label)
        c->label = strndup(src->label, 101);
    if (src->value)
        c->value = strndup(src->value, 101);
    if (src->description)
        c->description = strndup(src->description, 101);
    c->emoji = _d_copy_emoji(src->emoji);
    c->_default = src->_default;
    return c;
}

void discord_destroy_select_option(struct discord_select_option *option) {
    char *to_free[3] = {option->label, option->value, option->description};
    for (int i = 0; i < 3; i++)
        if (to_free[i])
            free(to_free[i]);
    if (option->emoji)
        discord_destroy_emoji(option->emoji);
    free(option);
}

// TODO implement
void _d_component_to_json(cJSON *json, struct discord_component *c) {
    (void)json;
    (void)c;
}
