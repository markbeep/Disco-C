#include <cJSON/cJSON.h>
#include <discord/disco.h>
#include <discord/user.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *discord_create_user_struct_json(cJSON *data) {
    struct discord_user *user = (struct discord_user *)calloc(1, sizeof(struct discord_user));
    user->id = get_long_from_string_json(data, "id", 0);
    user->username = get_string_from_json(data, "username");
    user->discriminator = get_string_from_json(data, "discriminator");
    user->avatar = get_string_from_json(data, "avatar");
    user->bot = get_bool_from_json(data, "bot", 0);
    user->system = get_bool_from_json(data, "system", 0);
    user->mfa_enabled = get_bool_from_json(data, "mfa_enabled", 0);
    user->banner = get_string_from_json(data, "banner");
    user->accent_color = get_int_from_json(data, "accent_color", 0);
    user->locale = get_string_from_json(data, "locale");
    user->verified = get_bool_from_json(data, "verified", 0);
    user->email = get_string_from_json(data, "email");
    user->flags = get_int_from_json(data, "flags", 0);
    user->premium_type = get_int_from_json(data, "premium_type", 0);
    user->public_flags = get_int_from_json(data, "public_flags", 0);
    return user;
}

void discord_destroy_user(struct discord_user *user) {
    if (user->username)
        free(user->username);
    if (user->discriminator)
        free(user->discriminator);
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
void *discord_create_member_struct_json(cJSON *data, struct discord_user *user) {
    struct discord_member *mem = (struct discord_member *)calloc(1, sizeof(struct discord_member));
    cJSON *tmp;
    if (user) // we already have a user struct
        mem->user = user;
    else { // user information might be in the member json
        tmp = cJSON_GetObjectItem(data, "user");
        if (tmp)
            mem->user = discord_create_user_struct_json(tmp);
    }
    mem->nick = get_string_from_json(data, "nick");
    mem->avatar = get_string_from_json(data, "avatar");
    // TODO roles and rest of implementation

    return mem;
}

void discord_destroy_member(struct discord_member *member) {
    (void)member;
}
