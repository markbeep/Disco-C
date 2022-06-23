#ifndef DISCO_APPLICATION
#define DISCO_APPLICATION

#include "user.h"
#include <cJSON/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

// https://discord.com/developers/docs/topics/teams#data-models-team-member-object
struct discord_team_member {
    int membership_state;
    char **permissions;
    int permissions_count;
    int64_t team_id;
    struct discord_user *user;
};

// https://discord.com/developers/docs/topics/teams#data-models-team-object
struct discord_team {
    char *icon;
    int64_t id;
    struct discord_team_member **members;
    int members_count;
    char *name;
    int64_t owner_user_id;
};

// https://discord.com/developers/docs/resources/application#install-params-object
struct discord_params {
    char **scopes;
    int scopes_count;
    char *permissions;
};

// https://discord.com/developers/docs/resources/application#application-object
struct discord_application {
    int64_t id;
    char *name;
    char *icon;
    char *description;
    char **rpc_origins;
    int rpc_origins_count;
    bool bot_public;
    bool bot_require_code_grant;
    char *terms_of_service_url;
    char *privacy_policy_url;
    struct discord_user *owner;
    char *verify_key;
    int64_t guild_id;
    int64_t primary_sku_id;
    char *slug;
    char *cover_image;
    int flags;
    char **tags;
    int tags_count;
    struct discord_params *install_params;
    char *custom_install_url;
};

void *disco_create_application_struct_json(cJSON *data);
void disco_destroy_application(struct discord_application *application);

#endif
