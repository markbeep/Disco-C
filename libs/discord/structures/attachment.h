#ifndef DISCO_ATTACHMENT
#define DISCO_ATTACHMENT

#include "../../utils/cJSON.h"

struct discord_attachment {
};

// TODO implement
struct discord_attachment *disco_create_attachment_struct_json(cJSON *data);
void disco_destroy_attachment(struct discord_attachment *attachment);

#endif