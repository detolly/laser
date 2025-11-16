CFLAGS += -Wall -Wextra -std=c99 -Iinclude
BINARIES = motor_test projection_test
SOURCES = src/motor.c src/grid.c src/picture.c src/config.c
OBJ_DIR = obj

ifeq ($(DEBUG),1)
CFLAGS += -DLASER_DEBUG
else
CFLAGS += -O3
LDFLAGS += -flto
endif

# Host (x86_64)
HOST_CC = gcc
HOST_CFLAGS = $(CFLAGS) -march=native
HOST_LDFLAGS += $(LDFLAGS) -lm
HOST_BIN_DIR = target/host
HOST_OBJ = $(SOURCES:%.c=$(OBJ_DIR)/%_host.o)
HOST_BINARIES = $(BINARIES:%=$(HOST_BIN_DIR)/%)

# Host (aarch64)
ifeq ($(HOST_DEVICE),1)
HOST_CFLAGS += -DLASER_DEVICE
HOST_LDFLAGS += -lpigpio
endif

# Device (aarch64)
DEV_CC = aarch64-linux-gnu-gcc
DEV_CFLAGS = $(CFLAGS) -mcpu=cortex-a72 -march=armv8-a+fp+simd  -Ilib/pigpio -DLASER_DEVICE
DEV_LDFLAGS = -lm -flto -Llib/pigpio -lpigpio
DEV_BIN_DIR = target/device
DEV_OBJ = $(SOURCES:%.c=$(OBJ_DIR)/%_device.o)
DEV_BINARIES = $(BINARIES:%=$(DEV_BIN_DIR)/%)

all: host device

host: $(HOST_BINARIES) $(HOST_BIN_DIR)/combined | $(HOST_BIN_DIR)
device: $(DEV_BINARIES) $(DEV_BIN_DIR)/combined | $(DEV_BIN_DIR)

$(HOST_BIN_DIR):
	mkdir -p $@

$(DEV_BIN_DIR):
	mkdir -p $@

$(OBJ_DIR)/src:
	mkdir -p $@

$(HOST_BIN_DIR)/combined: src/combined.c
	$(HOST_CC) $(HOST_CFLAGS) $< $(HOST_LDFLAGS)  -o $@
$(DEV_BIN_DIR)/combined: src/combined.c
	$(DEV_CC) $(DEV_LDFLAGS) $(DEV_CFLAGS) $< -o $@

$(HOST_BIN_DIR)/%: $(HOST_OBJ) $(OBJ_DIR)/src/%_host.o | $(HOST_BIN_DIR)
	$(HOST_CC) $^ $(HOST_LDFLAGS) -o $@

$(DEV_BIN_DIR)/%: $(DEV_OBJ) $(OBJ_DIR)/src/%_device.o | $(DEV_BIN_DIR)
	$(DEV_CC) $(DEV_LDFLAGS) $^ -o $@

$(OBJ_DIR)/%_host.o: %.c | $(OBJ_DIR)/src
	$(HOST_CC) $(HOST_CFLAGS) -c $< -o $@

$(OBJ_DIR)/%_device.o: %.c | $(OBJ_DIR)/src
	$(DEV_CC) $(DEV_CFLAGS) -c $< -o $@

transfer: device
	scp -r $(DEV_BIN_DIR) thomas@pi:/home/thomas/laser-build

clean:
	rm -rf src/*.o $(HOST_BIN_DIR)/* $(DEV_BIN_DIR)/* $(OBJ_DIR)/*
