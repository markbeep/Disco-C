/**
 * @file cache.c
 * @brief This file handles a cache for messages, guilds and channels.
 * Structures are kept alive in the queue even if a new structure for
 * the ID is created (message was edited). The structure will be cleaned
 * up though if it is at the beginning of the queue and the max cache size
 * is reached.
 * struct.
 * @version 0.1
 * @date 2022-06-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <sys/queue.h>
#include <utils/cache.h>
#include <utils/disco_logging.h>

struct node {
    uint64_t *alloc_id; // we need to allocate the ID and save the pointer somewhere
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
static int max_message_cache_size = 1000;

static struct hashmap_s channels_map;
static struct buffer channels_queue;
static int max_channel_cache_size = 1000;

static struct hashmap_s guilds_map;
static struct buffer guilds_queue;
static int max_guild_cache_size = 1000;

static cache_destroy_callback_fn destroy_item_callback_fn;

static void default_destroy_callback(enum Disco_Cache_Type type, void *structure) {
    switch (type) {
    case DISCO_MESSAGE_CACHE:
        discord_destroy_message((struct discord_message *)structure);
        break;
    case DISCO_CHANNEL_CACHE:
        discord_destroy_channel((struct discord_channel *)structure);
        break;
    case DISCO_GUILD_CACHE:
        discord_destroy_guild((struct discord_guild *)structure);
        break;
    default:
        break;
    }
}

int discord_cache_init(int message_cache_size, int channel_cache_size, int guild_cache_size, cache_destroy_callback_fn callback) {
    max_message_cache_size = message_cache_size;
    if (max_message_cache_size < 2)
        max_message_cache_size = 2;
    max_channel_cache_size = channel_cache_size;
    if (max_channel_cache_size < 2)
        max_channel_cache_size = 2;
    max_guild_cache_size = guild_cache_size;
    if (max_guild_cache_size < 2)
        max_guild_cache_size = 2;
    if (callback)
        destroy_item_callback_fn = callback;
    else
        destroy_item_callback_fn = default_destroy_callback;

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

    d_log_notice("Estimated maximum cache sizes:\n\tMessage: \t%luB\n\tChannel: \t%luB\n\tGuild:  \t%luB\n",
                 (sizeof(struct node) + sizeof(struct discord_message)) * (long unsigned int)max_message_cache_size,
                 (sizeof(struct node) + sizeof(struct discord_channel)) * (long unsigned int)max_channel_cache_size,
                 (sizeof(struct node) + sizeof(struct discord_guild)) * (long unsigned int)max_guild_cache_size);

    return 0;
}

void discord_cache_destroy(void) {
    while (!TAILQ_EMPTY(&messages_queue.head)) {
        struct node *first = TAILQ_FIRST(&messages_queue.head);
        TAILQ_REMOVE(&messages_queue.head, first, pointers);
        destroy_item_callback_fn(DISCO_MESSAGE_CACHE, first->data);
        free(first->alloc_id);
        free(first);
        messages_queue.size--;
    }
    while (!TAILQ_EMPTY(&channels_queue.head)) {
        struct node *first = TAILQ_FIRST(&channels_queue.head);
        TAILQ_REMOVE(&channels_queue.head, first, pointers);
        destroy_item_callback_fn(DISCO_CHANNEL_CACHE, first->data);
        free(first->alloc_id);
        free(first);
        channels_queue.size--;
    }
    while (!TAILQ_EMPTY(&guilds_queue.head)) {
        struct node *first = TAILQ_FIRST(&guilds_queue.head);
        TAILQ_REMOVE(&guilds_queue.head, first, pointers);
        destroy_item_callback_fn(DISCO_GUILD_CACHE, first->data);
        free(first->alloc_id);
        free(first);
        guilds_queue.size--;
    }
    hashmap_destroy(&messages_map);
    hashmap_destroy(&channels_map);
    hashmap_destroy(&guilds_map);
}

int discord_cache_set(enum Disco_Cache_Type type, void *cont) {
    static struct hashmap_s *map;
    static struct buffer *queue;
    uint64_t id;
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
    default:
        max_cache_size = 1000;
        break;
    }

    struct node *old = (struct node *)hashmap_get(map, (char *)&id, sizeof(uint64_t));
    if (old) {
        // if there's an old node, moves it back to the beginning of the queue
        d_log_debug("Freed older entry in %s cache\n", cache_name);
        TAILQ_REMOVE(&queue->head, old, pointers);
        TAILQ_INSERT_TAIL(&queue->head, old, pointers);
    }

    struct node *n = (struct node *)malloc(sizeof(struct node));
    n->data = cont;
    queue->size++;
    // we have to place the ID on the heap so the key stays intact
    n->alloc_id = (uint64_t *)malloc(sizeof(uint64_t));
    *n->alloc_id = id;
    if (0 != hashmap_put(map, (char *)n->alloc_id, sizeof(uint64_t), (void *)n)) {
        d_log_err("Adding entry to %s cache failed\n", cache_name);
        return 1;
    }
    TAILQ_INSERT_TAIL(&queue->head, n, pointers);

    // cleanup older entries in cache
    if (max_cache_size < queue->size) {
        int to_remove = queue->size - max_cache_size;
        d_log_debug("Removing %d elements from %s cache\n", to_remove, cache_name);
        for (int i = 0; i < to_remove; i++) {
            struct node *first = TAILQ_FIRST(&queue->head);
            switch (type) {
            case DISCO_MESSAGE_CACHE:
                discord_cache_delete_message(((struct discord_message *)first->data)->id);
                break;
            case DISCO_CHANNEL_CACHE:
                discord_cache_delete_channel(((struct discord_channel *)first->data)->id);
                break;
            case DISCO_GUILD_CACHE:
                discord_cache_delete_guild(((struct discord_guild *)first->data)->id);
                break;
            default:
                break;
            }
        }
    }
    d_log_debug("%s cache size = %d / %d\n", cache_name, queue->size, max_cache_size);

    return 0;
}

void *discord_cache_get(enum Disco_Cache_Type type, uint64_t id) {
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
    default:
        break;
    }
    struct node *n = (struct node *)hashmap_get(map, (char *)&id, sizeof(uint64_t));
    if (n) {
        // entries that are viewed, are put back at the end of the queue
        TAILQ_REMOVE(&queue->head, n, pointers);
        TAILQ_INSERT_TAIL(&queue->head, n, pointers);
        return n->data;
    } else
        return NULL;
}

void discord_cache_delete(enum Disco_Cache_Type type, uint64_t id) {
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
    default:
        break;
    }
    struct node *n = (struct node *)hashmap_get(map, (char *)&id, sizeof(uint64_t));
    if (n) {
        TAILQ_REMOVE(&queue->head, n, pointers);
        hashmap_remove(map, (char *)&id, sizeof(uint64_t));
        switch (type) {
        case DISCO_MESSAGE_CACHE:
            destroy_item_callback_fn(DISCO_MESSAGE_CACHE, n->data);
            break;
        case DISCO_CHANNEL_CACHE:
            destroy_item_callback_fn(DISCO_CHANNEL_CACHE, n->data);
            break;
        case DISCO_GUILD_CACHE:
            destroy_item_callback_fn(DISCO_GUILD_CACHE, n->data);
            break;
        default:
            break;
        }
        free(n->alloc_id);
        free(n);
        queue->size--;
    }
}

int discord_cache_set_message(struct discord_message *message) {
    return discord_cache_set(DISCO_MESSAGE_CACHE, (void *)message);
}
struct discord_message *discord_cache_get_message(uint64_t id) {
    return discord_cache_get(DISCO_MESSAGE_CACHE, id);
}
void discord_cache_delete_message(uint64_t id) {
    discord_cache_delete(DISCO_MESSAGE_CACHE, id);
}

int discord_cache_set_channel(struct discord_channel *channel) {
    return discord_cache_set(DISCO_CHANNEL_CACHE, (void *)channel);
}
struct discord_channel *discord_cache_get_channel(uint64_t id) {
    return discord_cache_get(DISCO_CHANNEL_CACHE, id);
}
void discord_cache_delete_channel(uint64_t id) {
    discord_cache_delete(DISCO_CHANNEL_CACHE, id);
}

int discord_cache_set_guild(struct discord_guild *guild) {
    return discord_cache_set(DISCO_GUILD_CACHE, (void *)guild);
}
struct discord_guild *discord_cache_get_guild(uint64_t id) {
    return discord_cache_get(DISCO_GUILD_CACHE, id);
}
void discord_cache_delete_guild(uint64_t id) {
    discord_cache_delete(DISCO_GUILD_CACHE, id);
}
