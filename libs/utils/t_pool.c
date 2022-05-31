#include "t_pool.h"
#include "disco_logging.h"
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

static void *thread_work_loop(void *tp) {
    t_pool_t *pool = (t_pool_t *)tp;
    while (1) {
        pthread_mutex_lock(pool->lock);
        while (pool->work_count == 0 && !pool->stop) {
            pthread_cond_wait(pool->work_cond, pool->lock);
        }
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
        work->func(work->arg);
        free(work);
    }
    return NULL;
}

t_pool_t *t_pool_init(int num_t) {
    t_pool_t *pool = malloc(sizeof(struct t_pool));
    pool->first_work = NULL;
    pool->last_work = NULL;
    pool->thread_count = num_t;
    pool->work_count = 0;
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

int t_pool_add_work(t_pool_t *tp, t_func func, void *arg) {
    // creates the new work node
    t_work_t *work = (t_work_t *)malloc(sizeof(struct t_work));
    work->arg = arg;
    work->func = func;
    work->next = NULL;

    // coarsely locks the queue
    pthread_mutex_lock(tp->lock);
    t_work_t *last = tp->last_work;
    if (last)
        last->next = work;
    else
        tp->first_work = work;
    tp->last_work = work;
    tp->work_count++;
    pthread_cond_signal(tp->work_cond);
    pthread_mutex_unlock(tp->lock);

    return 1;
}

t_work_t *t_pool_pop_work(t_pool_t *tp) {
    if (tp->work_count == 0)
        return NULL;
    t_work_t *head = tp->first_work;
    if (head->next) // there are still elements in the queue
        tp->first_work = head->next;
    else { // we removed the last element in the queue
        tp->first_work = NULL;
        tp->last_work = NULL;
    }
    tp->work_count--;
    return head;
}

void t_pool_wait(t_pool_t *tp) {
    pthread_mutex_lock(tp->lock);
    while (tp->thread_count > 0)
        pthread_cond_wait(tp->finished_cond, tp->lock);
    pthread_mutex_unlock(tp->lock);
}

void t_pool_destroy(t_pool_t *tp) {
    pthread_mutex_lock(tp->lock);
    tp->stop = 1;
    // delete all nodes still in the queue
    t_work_t *cur = tp->first_work, *prev = NULL;
    while (cur) {
        prev = cur;
        cur = cur->next;
        if (prev)
            free(prev);
    }
    tp->work_count = 0;
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

int t_process_count() {
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
}
