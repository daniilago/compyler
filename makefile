CC     = gcc
CFLAGS = -Wall -g
SRC    = main.c lexer.c reader.c

all: $(SRC)
	$(CC) $(CFLAGS) -o main $(SRC)

run: all
	./main input.txt

clean:
	rm -f main

.PHONY: all run clean