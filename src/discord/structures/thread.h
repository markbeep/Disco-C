#ifndef DISCO_THREAD
#define DISCO_THREAD

#include <cJSON/cJSON.h>
#include <stdbool.h>

// https://discord.com/developers/docs/resources/channel#thread-metadata-object
struct discord_thread_metadata {
    bool archived;
    int auto_archive_duration;
    char *archive_timestamp;
    bool locked;
    bool invitable;
    char *create_timestamp;
};

// TODO implement
void *disco_create_thread_metadata_struct_json(cJSON *data);
void disco_destroy_thread_metadata(struct discord_thread_metadata *thread);

// https://discord.com/developers/docs/resources/channel#thread-member-object
struct discord_thread_member {
    char *id;
    char *user_id;
    char *join_timestamp;
    int flags;
};

// TODO implement
void *disco_create_thread_member_struct_json(cJSON *data);
void disco_destroy_thread_member(struct discord_thread_member *member);

#endif
