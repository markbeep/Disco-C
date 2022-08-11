#include <stdio.h>
#include <sys/time.h>

#define TIMER_START_FIRST                      \
    struct timeval tend_109823, tstart_123123; \
    do {                                       \
        gettimeofday(&tstart_123123, NULL);    \
    } while (0)
#define TIMER_START                         \
    do {                                    \
        gettimeofday(&tstart_123123, NULL); \
    } while (0)
#define TIMER_END(name)                                                                                                                      \
    do {                                                                                                                                     \
        gettimeofday(&tend_109823, NULL);                                                                                                    \
        printf("%s: %lu us\n", name, (tend_109823.tv_sec - tstart_123123.tv_sec) * 1000000 + (tend_109823.tv_usec - tstart_123123.tv_usec)); \
    } while (0)

#define TIMER_FUNC(a)                                                                                                                \
    do {                                                                                                                             \
        struct timeval stop_timer, start_timer;                                                                                      \
        gettimeofday(&start_timer, NULL);                                                                                            \
        a;                                                                                                                           \
        gettimeofday(&stop_timer, NULL);                                                                                             \
        printf("%s: %lu us\n", #a, (stop_timer.tv_sec - start_timer.tv_sec) * 1000000 + (stop_timer.tv_usec - start_timer.tv_usec)); \
    } while (0)
