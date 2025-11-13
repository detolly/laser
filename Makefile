CC      := gcc
CFLAGS  := -Wall -Wextra -std=c23 -Iinclude -O3 -march=native -DLASER_DEBUG=1
LDFLAGS := -lm
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:.c=.o)

all: bin/projection_test

bin/projection_test: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
