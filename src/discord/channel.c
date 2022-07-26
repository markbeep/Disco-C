#include <discord/channel.h>
#include <discord/disco.h>
#include <discord/message.h>
#include <discord/thread.h>

void *_d_json_to_channel(cJSON *data) {
    struct discord_channel *ch = (struct discord_channel *)calloc(1, sizeof(struct discord_channel));
    cJSON *tmp = NULL;
    ch->id = get_long_from_string_json(data, "id", 0);
    ch->type = (enum Discord_Channel_Type)get_int_from_json(data, "type", 0);
    ch->guild_id = get_long_from_string_json(data, "guild_id", 0);
    ch->position = get_int_from_json(data, "position", -1);
    ch->permission_overwrites_count = get_array_from_json(data, "permission_overwrites", (void ***)&ch->permission_overwrites, sizeof(struct discord_overwrite), &discord_create_overwrite_struct_json);
    ch->name = get_string_from_json(data, "name");
    ch->topic = get_string_from_json(data, "topic");
    ch->nsfw = get_bool_from_json(data, "nsfw", 0);
    ch->last_message_id = get_long_from_string_json(data, "last_message_id", 0);
    ch->bitrate = get_int_from_json(data, "bitrate", 0);
    ch->user_limit = get_int_from_json(data, "user_limit", 0);
    ch->rate_limit_per_user = get_int_from_json(data, "rate_limit_per_user", 0);
    ch->recipients_count = get_array_from_json(data, "recipients", (void ***)&ch->recipients, sizeof(struct discord_user), &_d_json_to_user);
    ch->icon = get_string_from_json(data, "icon");
    ch->owner_id = get_long_from_string_json(data, "owner_id", 0);
    ch->application_id = get_long_from_string_json(data, "application_id", 0);
    ch->parent_id = get_long_from_string_json(data, "parent_id", 0);
    ch->last_pin_timestamp = get_string_from_json(data, "last_pin_timestamp");
    ch->rtc_region = get_string_from_json(data, "rtc_region");
    ch->video_quality_mode = get_int_from_json(data, "video_quality_mode", 1);
    ch->message_count = get_int_from_json(data, "message_count", 0);
    ch->member_count = get_int_from_json(data, "member_count", 0);
    tmp = cJSON_GetObjectItem(data, "thread_metadata");
    if (tmp)
        ch->thread_metadata = discord_create_thread_metadata_struct_json(tmp);
    tmp = cJSON_GetObjectItem(data, "member");
    if (tmp)
        ch->member = discord_create_thread_member_struct_json(tmp);
    ch->default_auto_archive_duration = get_int_from_json(data, "default_auto_archive_duration", 0);
    ch->permissions = get_string_from_json(data, "permissions");
    ch->flags = get_int_from_json(data, "flags", 0);
    return ch;
}
void discord_destroy_channel(struct discord_channel *ch) {
    // frees all the char pointers
    char *to_free[12] = {
        ch->name,
        ch->topic,
        ch->icon,
        ch->last_pin_timestamp,
        ch->rtc_region,
        ch->permissions};
    for (int i = 0; i < 12; i++)
        if (to_free[i])
            free(to_free[i]);
    // frees the structs
    for (int i = 0; i < ch->permission_overwrites_count; i++)
        discord_destroy_overwrite(ch->permission_overwrites[i]);
    if (ch->permission_overwrites)
        free(ch->permission_overwrites);
    for (int i = 0; i < ch->recipients_count; i++)
        discord_destroy_user(ch->recipients[i]);
    if (ch->recipients)
        free(ch->recipients);
    if (ch->thread_metadata)
        free(ch->thread_metadata);
    if (ch->member)
        free(ch->member);
    free(ch);
}

void *_d_json_to_channel_mention(cJSON *data) {
    struct discord_channel_mention *ch = (struct discord_channel_mention *)calloc(1, sizeof(struct discord_channel_mention));
    ch->id = get_long_from_string_json(data, "id", 0);
    ch->guild_id = get_long_from_string_json(data, "guild_id", 0);
    ch->type = get_int_from_json(data, "type", 0);
    ch->name = get_string_from_json(data, "name");
    return ch;
}
void discord_destroy_channel_mention(struct discord_channel_mention *ch) {
    free(ch->name);
    free(ch);
}
