#ifndef DISCO_APPLICATION
#define DISCO_APPLICATION

#include "../../utils/cJSON.h"

struct discord_application {
};

// TODO implement
void *disco_create_application_struct_json(cJSON *data);
void disco_destroy_application(struct discord_application *application);

#endif