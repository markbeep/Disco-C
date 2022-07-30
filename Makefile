all: install
main: install

CC 		= gcc
DEVFLAGS	= -std=gnu11 -g -pedantic -Wall -Wno-conversion \
    -Wextra -Wno-missing-field-initializers -Wformat=2 \
    -Wswitch-default -Wswitch -Wcast-align -Wpointer-arith \
    -Wno-bad-function-cast -Wstrict-prototypes -Winline \
    -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code \
    -Wlogical-op -Wno-float-equal -Wstrict-aliasing=3 -Wredundant-decls \
    -Wold-style-definition -Werror -ggdb3 -O0 \
    -fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing -lm
MAINFLAGS = -O3
BUILD	= build
INCLUDE = -Iinclude -Iexternal -I.

####################################

# MAIN

####################################

WEB_SOURCES := $(wildcard src/web/*.c)
WEB_OBJECTS=$(patsubst src/web/%.c, $(BUILD)/web-%.o, $(WEB_SOURCES))
$(WEB_OBJECTS): $(BUILD)/web-%.o : src/web/%.c
	$(CC) $(MAINFLAGS) -c $(INCLUDE) $< -o $@

UTILS_SOURCES := $(wildcard src/utils/*.c)
UTILS_OBJECTS=$(patsubst src/utils/%.c, $(BUILD)/utils-%.o, $(UTILS_SOURCES))
$(UTILS_OBJECTS): $(BUILD)/utils-%.o : src/utils/%.c
	$(CC) $(MAINFLAGS) -c $(INCLUDE) $< -o $@

DISCORD_SOURCES := $(wildcard src/discord/*.c)
DISCORD_OBJECTS=$(patsubst src/discord/%.c, $(BUILD)/discord-%.o, $(DISCORD_SOURCES))
$(DISCORD_OBJECTS): $(BUILD)/discord-%.o : src/discord/%.c
	$(CC) $(MAINFLAGS) -c $(INCLUDE) $< -o $@

####################################

# DEV

####################################
DEV_WEB_SOURCES := $(wildcard src/web/*.c)
DEV_WEB_OBJECTS=$(patsubst src/web/%.c, $(BUILD)/dev-web-%.o, $(DEV_WEB_SOURCES))
$(DEV_WEB_OBJECTS): $(BUILD)/dev-web-%.o : src/web/%.c
	$(CC) $(DEVFLAGS) -c $(INCLUDE) $< -o $@

DEV_UTILS_SOURCES := $(wildcard src/utils/*.c)
DEV_UTILS_OBJECTS=$(patsubst src/utils/%.c, $(BUILD)/dev-utils-%.o, $(DEV_UTILS_SOURCES))
$(DEV_UTILS_OBJECTS): $(BUILD)/dev-utils-%.o : src/utils/%.c
	$(CC) $(DEVFLAGS) -c $(INCLUDE) $< -o $@

DEV_DISCORD_SOURCES := $(wildcard src/discord/*.c)
DEV_DISCORD_OBJECTS=$(patsubst src/discord/%.c, $(BUILD)/dev-discord-%.o, $(DEV_DISCORD_SOURCES))
$(DEV_DISCORD_OBJECTS): $(BUILD)/dev-discord-%.o : src/discord/%.c
	$(CC) $(DEVFLAGS) -c $(INCLUDE) $< -o $@

example:
	(cd examples/example_bot_1 && make)


install: build $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) cJSON
	ar -rsv libdisco.a $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(BUILD)/cJSON.o

dev: build $(DEV_WEB_OBJECTS) $(DEV_UTILS_OBJECTS) $(DEV_DISCORD_OBJECTS) cJSON
	ar -rsv libdevdisco.a $(DEV_WEB_OBJECTS) $(DEV_UTILS_OBJECTS) $(DEV_DISCORD_OBJECTS) $(BUILD)/cJSON.o

build:
	mkdir -p $(BUILD)

clean: clean_test
	rm -rf $(BUILD)
	rm -f libdisco.a
	rm -f libdevdisco.a

cJSON:
	$(CC) $(CFLAGS) -c $(INCLUDE) external/cJSON/cJSON.c -o $(BUILD)/cJSON.o

test: install
	(cd tests && make)

clean_test:
	rm -f $(TEST_EXECUTABLES)
