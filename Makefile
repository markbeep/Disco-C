LIBS 	= -lcurl -lpthread -lwebsockets
CC 		= gcc
CFLAGS	= -Wall -Wextra -Wconversion -pedantic -std=gnu11

target:
	$(CC) -g main.c -I/lib/libwebsockets/include -I/lib/libwebsockets -I/usr/lib/libwebsockets libs/web/request.c libs/web/websocket.c libs/web/gateway.c libs/utils/cJSON.c $(CFLAGS) $(LIBS) -o main