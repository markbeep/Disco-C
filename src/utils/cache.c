/**
 * @file cache.c
 * @brief This file handles a cache for messages, guilds and channels. It will usually clean
 * up the structures itself. Meaning if a message has to be taken out of cache for space reasons
 * the cache will deallocate the message accordingly. The only exception to this is if an element
 * is updated (a message is edited). Then the old message/channel/guild structure will NOT be cleaned up.
 * It will have to be manually cleaned up by the user, since the hashmap now points to the new message
 * struct.
 * @version 0.1
 * @date 2022-06-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "cache.h"
#include "disco_logging.h"
#include <sys/queue.h>

struct node {
    void *data;
    TAILQ_ENTRY(node)
    pointers;
};

// buffer is used to keep track of what messages were added first
// so that we can then remove old messages
struct buffer {
    TAILQ_HEAD(queue, node)
    head;
    int size;
};

// hashmap with string-node pointer mapping
static struct hashmap_s messages_map;
// queue with node pointers in the order they were received
static struct buffer messages_queue;
// message cache size
static int max_message_cache_size;

static struct hashmap_s channels_map;
static struct buffer channels_queue;
static int max_channel_cache_size = 1000;

static struct hashmap_s guilds_map;
static struct buffer guilds_queue;
static int max_guild_cache_size = 100;

int disco_cache_init(int cache_size) {
    max_message_cache_size = cache_size;

    if (0 != hashmap_create(2, &messages_map)) {
        d_log_err("Message cache creation failed\n");
        return 1;
    }
    TAILQ_INIT(&messages_queue.head);
    messages_queue.size = 0;

    if (0 != hashmap_create(2, &channels_map)) {
        d_log_err("Channel cache creation failed\n");
        return 1;
    }
    TAILQ_INIT(&channels_queue.head);
    channels_queue.size = 0;

    if (0 != hashmap_create(2, &guilds_map)) {
        d_log_err("Guild cache creation failed\n");
        return 1;
    }
    TAILQ_INIT(&guilds_queue.head);
    guilds_queue.size = 0;

    return 0;
}

void disco_cache_destroy() {
    while (!TAILQ_EMPTY(&messages_queue.head)) {
        struct node *first = TAILQ_FIRST(&messages_queue.head);
        TAILQ_REMOVE(&messages_queue.head, first, pointers);
        disco_destroy_message(first->data);
        free(first);
        messages_queue.size--;
    }
    while (!TAILQ_EMPTY(&channels_queue.head)) {
        struct node *first = TAILQ_FIRST(&channels_queue.head);
        TAILQ_REMOVE(&channels_queue.head, first, pointers);
        disco_destroy_channel(first->data);
        free(first);
        channels_queue.size--;
    }
    while (!TAILQ_EMPTY(&guilds_queue.head)) {
        struct node *first = TAILQ_FIRST(&guilds_queue.head);
        TAILQ_REMOVE(&guilds_queue.head, first, pointers);
        disco_destroy_guild(first->data);
        free(first);
        guilds_queue.size--;
    }
    hashmap_destroy(&messages_map);
    hashmap_destroy(&channels_map);
    hashmap_destroy(&guilds_map);
}

int disco_cache_set(enum Disco_Cache_Type type, void *cont) {
    static struct hashmap_s *map;
    static struct buffer *queue;
    char *id;
    int max_cache_size;
    char *cache_name;

    switch (type) {
    case DISCO_MESSAGE_CACHE:
        cache_name = "Message";
        map = &messages_map;
        queue = &messages_queue;
        id = ((struct discord_message *)cont)->id;
        max_cache_size = max_message_cache_size;
        break;
    case DISCO_CHANNEL_CACHE:
        cache_name = "Channel";
        map = &channels_map;
        queue = &channels_queue;
        id = ((struct discord_channel *)cont)->id;
        max_cache_size = max_channel_cache_size;
        break;
    case DISCO_GUILD_CACHE:
        cache_name = "Guild";
        map = &guilds_map;
        queue = &guilds_queue;
        id = ((struct discord_guild *)cont)->id;
        max_cache_size = max_guild_cache_size;
        break;
    }
    unsigned int id_len = (unsigned int)strnlen(id, 20);

    struct node *n = (struct node *)malloc(sizeof(struct node));
    n->data = cont;

    struct node *old = (struct node *)hashmap_get(map, id, id_len);
    if (old) { // frees the old node if it exists
        // NOTE: this doesn't free the struct, only the node in the queue and map
        d_log_debug("Freed older entry in %s cache\n", cache_name);
        TAILQ_REMOVE(&queue->head, old, pointers);
        free(old);
    } else { // new entry, so increment size
        queue->size++;
    }
    if (0 != hashmap_put(map, id, id_len, (void *)n)) {
        d_log_err("Adding entry to %s cache failed\n", cache_name);
        return 1;
    }
    TAILQ_INSERT_TAIL(&queue->head, n, pointers);

    // cleanup older entries in cache
    if (max_cache_size < queue->size) {
        int to_remove = queue->size - max_cache_size;
        for (int i = 0; i < to_remove; i++) {
            struct node *first = TAILQ_FIRST(&queue->head);
            switch (type) {
            case DISCO_MESSAGE_CACHE:
                disco_cache_delete_message(((struct discord_message *)first->data)->id);
                break;
            case DISCO_CHANNEL_CACHE:
                disco_cache_delete_channel(((struct discord_channel *)first->data)->id);
                break;
            case DISCO_GUILD_CACHE:
                disco_cache_delete_guild(((struct discord_guild *)first->data)->id);
                break;
            }
        }
    }
    d_log_notice("%s cache size = %d\n", cache_name, queue->size);

    return 0;
}

void *disco_cache_get(enum Disco_Cache_Type type, char *id) {
    static struct hashmap_s *map;
    static struct buffer *queue;
    switch (type) {
    case DISCO_MESSAGE_CACHE:
        map = &messages_map;
        queue = &messages_queue;
        break;
    case DISCO_CHANNEL_CACHE:
        map = &channels_map;
        queue = &channels_queue;
        break;
    case DISCO_GUILD_CACHE:
        map = &guilds_map;
        queue = &guilds_queue;
        break;
    }
    struct node *n = (struct node *)hashmap_get(map, id, (unsigned int)strnlen(id, 20));
    if (n) {
        // entries that are viewed, are put back at the end of the queue
        TAILQ_REMOVE(&queue->head, n, pointers);
        TAILQ_INSERT_TAIL(&queue->head, n, pointers);
        return n->data;
    } else
        return NULL;
}

void disco_cache_delete(enum Disco_Cache_Type type, char *id) {
    static struct hashmap_s *map;
    static struct buffer *queue;
    switch (type) {
    case DISCO_MESSAGE_CACHE:
        map = &messages_map;
        queue = &messages_queue;
        break;
    case DISCO_CHANNEL_CACHE:
        map = &channels_map;
        queue = &channels_queue;
        break;
    case DISCO_GUILD_CACHE:
        map = &guilds_map;
        queue = &guilds_queue;
        break;
    }
    struct node *n = (struct node *)hashmap_get(map, id, (unsigned int)strnlen(id, 20));
    if (n) {
        TAILQ_REMOVE(&queue->head, n, pointers);
        hashmap_remove(map, id, (unsigned int)strnlen(id, 20));
        switch (type) {
        case DISCO_MESSAGE_CACHE:
            disco_destroy_message((struct discord_message *)n->data);
            break;
        case DISCO_CHANNEL_CACHE:
            disco_destroy_channel((struct discord_channel *)n->data);
            break;
        case DISCO_GUILD_CACHE:
            disco_destroy_guild((struct discord_guild *)n->data);
            break;
        }
        free(n);
        queue->size--;
    }
}

int disco_cache_set_message(struct discord_message *message) {
    return disco_cache_set(DISCO_MESSAGE_CACHE, (void *)message);
}
struct discord_message *disco_cache_get_message(char *id) {
    return disco_cache_get(DISCO_MESSAGE_CACHE, id);
}
void disco_cache_delete_message(char *id) {
    disco_cache_delete(DISCO_MESSAGE_CACHE, id);
}

int disco_cache_set_channel(struct discord_channel *channel) {
    return disco_cache_set(DISCO_CHANNEL_CACHE, (void *)channel);
}
struct discord_channel *disco_cache_get_channel(char *id) {
    return disco_cache_get(DISCO_CHANNEL_CACHE, id);
}
void disco_cache_delete_channel(char *id) {
    disco_cache_delete(DISCO_CHANNEL_CACHE, id);
}

int disco_cache_set_guild(struct discord_guild *guild) {
    return disco_cache_set(DISCO_GUILD_CACHE, (void *)guild);
}
struct discord_guild *disco_cache_get_guild(char *id) {
    return disco_cache_get(DISCO_GUILD_CACHE, id);
}
void disco_cache_delete_guild(char *id) {
    disco_cache_delete(DISCO_GUILD_CACHE, id);
}
