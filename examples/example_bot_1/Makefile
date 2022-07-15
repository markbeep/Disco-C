LIBSPATH= -L../..
LIBS 	= -ldisco -lcurl -lpthread -lwebsockets
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
INCLUDE = -I../../include -I../../external -I../..

main: main.c example_slash_commands/example_hello.c example_slash_commands/example_ping.c
	$(CC) $(INCLUDE) main.c example_slash_commands/example_hello.c example_slash_commands/example_ping.c -o $@ $(LIBSPATH) $(LIBS)
