#include <stdio.h>
#include <sys/time.h>

#define TIMER_START_FIRST                   \
    struct timeval stop_timer, start_timer; \
    gettimeofday(&start_timer, NULL);
#define TIMER_START gettimeofday(&start_timer, NULL);
#define TIMER_END(name)              \
    gettimeofday(&stop_timer, NULL); \
    (printf("%s: %lu us\n", name, (stop_timer.tv_sec - start_timer.tv_sec) * 1000000 + (stop_timer.tv_usec - start_timer.tv_usec)))