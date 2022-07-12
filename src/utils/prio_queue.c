#include <stdlib.h>
#include <utils/prio_queue.h>

prio_node_t *prio_push(struct prio_queue *queue, enum Request_Type request_type, cJSON *data, int retry_after) {
    prio_node_t *node = (prio_node_t *)malloc(sizeof(struct prio_node));
    node->data = (struct discord_prio_packet *)malloc(sizeof(struct discord_prio_packet));
    node->data->json = data;
    node->data->request_type = request_type;
    node->data->retry_after = retry_after;
    node->next = NULL;
    if (!queue) {
        struct prio_queue *q = (struct prio_queue *)malloc(sizeof(struct prio_queue));
        q->head = node;
        return node;
    }
    prio_node_t *cur = queue->head;
    prio_node_t *prev = NULL;
    while (cur->next && cur->data->retry_after < retry_after) {
        prev = cur;
        cur = cur->next;
    }
    // at this point cur is greater than our node
    // and prev is smaller
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
    if (node)
        queue->head = node->next;
    return node;
}

void prio_destroy_node(prio_node_t *node) {
    if (!node)
        return;
    if (node->data)
        free(node->data);
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
    } while (cur->next);
    free(queue);
}
