CC = gcc
CFLAGS = -Wall -Iinclude

all:
	$(CC) $(CFLAGS) src/main.c -o build/main

run:
	./build/main

clean:
	rm -f build/main