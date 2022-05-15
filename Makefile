LIBS 	= -lcurl -lpthread -lwebsockets
CC 		= gcc
CFLAGS	= -Wall -Wextra -Wconversion -pedantic -std=gnu11

target: config.h
	$(CC) -g main.c -I/lib/libwebsockets/include -I/lib/libwebsockets \
	-I/usr/lib/libwebsockets libs/web/request.c libs/web/websocket.c \
	libs/web/gateway.c libs/utils/cJSON.c libs/discord/event.c \
	libs/discord/channel.c libs/discord/disco.c $(CFLAGS) $(LIBS) -o main

config.h:
	@echo '#define DISCORD_TOKEN "token_placeholder"'  > $@