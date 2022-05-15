// https://discord.com/developers/docs/resources/user#user-object
struct discord_user {
    char *id;
    char *username;
    char *discriminator;
    char *avatar;
    int bot;
    int system;
    int mfa_enabled;
    char *banner;
    int accent_color;
    char *locale;
    int verified;
    char *email;
    int flags;
    int premium_type;
    int public_flags;
};

// https://discord.com/developers/docs/resources/guild#guild-member-object
struct discord_member {
    struct discord_user *user;
    char *nick;
    char *avatar;
    char **roles;
    int roles_count;
    char *joined_at;
    char *premium_since;
    int deaf;
    int mute;
    int pending;
    char *permissions;
    char *communication_disabled_until;
};