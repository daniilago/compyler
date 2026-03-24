CC      = gcc
CFLAGS  = -Wall -Wextra -g

SRC     = main.c lexer.c reader.c
OBJ     = $(SRC:.c=.o)
TARGET  = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET) input.txt

.PHONY: all clean run