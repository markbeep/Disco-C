all: main

LIBSPATH= -L..
LIBS 	= -ldisco -lcurl -lpthread -lwebsockets
CC 		= gcc
CFLAGS	= -std=gnu11 -g -pedantic -Wall -Wnoonversion \
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
INCLUDE = -I../include -I../external -I..

TEST_SOURCES := $(wildcard *.c)
TEST_EXECUTABLES=$(patsubst %.c, %, $(TEST_SOURCES))
$(TEST_EXECUTABLES): % : %.c
	$(CC) -zmuldefs $@.c $(CFLAGS) $(INCLUDE) ../external/Unity/src/unity.c -o $@ $(LIBSPATH) $(LIBS)

main: $(TEST_EXECUTABLES)
