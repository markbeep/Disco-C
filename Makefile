LIBS 	= -lcurl -lpthread -lwebsockets
CC 		= gcc
CFLAGS	= -Wall -Wextra -Wconversion -pedantic -std=gnu11 -g
BUILD	= build
INCLUDE = -I/lib/libwebsockets -I/usr/lib/libwebsockets -Ilibs
COMPILE = $(CC) $(INCLUDE) $(CFLAGS) $(LIBS)

WEB_SOURCES := $(wildcard libs/web/*.c)
UTILS_SOURCES := $(wildcard libs/utils/*.c)
DISCORD_SOURCES := $(wildcard libs/discord/*.c)
WEB_OBJECTS=$(patsubst libs/web/%.c, $(BUILD)/%.o, $(WEB_SOURCES))
UTILS_OBJECTS=$(patsubst libs/utils/%.c, $(BUILD)/%.o, $(UTILS_SOURCES))
DISCORD_OBJECTS=$(patsubst libs/discord/%.c, $(BUILD)/%.o, $(DISCORD_SOURCES))

all: main

main: config.h build $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE) main.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(LIBS) -o $@

$(WEB_OBJECTS): $(BUILD)/%.o : libs/web/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

$(UTILS_OBJECTS): $(BUILD)/%.o : libs/utils/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

$(DISCORD_OBJECTS): $(BUILD)/%.o : libs/discord/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

build:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
	rm main

config.h:
	@echo '#define DISCORD_TOKEN "token_placeholder"' > $@