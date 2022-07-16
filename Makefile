all: install

LIBS 	= -lcurl -lpthread -lwebsockets
CC 		= gcc
CFLAGS	= -std=gnu11 -g -pedantic -Wall -Wno-conversion \
    -Wextra -Wno-missing-field-initializers -Wformat=2 \
    -Wswitch-default -Wswitch -Wcast-align -Wpointer-arith \
    -Wno-bad-function-cast -Wstrict-prototypes -Winline \
    -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code \
    -Wlogical-op -Wno-float-equal -Wstrict-aliasing=3 -Wredundant-decls \
    -Wold-style-definition -Werror \
    -ggdb3 \
    -O0 \
    -fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing \
    -lm
BUILD	= build
INCLUDE = -Iinclude -Iexternal -I.

WEB_SOURCES := $(wildcard src/web/*.c)
WEB_OBJECTS=$(patsubst src/web/%.c, $(BUILD)/%.o, $(WEB_SOURCES))
$(WEB_OBJECTS): $(BUILD)/%.o : src/web/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

UTILS_SOURCES := $(wildcard src/utils/*.c)
UTILS_OBJECTS=$(patsubst src/utils/%.c, $(BUILD)/%.o, $(UTILS_SOURCES))
$(UTILS_OBJECTS): $(BUILD)/%.o : src/utils/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

DISCORD_SOURCES := $(wildcard src/discord/*.c)
DISCORD_OBJECTS=$(patsubst src/discord/%.c, $(BUILD)/%.o, $(DISCORD_SOURCES))
$(DISCORD_OBJECTS): $(BUILD)/%.o : src/discord/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

example:
	(cd examples/example_bot_1 && make)

install: config.h build $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) cJSON
	ar -rsv libdisco.a $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(BUILD)/cJSON.o

build:
	mkdir -p $(BUILD)

clean: clean_test
	rm -rf $(BUILD)
	rm -f main
	rm -f examples/register

config.h:
	@echo '#define DISCORD_TOKEN "token_placeholder"\n#define APPLICATION_ID "bot/application ID"' > $@

cJSON:
	$(CC) $(CFLAGS) -c $(INCLUDE) external/cJSON/cJSON.c -o $(BUILD)/cJSON.o

test: install
	(cd tests && make)

clean_test:
	rm -f $(TEST_EXECUTABLES)

register: main
	$(CC) $(CFLAGS) $(INCLUDE) register.c\
	$(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(EXAMPLE_OBJECTS)\
	$(COMMAND_OBJECTS) $(BUILD)/cJSON.o $(LIBS) -o $@
