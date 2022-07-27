#include <discord/application.h>
#include <stdlib.h>

// TODO implement creation of application struct (Low importance)
void *_d_json_to_application(cJSON *data) {
    struct discord_application *app = (struct discord_application *)calloc(1, sizeof(struct discord_application));
    (void)data;
    return app;
}

// TODO implement
struct discord_application *_d_copy_application(struct discord_application *src) {
    (void)src;
    return NULL;
}

void discord_destroy_application(struct discord_application *application) {
    free(application);
}
