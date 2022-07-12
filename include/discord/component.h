#ifndef DISCO_COMPONENT
#define DISCO_COMPONENT

#include "emoji.h"
#include "user.h"
#include <cJSON/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * \addtogroup Component
 * Component functions and structures
 * @{
 *
 */

struct discord_component;

enum Discord_Component_Type { COMPONENT_ACTION_ROW = 1,
                              COMPONENT_BUTTON = 2,
                              COMPONENT_SELECT_MENU = 3,
                              COMPONENT_TEXT_INPUT = 4 };

// https://discord.com/developers/docs/interactions/message-components#action-rows
struct discord_action_row {
    struct discord_component **components;
    int components_count;
};

enum Discord_Button_Style { BUTTON_PRIMARY = 1,
                            BUTTON_SECONDARY = 2,
                            BUTTON_SUCCESS = 3,
                            BUTTON_DANGER = 4,
                            BUTTON_LINK = 5 };

// https://discord.com/developers/docs/interactions/message-components#button-object
struct discord_button {
    enum Discord_Button_Style style;
    char *label;
    struct discord_emoji *emoji;
    char *custom_id;
    char *url;
    bool disabled;
};

// https://discord.com/developers/docs/interactions/message-components#select-menu-object-select-option-structure
struct discord_select_option {
    char *label;
    char *value;
    char *description;
    struct discord_emoji *emoji;
    bool _default; // "default"
};

/**
 * @brief Creates a select option structure from a given JSON
 *
 * @param data
 * @return void* discord_select_option struct
 */
void *discord_create_select_option_struct(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param option
 */
void discord_destroy_select_option(struct discord_select_option *option);

// https://discord.com/developers/docs/interactions/message-components#select-menu-object
struct discord_select_menu {
    char *custom_id;
    struct discord_select_option **options;
    int options_count;
    char *placeholder;
    int min_values;
    int max_values;
    bool disabled;
};

enum Discord_Text_Input_Style { TEXT_INPUT_SHORT = 1,
                                TEXT_INPUT_PARAGRAPH = 2 };

// https://discord.com/developers/docs/interactions/message-components#text-inputs-text-input-structure
struct discord_text_input {
    char *custom_id;
    enum Discord_Text_Input_Style style;
    char *label;
    int min_length;
    int max_length;
    bool required;
    char *value;
    char *placeholder;
};

// https://discord.com/developers/docs/interactions/message-components#component-object
struct discord_component {
    enum Discord_Component_Type type;
    union {
        struct discord_action_row *action_row;
        struct discord_button *button;
        struct discord_select_menu *select_menu;
        struct discord_text_input *text_input;
    } comp;
};

/**
 * @brief Creates a component structure from a given JSON with the correct type
 *
 * @param data
 * @return void* discord_component struct
 */
void *discord_create_component_struct_json(cJSON *data);
/**
 * @brief Destroys the given structure and frees the pointer
 *
 * @param c discord_component struct
 */
void discord_destroy_component(struct discord_component *c);

/**
 * @brief Fills in a JSON with the attributes of the component structure.
 *
 * @param json JSON struct that should be filled in with the values.
 * @param component The component that will be used to fill the JSON.
 */
void discord_fill_json_with_component(cJSON *json, struct discord_component *component);

/** @}*/
#endif
