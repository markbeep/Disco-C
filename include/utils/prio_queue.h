#include <cJSON/cJSON.h>
#include <web/request.h>

struct discord_prio_packet {
    int retry_after;
    enum Request_Type request_type;
    cJSON *json;
};

typedef struct prio_node prio_node_t;

typedef struct prio_node {
    struct discord_prio_packet *data;
    prio_node_t *next;
} prio_node_t;

struct prio_queue {
    prio_node_t *head;
};

prio_node_t *prio_push(struct prio_queue *queue, enum Request_Type request_type, cJSON *data, int retry_after);
prio_node_t *prio_pop(struct prio_queue *queue);
void prio_destroy_node(prio_node_t *node);
void prio_destroy_queue(struct prio_queue *queue);
