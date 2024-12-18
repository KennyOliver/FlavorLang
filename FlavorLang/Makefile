CC = gcc
CFLAGS = -Wall -Wextra -g

all: flavor

flavor: main.c tokenizer.c parser.c
	$(CC) $(CFLAGS) -o flavor main.c tokenizer.c parser.c

clean:
	rm -f flavor
