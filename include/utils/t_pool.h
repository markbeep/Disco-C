#ifndef T_POOL
#define T_POOL

#include <curl/curl.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include <utils/prio_queue.h>

typedef void (*t_func)(void *, CURL *);
typedef struct t_work t_work_t;

struct t_work {
    t_func func;
    void *arg;
    t_work_t *next;
    struct timeval wait_until;
};

typedef struct t_pool {
    struct prio_queue queue;
    int thread_count; ///< amount of active threads
    bool stop;        ///< bool is threads should stop
    pthread_mutex_t *lock;
    pthread_cond_t *work_cond;
    pthread_cond_t *finished_cond;
    struct timeval sleep_until;
    const char *token; ///< Bot token which will be used to setup CURL headers
} t_pool_t;

/**
 * @brief Initializes the thread pool.
 *
 * @param num_t Number of threads to use for the thread pool.
 * @param token Bot token. Will be used to setup the CURL headers.
 * @return t_pool_t*
 */
t_pool_t *t_pool_init(int num_t, const char *token);

/**
 * @brief Adds work to the thread pool.
 *
 * @param tp
 * @param func
 * @param work
 * @return int
 */
int t_pool_add_work(t_pool_t *tp, t_func func, void *work, struct timeval wait_until);
/**
 * @brief Pops the head of the pool and returns the pointer to the head.
 * IMPORTANT: The received work needs to be freed to avoid memory leak!
 * (Not thread-safe)
 * @param tp
 * @return t_work_t*
 */
t_work_t *t_pool_pop_work(t_pool_t *tp);
void t_pool_wait(t_pool_t *tp);
void t_pool_destroy(t_pool_t *tp);
int t_process_count(void);

#endif
