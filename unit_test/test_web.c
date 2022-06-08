#include "../src/web/request.h"
#include "../src/web/websocket.h"
#include <stdio.h>
#include <unity/src/unity.h>

void setUp(void) {}
void tearDown(void) {}

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
    websocket_client_t client = {0};
    int c = websocket_create(&client, websocket_on_receive);
    TEST_ASSERT_EQUAL_INT(c, 0);
    TEST_ASSERT_EQUAL_INT(client.active, 0);
    TEST_ASSERT_EQUAL_INT(client.connected, 0);
    TEST_ASSERT_EQUAL_INT(client.heartbeat_active, 0);
    TEST_ASSERT_EQUAL_INT(client.reconnect, 0);
    TEST_ASSERT_EQUAL_INT(client.success_login, 0);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_requests);
    RUN_TEST(test_websocket_create);
    return UNITY_END();
}
