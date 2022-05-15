// https://discord.com/developers/docs/resources/channel#thread-metadata-object
struct discord_thread_metadata {
    int archived;
    int auto_archive_duration;
    char *archive_timestamp;
    int locked;
    int invitable;
    char *create_timestamp;
};

// https://discord.com/developers/docs/resources/channel#thread-member-object
struct discord_thread_member {
    char *id;
    char *user_id;
    char *join_timestamp;
    int flags;
};