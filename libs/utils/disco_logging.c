#include "disco_logging.h"

static int log_level = 0;

void d_set_log_level(int flags) {
    log_level = flags;
}

int d_should_log_err() {
    return log_level & D_LOG_ERR;
}
int d_should_log_notice() {
    return log_level & D_LOG_NOTICE;
}
int d_should_log_normal() {
    return log_level & D_LOG_NORMAL;
}
int d_should_log_debug() {
    return log_level & D_LOG_DEBUG;
}
