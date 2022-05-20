#ifndef DISCO_LOGGING
#define DISCO_LOGGING

#include <stdio.h>

#define D_LOG_ERR (1 << 0)
#define D_LOG_NOTICE (1 << 1)
#define D_LOG_NORMAL (1 << 2)
#define D_LOG_DEBUG (1 << 3)

static int __log_level = 0;

void d_set_log_level(int log_level);

#define d_log_err(str, ...)                                           \
    {                                                                 \
        if (__log_level & D_LOG_ERR)                                  \
            fprintf(stderr, "\033[91m" str "\033[0m", ##__VA_ARGS__); \
    }

#define d_log_notice(str, ...)                                        \
    {                                                                 \
        if (__log_level & D_LOG_NOTICE)                               \
            fprintf(stderr, "\033[95m" str "\033[0m", ##__VA_ARGS__); \
    }

#define d_log_normal(str, ...)                   \
    {                                            \
        if (__log_level & D_LOG_NORMAL)          \
            fprintf(stderr, str, ##__VA_ARGS__); \
    }

#define d_log_debug(str, ...)                                         \
    {                                                                 \
        if (__log_level & D_LOG_DEBUG)                                \
            fprintf(stderr, "\033[92m" str "\033[0m", ##__VA_ARGS__); \
    }

#endif
