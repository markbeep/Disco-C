#include "structures/user.h"
#include "../utils/cJSON.h"
#include "disco.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *disco_create_user_struct_json(cJSON *data) {
    struct discord_user *user = (struct discord_user *)calloc(1, sizeof(struct discord_user));
    user->id = get_string_from_json(data, "id");
    user->username = get_string_from_json(data, "username");
    user->discriminator = get_string_from_json(data, "discriminator");
    user->avatar = get_string_from_json(data, "avatar");
    // for the booleans we can check if they're greater than 0
    // -1 would mean we turn to the default value, but these are false by default
    user->bot = get_bool_from_json(data, "bot") > 0;
    user->system = get_bool_from_json(data, "system") > 0;
    user->mfa_enabled = get_bool_from_json(data, "mfa_enabled") > 0;
    user->banner = get_string_from_json(data, "banner");
    user->accent_color = get_int_from_json(data, "accent_color");
    user->locale = get_string_from_json(data, "locale");
    user->verified = get_bool_from_json(data, "verified");
    user->email = get_string_from_json(data, "email");
    user->flags = get_int_from_json(data, "flags");
    user->premium_type = get_int_from_json(data, "premium_type");
    user->public_flags = get_int_from_json(data, "public_flags");
    return user;
}

void disco_destroy_user(struct discord_user *user) {
    if (user->id)
        free(user->id);
    if (user->username)
        free(user->username);
    if (user->discriminator)
        free(user->avatar);
    if (user->avatar)
        free(user->avatar);
    if (user->banner)
        free(user->banner);
    if (user->locale)
        free(user->locale);
    if (user->email)
        free(user->email);
    free(user);
}

// TODO implement
void *disco_create_member_struct_json(cJSON *data, struct discord_user *user) {
    (void)data;
    (void)user;
    return NULL;
}

void disco_destroy_member(struct discord_user *member) {
    (void)member;
}