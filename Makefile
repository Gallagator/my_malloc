CC     = gcc
CFLAGS = -Werror -std=c99 -pedantic -D_DEFAULT_SOURCE -g
BUILD  = malloc.o main

all: $(BUILD)

clean:
	/bin/rm -f $(BUILD)

main: malloc.o src/main.c
	$(CC) $(CFLAGS) src/main.c malloc.o -o main

malloc.o: src/malloc/mem.c src/malloc/mem.h
	$(CC) $(CFLAGS) -c src/malloc/mem.c -o malloc.o

