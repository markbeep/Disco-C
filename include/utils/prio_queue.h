#ifndef D_PRIO_QUEUE
#define D_PRIO_QUEUE

typedef struct prio_node {
    void *data;
    long retry_after;
    struct prio_node *next;
} prio_node_t;

struct prio_queue {
    prio_node_t *head;
    size_t size;
};

void prio_init(struct prio_queue *queue);
prio_node_t *prio_push(struct prio_queue *queue, void *data, long retry_after);
prio_node_t *prio_pop(struct prio_queue *queue);
void prio_destroy_node(prio_node_t *node);
void prio_destroy_queue(struct prio_queue *queue);

#endif
