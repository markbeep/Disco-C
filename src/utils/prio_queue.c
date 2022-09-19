#include <stdlib.h>
#include <utils/prio_queue.h>

void prio_init(struct prio_queue *queue) {
    queue->head = NULL;
    queue->size = 0;
}

prio_node_t *prio_push(struct prio_queue *queue, void *data, struct timeval wait_until) {
    prio_node_t *node = (prio_node_t *)malloc(sizeof(struct prio_node));
    node->data = data;
    node->wait_until = wait_until;
    node->next = NULL;
    if (!queue->head) {
        queue->head = node;
        queue->size = 1;
        return node;
    }
    queue->size++;
    prio_node_t *cur = queue->head;
    prio_node_t *prev = NULL;
    while (cur->next && (cur->wait_until.tv_sec < wait_until.tv_sec || (cur->wait_until.tv_sec == wait_until.tv_sec && cur->wait_until.tv_usec <= wait_until.tv_usec))) {
        prev = cur;
        cur = cur->next;
    }
    // at this point cur is greater than our node
    // and prev is strictly smaller
    if (prev)
        prev->next = node;
    if (!prev) // node is the first element
        queue->head = node;
    node->next = cur;
    return node;
}

prio_node_t *prio_pop(struct prio_queue *queue) {
    if (!queue || !queue->head)
        return NULL;
    prio_node_t *node = queue->head;
    if (node) {
        queue->head = node->next;
        queue->size--;
    }
    return node;
}

void prio_destroy_node(prio_node_t *node) {
    if (!node)
        return;
    free(node);
}

void prio_destroy_queue(struct prio_queue *queue) {
    if (!queue || !queue->head)
        return;
    prio_node_t *cur = queue->head;
    prio_node_t *prev = NULL;
    do {
        prev = cur;
        cur = cur->next;
        if (prev)
            prio_destroy_node(prev);
    } while (cur && cur->next);
}
