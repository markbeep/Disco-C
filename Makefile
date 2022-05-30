LIBS 	= -lcurl -lpthread -lwebsockets
CC 		= gcc
CFLAGS	= -Wall -Wextra -Wconversion -Wpedantic -std=gnu11 -g
BUILD	= build
INCLUDE = -I/lib/libwebsockets -I/usr/lib/libwebsockets -Ilibs
COMPILE = $(CC) $(INCLUDE) $(CFLAGS) $(LIBS)

WEB_SOURCES := $(wildcard libs/web/*.c)
UTILS_SOURCES := $(wildcard libs/utils/*.c)
DISCORD_SOURCES := $(wildcard libs/discord/*.c)
WEB_OBJECTS=$(patsubst libs/web/%.c, $(BUILD)/%.o, $(WEB_SOURCES))
UTILS_OBJECTS=$(patsubst libs/utils/%.c, $(BUILD)/%.o, $(UTILS_SOURCES))
DISCORD_OBJECTS=$(patsubst libs/discord/%.c, $(BUILD)/%.o, $(DISCORD_SOURCES))

# COMPILES THE EXAMPLE FILES
EXAMPLE_SOURCES := $(wildcard example/*.c)
EXAMPLE_OBJECTS=$(patsubst example/%.c, $(BUILD)/%.o, $(EXAMPLE_SOURCES))

all: main

main: config.h build main.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(EXAMPLE_OBJECTS) cJSON
	$(CC) $(CFLAGS) $(INCLUDE) main.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(EXAMPLE_OBJECTS) $(BUILD)/cJSON.o $(LIBS) -o $@

$(WEB_OBJECTS): $(BUILD)/%.o : libs/web/%.c libs/utils/disco_logging.h
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

$(UTILS_OBJECTS): $(BUILD)/%.o : libs/utils/%.c libs/utils/disco_logging.h
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

$(DISCORD_OBJECTS): $(BUILD)/%.o : libs/discord/%.c libs/utils/disco_logging.h
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

# COMPILES THE EXAMPLE FILES
$(EXAMPLE_OBJECTS): $(BUILD)/%.o : example/%.c libs/utils/disco_logging.h
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

build:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
	rm -f main

config.h:
	@echo '#define DISCORD_TOKEN "token_placeholder"' > $@

cJSON:
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) libs/utils/cJSON/cJSON.c -o $(BUILD)/cJSON.o
