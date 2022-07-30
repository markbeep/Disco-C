#include <string.h>
#include <utils/disco_logging.h>

static int log_level = 0;

void d_set_log_level(int flags) {
    log_level = flags;
}

int d_should_log_err(void) {
    return log_level & D_LOG_ERR;
}
int d_should_log_notice(void) {
    return log_level & D_LOG_NOTICE;
}
int d_should_log_normal(void) {
    return log_level & D_LOG_NORMAL;
}
int d_should_log_debug(void) {
    return log_level & D_LOG_DEBUG;
}

void _d_datetime(char s[50]) {
    time_t t = time(NULL);
    struct tm dt = *localtime(&t);
    sprintf(s, "%d-%02d-%02d %02d:%02d:%02d",
            dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
}
