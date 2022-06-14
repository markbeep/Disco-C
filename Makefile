LIBS 	= -lcurl -lpthread -lwebsockets
CC 		= gcc
CFLAGS	= -Wall -Wextra -Wconversion -Wpedantic -std=gnu11 -g
BUILD	= build
TEST = tests
INCLUDE = -Ilib
COMPILE = $(CC) $(INCLUDE) $(CFLAGS) $(LIBS)

WEB_SOURCES := $(wildcard src/web/*.c)
UTILS_SOURCES := $(wildcard src/utils/*.c)
DISCORD_SOURCES := $(wildcard src/discord/*.c)
WEB_OBJECTS=$(patsubst src/web/%.c, $(BUILD)/%.o, $(WEB_SOURCES))
UTILS_OBJECTS=$(patsubst src/utils/%.c, $(BUILD)/%.o, $(UTILS_SOURCES))
DISCORD_OBJECTS=$(patsubst src/discord/%.c, $(BUILD)/%.o, $(DISCORD_SOURCES))

# COMPILES THE EXAMPLE FILES
EXAMPLE_SOURCES := $(wildcard example/*.c)
EXAMPLE_OBJECTS=$(patsubst example/%.c, $(BUILD)/%.o, $(EXAMPLE_SOURCES))

all: main

main: config.h build main.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(EXAMPLE_OBJECTS) cJSON
	$(CC) $(CFLAGS) $(INCLUDE) main.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(EXAMPLE_OBJECTS) $(BUILD)/cJSON.o $(LIBS) -o $@

$(WEB_OBJECTS): $(BUILD)/%.o : src/web/%.c src/utils/disco_logging.h
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

$(UTILS_OBJECTS): $(BUILD)/%.o : src/utils/%.c src/utils/disco_logging.h
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

$(DISCORD_OBJECTS): $(BUILD)/%.o : src/discord/%.c src/utils/disco_logging.h
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

# COMPILES THE EXAMPLE FILES
$(EXAMPLE_OBJECTS): $(BUILD)/%.o : example/%.c src/utils/disco_logging.h
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

build:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
	rm -f main

config.h:
	@echo '#define DISCORD_TOKEN "token_placeholder"' > $@

cJSON:
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) lib/cJSON/cJSON.c -o $(BUILD)/cJSON.o

test: build $(TEST)/test_web.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) cJSON
	$(CC) $(INCLUDE) $(TEST)/test_web.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(BUILD)/cJSON.o lib/Unity/src/unity.c $(LIBS) -o $(TEST)/test
