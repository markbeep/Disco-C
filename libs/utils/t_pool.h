#ifndef T_POOL
#define T_POOL

#include <pthread.h>

typedef struct t_work t_work_t;

typedef struct t_pool {
    pthread_t **threads;
    t_work_t *first_work;
    t_work_t *last_work;
    int work_count;   // amount of active work load
    int thread_count; // amount of active threads
    int stop;         // bool is threads should stop
    pthread_mutex_t *lock;
    pthread_cond_t *work_cond;
    pthread_cond_t *finished_cond;
} t_pool_t;

typedef void (*t_func)(void *);

typedef struct t_work {
    t_func func;
    void *arg;
    t_work_t *next;
} t_work_t;

t_pool_t *t_pool_init(int num_t);
int t_pool_add_work(t_pool_t *tp, t_func func, void *work);
/**
 * @brief Pops the head of the pool and returns the pointer to the head.
 * IMPORTANT: The received work needs to be freed to avoid memory leak!
 *
 * @param tp
 * @return t_work_t*
 */
t_work_t *t_pool_pop_work(t_pool_t *tp);
void t_pool_wait(t_pool_t *tp);
void t_pool_destroy(t_pool_t *tp);
int t_process_count();

#endif
