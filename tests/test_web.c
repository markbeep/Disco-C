#include "../src/utils/disco_logging.h"
#include "../src/web/request.h"
#include "../src/web/websocket.h"
#include <Unity/src/unity.h>
#include <stdio.h>

static bot_client_t bot = {0};
static websocket_client_t client = {0};
void setUp(void) {
    bot.websocket_client = &client;
    curl_global_init(CURL_GLOBAL_ALL);
}
void tearDown(void) {
    curl_global_cleanup();
}

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
    TEST_ASSERT_EQUAL_INT(c, 0);
    TEST_ASSERT_EQUAL_INT(client.active, 1);
    TEST_ASSERT_EQUAL_INT(client.connected, 0);
    TEST_ASSERT_EQUAL_INT(client.heartbeat_active, 0);
    TEST_ASSERT_EQUAL_INT(client.reconnect, 0);
    TEST_ASSERT_EQUAL_INT(client.success_login, 0);
}

void test_websocket_connect(void) {
    int c = websocket_connect(&bot);
    TEST_ASSERT_EQUAL_INT(c, 0);
    TEST_ASSERT_EQUAL_INT(client.active, 1);
    TEST_ASSERT_EQUAL_INT(client.connected, 0);
    TEST_ASSERT_EQUAL_INT(client.heartbeat_active, 0);
    TEST_ASSERT_EQUAL_INT(client.reconnect, 0);
    TEST_ASSERT_EQUAL_INT(client.success_login, 0);
    TEST_ASSERT_EQUAL_INT64(bot.callbacks, NULL);
    TEST_ASSERT_EQUAL_INT64(bot.thread_pool, NULL);
    TEST_ASSERT_EQUAL_INT64(bot.user, NULL);
    TEST_ASSERT_EQUAL_INT64(bot.websocket_client, (int64_t)&client);
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
