#ifndef D_PRIO_QUEUE
#define D_PRIO_QUEUE

#include <sys/time.h>

typedef struct prio_node {
    void *data;
    struct timeval wait_until;
    struct prio_node *next;
} prio_node_t;

struct prio_queue {
    prio_node_t *head;
    size_t size;
};

void prio_init(struct prio_queue *queue);
prio_node_t *prio_push(struct prio_queue *queue, void *data, struct timeval wait_until);
prio_node_t *prio_pop(struct prio_queue *queue);
void prio_destroy_node(prio_node_t *node);
void prio_destroy_queue(struct prio_queue *queue);

#endif
