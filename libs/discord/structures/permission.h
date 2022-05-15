// https://discord.com/developers/docs/resources/channel#overwrite-object
struct discord_overwrite {
    char *id;
    int type;
    char *allow;
    char *deny;
};