#include <cJSON/cJSON.h>
#include <discord/disco.h>
#include <discord/user.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *_d_json_to_user(cJSON *data) {
    struct discord_user *user = (struct discord_user *)calloc(1, sizeof(struct discord_user));
    user->id = _d_get_long_from_string_json(data, "id", 0);
    user->username = _d_get_string_from_json(data, "username");
    user->discriminator = _d_get_string_from_json(data, "discriminator");
    user->avatar = _d_get_string_from_json(data, "avatar");
    user->bot = _d_get_bool_from_json(data, "bot", 0);
    user->system = _d_get_bool_from_json(data, "system", 0);
    user->mfa_enabled = _d_get_bool_from_json(data, "mfa_enabled", 0);
    user->banner = _d_get_string_from_json(data, "banner");
    user->accent_color = _d_get_int_from_json(data, "accent_color", 0);
    user->locale = _d_get_string_from_json(data, "locale");
    user->verified = _d_get_bool_from_json(data, "verified", 0);
    user->email = _d_get_string_from_json(data, "email");
    user->flags = _d_get_int_from_json(data, "flags", 0);
    user->premium_type = _d_get_int_from_json(data, "premium_type", 0);
    user->public_flags = _d_get_int_from_json(data, "public_flags", 0);
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

void *_d_json_to_member(cJSON *data, struct discord_user *user) {
    struct discord_member *mem = (struct discord_member *)calloc(1, sizeof(struct discord_member));
    cJSON *tmp;
    if (user) // we already have a user struct
        mem->user = user;
    else { // user information might be in the member json
        tmp = cJSON_GetObjectItem(data, "user");
        if (tmp)
            mem->user = _d_json_to_user(tmp);
    }
    mem->nick = _d_get_string_from_json(data, "nick");
    mem->avatar = _d_get_string_from_json(data, "avatar");
    tmp = cJSON_GetObjectItem(data, "roles");
    mem->roles_count = cJSON_GetArraySize(tmp);
    if (mem->roles_count > 0) {
        mem->roles = (uint64_t *)malloc((size_t)mem->roles_count * sizeof(uint64_t));
        int i = 0;
        cJSON *cur = NULL;
        cJSON_ArrayForEach(cur, tmp) {
            mem->roles[i++] = (uint64_t)strtoll(cur->valuestring, NULL, 10);
        }
    }
    mem->joined_at = _d_get_string_from_json(data, "joined_at");
    mem->premium_since = _d_get_string_from_json(data, "premium_since");
    mem->deaf = _d_get_bool_from_json(data, "deaf", 0);
    mem->mute = _d_get_bool_from_json(data, "mute", 0);
    mem->pending = _d_get_bool_from_json(data, "pending", 0);
    mem->permissions = _d_get_string_from_json(data, "permissions");
    mem->communication_disabled_until = _d_get_string_from_json(data, "communication_disabled_until");

    return mem;
}

void discord_destroy_member(struct discord_member *mem) {
    char *strings[6] = {
        mem->nick,
        mem->avatar,
        mem->joined_at,
        mem->premium_since,
        mem->permissions,
        mem->communication_disabled_until,
    };
    for (int i = 0; i < 6; i++) {
        if (strings[i])
            free(strings[i]);
    }
    if (mem->roles)
        free(mem->roles);
    if (mem->user)
        discord_destroy_user(mem->user);
    free(mem);
}
