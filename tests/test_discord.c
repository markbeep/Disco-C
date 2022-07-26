#include <Unity/src/unity.h>
#include <discord/component.h>
#include <discord/disco.h>
#include <discord/message.h>
#include <stdbool.h>
#include <utils/disco_logging.h>

void test_message_creation(void) {
    struct discord_create_message msg = {0}, unmod = {0};
    cJSON *json, *j;

    // test for an empty created message
    json = cJSON_CreateObject();
    _d_create_message_to_json(json, NULL, &msg);
    TEST_ASSERT_EQUAL_INT(false, get_bool_from_json(json, "tts", false));
    j = cJSON_GetObjectItem(json, "content");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "embeds");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "allowed_mentions");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "message_reference");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "components");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "sticker_ids");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "files");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "payload_json");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "attachments");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    // make sure the struct is unmodified
    TEST_ASSERT_EQUAL_MEMORY(&unmod, &msg, sizeof(unmod));
    cJSON_Delete(json);

    // test for message with a few stuff in it
    json = cJSON_CreateObject();
    msg.tts = true;
    char *content = "test message";
    uint64_t sticker_ids[3] = {123, 456, 789};
    msg.sticker_ids = sticker_ids;
    msg.sticker_ids_count = 3;
    msg.flags = 1 << 4;
    _d_create_message_to_json(json, content, &msg);
    TEST_ASSERT_EQUAL_INT(true, get_bool_from_json(json, "tts", true));
    j = cJSON_GetObjectItem(json, "content");
    TEST_ASSERT_EQUAL_STRING_LEN("test message", cJSON_GetStringValue(j), 13);
    j = cJSON_GetObjectItem(json, "embeds");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "allowed_mentions");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "message_reference");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "sticker_ids");
    TEST_ASSERT_EQUAL_INT(3, cJSON_GetArraySize(j));
    j = cJSON_GetObjectItem(json, "files");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "payload_json");
    TEST_ASSERT_EQUAL_INT64(NULL, j);
    j = cJSON_GetObjectItem(json, "attachments");
    TEST_ASSERT_EQUAL_INT64(NULL, j);

    cJSON_Delete(json);
}

void test_embed_creation(void) {
    // struct discord_create_message msg = {0};
    // cJSON *json = cJSON_CreateObject(), *j;
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    // turns off console printing
    lws_set_log_level(0, NULL);
    d_set_log_level(0);

    UNITY_BEGIN();
    RUN_TEST(test_message_creation);
    return UNITY_END();
}
