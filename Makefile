CC       = aarch64-linux-gnu-gcc
CFLAGS   = -Wall -Wextra -std=c23 -Iinclude -march=armv8-a+fp+simd -mcpu=cortex-a72 -O3 -DLASER_DEBUG
LDFLAGS  = -lm -flto

BINARIES = bin/projection_test bin/motor_test
SOURCES  = config.c picture.c motor.c laser_math.c
COMMON   = $(SOURCES:%.c=src/%.o)
BIN_OBJ  = $(BINARIES:bin/%=src/%.o)

all: $(BINARIES) bin/combined

transfer: $(BINARIES) bin/combined
	scp -r bin thomas@pi:/home/thomas/laser

bin/combined:
	$(CC) $(LDFLAGS) $(CFLAGS) src/combined.c -o bin/combined

$(BINARIES): $(COMMON) $(BIN_OBJ)
	$(CC) $(LDFLAGS) $(COMMON) $(@:bin/%=src/%.o) -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(BINARIES)
