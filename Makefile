LIBS 	= -lcurl
CC 		= gcc

target:
	$(CC) main.c libs/web/request.c $(CFLAGS) $(LIBS) -o main