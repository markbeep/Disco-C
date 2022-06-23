#include "../src/discord/disco.h"
#include "../src/utils/disco_logging.h"
#include "../src/web/request.h"
#include "../src/web/websocket.h"
#include <Unity/src/unity.h>

static bot_client_t bot = {0};
static websocket_client_t client = {0};

void test_requests(void) {
    char *r;
    CURLcode c;
    c = request("https://www.google.com/", &r, NULL, REQUEST_GET);
    TEST_ASSERT_EQUAL_INT((int)CURLE_OK, c);
    c = request("https://www.google.com/", &r, NULL, REQUEST_POST);
    TEST_ASSERT_EQUAL_INT((int)CURLE_OK, c);
    c = request("https://www.google.com/", &r, NULL, REQUEST_DELETE);
    TEST_ASSERT_EQUAL_INT((int)CURLE_OK, c);
    c = request("https://www.google.com/", &r, NULL, REQUEST_PATCH);
    TEST_ASSERT_EQUAL_INT((int)CURLE_OK, c);
    c = request("https://www.google.com/", &r, NULL, REQUEST_PUT);
    TEST_ASSERT_EQUAL_INT((int)CURLE_OK, c);
    c = request("https://www.google.com/", &r, NULL, REQUEST_UPDATE);
    TEST_ASSERT_EQUAL_INT((int)CURLE_OK, c);
}

void websocket_on_receive(bot_client_t *bot, char *message, size_t len) {
    (void)bot;
    (void)message;
    (void)len;
}

void test_websocket_create(void) {
    int c = websocket_create(&client, websocket_on_receive);
    TEST_ASSERT_EQUAL_INT(0, c);
    TEST_ASSERT_EQUAL_INT(1, client.active);
    TEST_ASSERT_EQUAL_INT(0, client.connected);
    TEST_ASSERT_EQUAL_INT(0, client.heartbeat_active);
    TEST_ASSERT_EQUAL_INT(0, client.reconnect);
    TEST_ASSERT_EQUAL_INT(0, client.success_login);
}

void test_websocket_connect(void) {
    int c = websocket_connect(&bot);
    TEST_ASSERT_EQUAL_INT(0, c);
    TEST_ASSERT_EQUAL_INT(1, client.active);
    TEST_ASSERT_EQUAL_INT(0, client.connected);
    TEST_ASSERT_EQUAL_INT(0, client.heartbeat_active);
    TEST_ASSERT_EQUAL_INT(0, client.reconnect);
    TEST_ASSERT_EQUAL_INT(0, client.success_login);
    TEST_ASSERT_EQUAL_INT64(NULL, bot.callbacks);
    TEST_ASSERT_EQUAL_INT64(NULL, bot.thread_pool);
    TEST_ASSERT_EQUAL_INT64(NULL, bot.user);
    TEST_ASSERT_EQUAL_INT64((uint64_t)&client, bot.websocket_client);
}

void setUp(void) {
    bot.websocket_client = &client;
    curl_global_init(CURL_GLOBAL_ALL);
}
void tearDown(void) {
    curl_global_cleanup();
}

int main(void) {
    // turns off console printing
    lws_set_log_level(0, NULL);
    d_set_log_level(0);

    UNITY_BEGIN();
    RUN_TEST(test_requests);
    RUN_TEST(test_websocket_create);
    RUN_TEST(test_websocket_connect);
    return UNITY_END();
}
