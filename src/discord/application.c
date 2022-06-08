#include "structures/application.h"
#include <stdlib.h>

// TODO implement creation of application struct (Low importance)
void *disco_create_application_struct_json(cJSON *data) {
    struct discord_application *app = (struct discord_application *)calloc(1, sizeof(struct discord_application));
    (void)data;
    return app;
}

void disco_destroy_application(struct discord_application *application) {
    free(application);
}