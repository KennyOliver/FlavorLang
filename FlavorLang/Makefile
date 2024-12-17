CC = gcc
CFLAGS = -Wall -Wextra -g

all: flavor

flavor: main.c tokenizer.c
	$(CC) $(CFLAGS) -o flavor main.c tokenizer.c

clean:
	rm -f flavor
