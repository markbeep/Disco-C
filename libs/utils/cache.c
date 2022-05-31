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
static int message_cache_size = 10;

static struct hashmap_s channels_map;
static struct hashmap_s guilds_map;

int disco_cache_init() {
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
    if (0 != hashmap_create(2, &guilds_map)) {
        d_log_err("Guild cache creation failed\n");
        return 1;
    }

    return 0;
}

void disco_cache_destroy() {
    while (!TAILQ_EMPTY(&messages_queue.head)) {
        struct node *first = TAILQ_FIRST(&messages_queue.head);
        TAILQ_REMOVE(&messages_queue.head, first, pointers);
        free(first->data);
        free(first);
        messages_queue.size--;
    }
    hashmap_destroy(&messages_map);
}

int disco_cache_set_message(struct discord_message *message) {
    unsigned int id_len = (unsigned int)strnlen(message->id, 20);

    struct node *n = (struct node *)malloc(sizeof(struct node));
    n->data = message;

    struct node *old = (struct node *)hashmap_get(&messages_map, message->id, id_len);
    if (old) { // frees the old message if it exists
        d_log_debug("Freed older message in cache\n");
        free(old->data);
        TAILQ_REMOVE(&messages_queue.head, old, pointers);
        free(old);
    } else { // new message, so increment size
        messages_queue.size++;
    }
    if (0 != hashmap_put(&messages_map, message->id, id_len, (void *)n)) {
        d_log_err("Adding message to cache failed\n");
        return 1;
    }
    TAILQ_INSERT_TAIL(&messages_queue.head, n, pointers);

    // cleanup older messages in cache
    if (message_cache_size < messages_queue.size) {
        int to_remove = messages_queue.size - message_cache_size;
        for (int i = 0; i < to_remove; i++) {
            struct node *first = TAILQ_FIRST(&messages_queue.head);
            TAILQ_REMOVE(&messages_queue.head, first, pointers);
            struct discord_message *msg = first->data;
            d_log_notice("p = %p\n", (void *)msg);
            d_log_notice("id = %s\n", msg->id);
            hashmap_remove(&messages_map, msg->id, (unsigned int)strnlen(msg->id, 20));
            free(msg);
            free(first);
            messages_queue.size--;
        }
    }
    d_log_notice("Message cache size = %d\n", messages_queue.size);

    return 0;
}

struct discord_message *disco_cache_get_message(char *id) {
    struct node *n = (struct node *)hashmap_get(&messages_map, id, (unsigned int)strnlen(id, 20));
    if (n) {
        // messages that are viewed, are put back at the end of the message queue
        TAILQ_REMOVE(&messages_queue.head, n, pointers);
        TAILQ_INSERT_TAIL(&messages_queue.head, n, pointers);
        return n->data;
    } else
        return NULL;
}
