#ifndef DISCO_ROLE
#define DISCO_ROLE

struct discord_role_tags {
    char *bot_id;
    char *integration_id;
    int premium_subscriber;
};

// TODO implement
void *disco_create_role_tags_struct_json(cJSON *data);
void disco_destroy_role_tags(struct discord_role_tags *tags);

struct discord_role {
    char *id;
    char *name;
    int color;
    int hoist;
    char *icon;
    char *unicode_emoji;
    int position;
    char *permissions;
    int managed;
    int mentionable;
};

// TODO implement
void *disco_create_role_struct_json(cJSON *data);
void disco_destroy_role(struct discord_role *role);

#endif
