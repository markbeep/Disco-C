all: main

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
TEST = tests
INCLUDE = -Ilib
COMPILE = $(CC) $(INCLUDE) $(CFLAGS) $(LIBS)

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

# COMPILES THE TEST FILES
TEST_SOURCES := $(wildcard $(TEST)/*.c)
TEST_EXECUTABLES=$(patsubst $(TEST)/%.c, $(TEST)/%, $(TEST_SOURCES))
$(TEST_EXECUTABLES): $(TEST)/% : $(TEST)/%.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS)
	$(CC) -zmuldefs $(CFLAGS) $(INCLUDE) $@.c $(WEB_OBJECTS) \
	$(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(TEST_SOURCES) \
	$(BUILD)/cJSON.o lib/Unity/src/unity.c $(LIBS) -o $@

# COMPILES THE EXAMPLE FILES
EXAMPLE_SOURCES := $(wildcard examples/*.c)
EXAMPLE_OBJECTS=$(patsubst examples/%.c, $(BUILD)/%.o, $(EXAMPLE_SOURCES))
$(EXAMPLE_OBJECTS): $(BUILD)/%.o : examples/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

# COMPILES THE EXAMPLE COMMAND FILES
COMMAND_SOURCES := $(wildcard examples/example_slash_commands/*.c)
COMMAND_OBJECTS=$(patsubst examples/example_slash_commands/%.c, $(BUILD)/%.o, $(COMMAND_SOURCES))
$(COMMAND_OBJECTS): $(BUILD)/%.o : examples/example_slash_commands/%.c
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) $< -o $@

main: config.h build main.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(EXAMPLE_OBJECTS) $(COMMAND_OBJECTS) cJSON
	$(CC) $(CFLAGS) $(INCLUDE) main.c $(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(EXAMPLE_OBJECTS) $(COMMAND_OBJECTS) $(BUILD)/cJSON.o $(LIBS) -o $@

build:
	mkdir -p $(BUILD)

clean: clean_test
	rm -rf $(BUILD)
	rm -f main
	rm -f examples/register

config.h:
	@echo '#define DISCORD_TOKEN "token_placeholder"\n#define APPLICATION_ID "bot/application ID"' > $@

cJSON:
	$(CC) $(CFLAGS) -c $(INCLUDE) $(LIBS) lib/cJSON/cJSON.c -o $(BUILD)/cJSON.o

test: main $(TEST_EXECUTABLES)

clean_test:
	rm -f $(TEST_EXECUTABLES)

register: main
	$(CC) $(CFLAGS) $(INCLUDE) register.c\
	$(WEB_OBJECTS) $(UTILS_OBJECTS) $(DISCORD_OBJECTS) $(EXAMPLE_OBJECTS)\
	$(COMMAND_OBJECTS) $(BUILD)/cJSON.o $(LIBS) -o $@
