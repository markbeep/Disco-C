#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <utils/disco_logging.h>
#include <utils/prio_queue.h>
#include <utils/t_pool.h>

static void *thread_work_loop(void *tp) {
    t_pool_t *pool = (t_pool_t *)tp;
    while (1) {
        pthread_mutex_lock(pool->lock);
        while (!pool->stop && (pool->sleep || pool->queue.size == 0)) {
            pthread_cond_wait(pool->work_cond, pool->lock);
        }
        d_log_info("Thread took work in thread pool\n");
        if (pool->stop) {
            pool->thread_count--;
            pthread_cond_signal(pool->finished_cond);
            pthread_mutex_unlock(pool->lock);
            break;
        }
        // here work_count > 0
        t_work_t *work = t_pool_pop_work(pool);

        // we can now unlock as the work afterwards is single memory
        pthread_mutex_unlock(pool->lock);

        // check if we can use the work node already
        struct timeval now;
        gettimeofday(&now, NULL);
        long wait_ms = 0;
        if (work->wait_until.tv_sec > 0 && work->wait_until.tv_usec > 0)
            wait_ms = (work->wait_until.tv_sec - now.tv_sec) * 1000 + (work->wait_until.tv_usec - now.tv_usec) / 1000;
        d_log_notice("Wait MS: %ld\n", wait_ms);
        if (wait_ms > 0) {
            t_pool_add_work(pool, work->func, work->arg, work->wait_until);
            continue;
        }

        work->func(work->arg);
        free(work);
    }
    return NULL;
}

t_pool_t *t_pool_init(int num_t) {
    t_pool_t *pool = malloc(sizeof(struct t_pool));
    prio_init(&pool->queue);
    pool->thread_count = num_t;
    pool->stop = 0;
    pool->work_cond = (pthread_cond_t *)calloc(1, sizeof(pthread_cond_t));
    pool->finished_cond = (pthread_cond_t *)calloc(1, sizeof(pthread_cond_t));
    pool->lock = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
    pthread_cond_init(pool->work_cond, NULL);
    pthread_cond_init(pool->finished_cond, NULL);

    pthread_t thread;
    for (int i = 0; i < num_t; i++) {
        pthread_create(&thread, NULL, &thread_work_loop, (void *)pool);
        pthread_detach(thread);
    }
    return pool;
}

int t_pool_add_work(t_pool_t *tp, t_func func, void *arg, struct timeval wait_until) {
    // creates the new work node
    t_work_t *work = (t_work_t *)malloc(sizeof(struct t_work));
    work->arg = arg;
    work->func = func;
    work->wait_until = wait_until;
    work->next = NULL;

    // coarsely locks the queue
    pthread_mutex_lock(tp->lock);
    prio_push(&tp->queue, (void *)work, wait_until);

    // we check if the threads should sleep until the timer of the first node is off
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval sleep_until = {0};
    if (tp->queue.head)
        sleep_until = tp->queue.head->wait_until;
    if (sleep_until.tv_sec > now.tv_sec || (sleep_until.tv_sec == now.tv_sec && sleep_until.tv_usec > now.tv_usec)) {
        tp->sleep = true;
        tp->sleep_until = sleep_until;
    }
    pthread_cond_signal(tp->work_cond);
    pthread_mutex_unlock(tp->lock);

    return 1;
}

t_work_t *t_pool_pop_work(t_pool_t *tp) {
    prio_node_t *node = prio_pop(&tp->queue);
    if (!node)
        return NULL;
    t_work_t *work = (t_work_t *)node->data;
    free(node);
    return work;
}

void t_pool_wait(t_pool_t *tp) {
    pthread_mutex_lock(tp->lock);
    while (tp->thread_count > 0)
        pthread_cond_wait(tp->finished_cond, tp->lock);
    pthread_mutex_unlock(tp->lock);
}

void t_pool_destroy(t_pool_t *tp) {
    pthread_mutex_lock(tp->lock);
    tp->stop = true;
    // delete all nodes still in the queue
    prio_node_t *cur = tp->queue.head, *prev = NULL;
    while (cur) {
        prev = cur;
        cur = cur->next;
        if (prev)
            free(prev->data);
    }
    prio_destroy_queue(&tp->queue);
    // wakes up all sleeping threads
    pthread_cond_broadcast(tp->work_cond);
    pthread_mutex_unlock(tp->lock);
    // waits for all threads to finish
    t_pool_wait(tp);
    pthread_cond_destroy(tp->work_cond);
    free(tp->work_cond);
    pthread_cond_destroy(tp->finished_cond);
    free(tp->finished_cond);
    pthread_mutex_destroy(tp->lock);
    free(tp->lock);
    free(tp);
}

int t_process_count(void) {
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
}
