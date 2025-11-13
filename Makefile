CC       = gcc
CFLAGS   = -Wall -Wextra -std=c23 -Iinclude -march=native -DLASER_DEBUG=1 -O3
LDFLAGS  = -lm -flto

BINARIES = bin/projection_test bin/motor_test
SOURCES  = config.c picture.c motor.c laser_math.c
COMMON   = $(SOURCES:%.c=src/%.o)
BIN_OBJ  = $(BINARIES:bin/%=src/%.o)

all: $(BINARIES)

$(BINARIES): $(COMMON) $(BIN_OBJ)
	$(CC) $(LDFLAGS) $(COMMON) $(@:bin/%=src/%.o) -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(BINARIES)
