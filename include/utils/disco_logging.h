#ifndef DISCO_LOGGING
#define DISCO_LOGGING

#include <stdio.h>
#include <time.h>

#define D_LOG_ERR (1 << 0)
#define D_LOG_NOTICE (1 << 1)
#define D_LOG_INFO (1 << 2)
#define D_LOG_DEBUG (1 << 3)
#define D_LOG_ALL D_LOG_ERR | D_LOG_NOTICE | D_LOG_INFO | D_LOG_DEBUG

void d_set_log_level(int flags);
int d_should_log_err(void);
int d_should_log_notice(void);
int d_should_log_info(void);
int d_should_log_debug(void);

void _d_datetime(char s[50]);

#define d_log_err(str, ...)                                                   \
    do {                                                                      \
        if (d_should_log_err()) {                                             \
            char s[50];                                                       \
            _d_datetime(s);                                                   \
            fprintf(stderr, "[%s] \033[91m" str "\033[0m", s, ##__VA_ARGS__); \
            fflush(stderr);                                                   \
        }                                                                     \
    } while (0);

#define d_log_notice(str, ...)                                                \
    do {                                                                      \
        if (d_should_log_notice()) {                                          \
            char s[50];                                                       \
            _d_datetime(s);                                                   \
            fprintf(stdout, "[%s] \033[95m" str "\033[0m", s, ##__VA_ARGS__); \
            fflush(stdout);                                                   \
        }                                                                     \
    } while (0);

#define d_log_info(str, ...)                                \
    do {                                                    \
        if (d_should_log_info()) {                          \
            char s[50];                                     \
            _d_datetime(s);                                 \
            fprintf(stdout, "[%s] " str, s, ##__VA_ARGS__); \
            fflush(stdout);                                 \
        }                                                   \
    } while (0);

#define d_log_debug(str, ...)                                                 \
    do {                                                                      \
        if (d_should_log_debug()) {                                           \
            char s[50];                                                       \
            _d_datetime(s);                                                   \
            fprintf(stdout, "[%s] \033[92m" str "\033[0m", s, ##__VA_ARGS__); \
            fflush(stdout);                                                   \
        }                                                                     \
    } while (0);

#endif
