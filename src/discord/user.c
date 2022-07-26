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
    tmp = cJSON_GetObjectItem(data, "roles");
    mem->roles_count = cJSON_GetArraySize(tmp);
    if (mem->roles_count > 0) {
        mem->roles = (uint64_t *)malloc((size_t)mem->roles_count * sizeof(uint64_t));
        int i = 0;
        cJSON *cur = NULL;
        cJSON_ArrayForEach(cur, tmp) {
            mem->roles[i++] = (uint64_t)strtoull(cur->valuestring, NULL, 10);
        }
    }
    mem->joined_at = get_string_from_json(data, "joined_at");
    mem->premium_since = get_string_from_json(data, "premium_since");
    mem->deaf = get_bool_from_json(data, "deaf", 0);
    mem->mute = get_bool_from_json(data, "mute", 0);
    mem->pending = get_bool_from_json(data, "pending", 0);
    mem->permissions = get_string_from_json(data, "permissions");
    mem->communication_disabled_until = get_string_from_json(data, "communication_disabled_until");

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
