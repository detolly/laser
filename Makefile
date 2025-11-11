CC      := gcc
CFLAGS  := -Wall -Wextra -std=c23 -Iinclude -g
LDFLAGS := -lm
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:.c=.o)
TARGET  := bin/laser

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
