// https://discord.com/developers/docs/resources/channel#allowed-mentions-object-allowed-mentions-structure
struct discord_allowed_mentions {
    int parse_role_mentions;     // if roles should be mentioned
    int parse_user_mentions;     // if users should be mentioned
    int parse_everyone_mentions; // if @everyone should mention
    char **roles;
    int roles_count;
    char **users;
    int users_count;
    int replied_user;
};

struct discord_component;
struct discord_attachment;

struct discord_embed_footer {
    char *text;
    char *icon_url;
    char *proxy_icon_url;
};

// video, image and thumbnail are all the same format
struct discord_embed_media {
    char *url;
    char *proxy_url;
    int height;
    int width;
};

struct discord_embed_provider {
    char *name;
    char *url;
};

struct discord_embed_author {
    char *name;
    char *url;
    char *icon_url;
    char *proxy_icon_url;
};

struct discord_embed_field {
    char *name;
    char *value;
    int is_inline;
};

// https://discord.com/developers/docs/resources/channel#embed-object
struct discord_embed {
    char *title;
    char *type; // should always be "rich" for generic embed
    char *description;
    char *url;
    char *timestamp;
    int color;
    struct discord_embed_footer *footer;
    struct discord_embed_media *image;
    struct discord_embed_media *thumbnail;
    struct discord_embed_media *video;
    struct discord_embed_provider *provider;
    struct discord_embed_author *author;
    struct discord_embed_field **fields;
    int field_count;
};

struct discord_message_reference {
    char *message_id;
    char *channel_id;
    char *guild_id;
    int fail_if_not_exists;
};

// https://discord.com/developers/docs/resources/channel#create-message
struct discord_create_message {
    char *content;
    int tts;
    struct discord_embed **embeds;
    int embeds_count;
    struct discord_allowed_mentions *allowed_mentions;
    struct discord_message_reference *message_reference;
    struct discord_component **components;
    int components_count;
    char **sticker_ids;
    struct discord_attachment **attachments;
    int attachments_count;
    int flags;
};

struct discord_role;
struct discord_reaction;
struct discord_message_activity;
struct discord_application;
struct discord_interaction;
struct discord_message_sticker_item;
struct discord_sticker;

// https://discord.com/developers/docs/resources/channel#message-object-message-types
enum discord_message_type { DEFAULT,
                            RECIPIENT_ADD,
                            RECIPIENT_REMOVE,
                            CALL,
                            CHANNEL_NAME_CHANGE,
                            CHANNEL_ICON_CHANGE,
                            CHANNEL_PINNED_MESSAGE,
                            GUILD_MEMBER_JOIN,
                            USER_PREMIUM_GUILD_SUBSCRIPTION,
                            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1,
                            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2,
                            USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3,
                            CHANNEL_FOLLOW_ADD = 12,
                            GUILD_DISCOVERY_DISQUALIFIED = 14,
                            GUILD_DISCOVERY_REQUALIFIED,
                            GUILD_DISCOVERY_GRACE_PERIOD_INITIAL_WARNING,
                            GUILD_DISCOVERY_GRACE_PERIOD_FINAL_WARNING,
                            THREAD_CREATED,
                            REPLY,
                            CHAT_INPUT_COMMAND,
                            THREAD_STARTER_MESSAGE,
                            GUILD_INVITE_REMINDER,
                            CONTEXT_MENU_COMMAND };

// https://discord.com/developers/docs/resources/channel#message-object
struct discord_message {
    char *id;
    char *channel_id;
    char *guild_id;
    struct discord_user *author;
    struct discord_member *member;
    char *content;
    char *timestamp;
    char *edited_timestamp;
    int tts;
    int mention_everyone;
    struct discord_member **mentions;
    int mentions_count;
    struct discord_role **mention_roles;
    int mention_roles_count;
    struct discord_attachment **attachments;
    int attachments_count;
    struct discord_embed **embeds;
    int embeds_count;
    struct discord_reaction **reactions;
    int reactions_count;
    char *nonce;
    int pinned;
    char *webhook_id;
    enum discord_message_type type;
    struct discord_message_activity *activity;
    struct discord_application *application;
    char *application_id;
    struct discord_message_reference *message_reference;
    struct discord_interaction *interaction;
    struct discord_channel *thread;
    struct discord_component **components;
    int components_count;
    struct discord_message_sticker_item **sticker_items;
    int message_sticker_item_count;
    struct discord_sticker **stickers;
    int stickers_count;
};