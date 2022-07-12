#ifndef DISCO_LOGGING
#define DISCO_LOGGING

#include <stdio.h>

#define D_LOG_ERR (1 << 0)
#define D_LOG_NOTICE (1 << 1)
#define D_LOG_NORMAL (1 << 2)
#define D_LOG_DEBUG (1 << 3)
#define D_LOG_ALL D_LOG_ERR | D_LOG_NOTICE | D_LOG_NORMAL | D_LOG_DEBUG

void d_set_log_level(int flags);
int d_should_log_err(void);
int d_should_log_notice(void);
int d_should_log_normal(void);
int d_should_log_debug(void);

#define d_log_err(str, ...)                                                                       \
    {                                                                                             \
        if (d_should_log_err())                                                                   \
            fprintf(stderr, "[%s:%d] \033[91m" str "\033[0m", __FILE__, __LINE__, ##__VA_ARGS__); \
    }

#define d_log_notice(str, ...)                                                                    \
    {                                                                                             \
        if (d_should_log_notice())                                                                \
            fprintf(stderr, "[%s:%d] \033[95m" str "\033[0m", __FILE__, __LINE__, ##__VA_ARGS__); \
    }

#define d_log_normal(str, ...)                                                  \
    {                                                                           \
        if (d_should_log_normal())                                              \
            fprintf(stderr, "[%s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__); \
    }

#define d_log_debug(str, ...)                                                                     \
    {                                                                                             \
        if (d_should_log_debug())                                                                 \
            fprintf(stderr, "[%s:%d] \033[92m" str "\033[0m", __FILE__, __LINE__, ##__VA_ARGS__); \
    }

#endif
