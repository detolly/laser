CFLAGS += -Wall -Wextra -std=c99 -Iinclude
BINARIES = motor_test projection_test display_test motor2_test galvo_test spi_test
SOURCES = src/motor.c src/grid.c src/picture.c src/config.c src/gpio.c src/display.c
OBJ_DIR = target/obj

ifeq ($(DEBUG),1)
CFLAGS += -DLASER_DEBUG -g
else
CFLAGS += -O3
LDFLAGS += -flto
endif

# Host (x86_64)
HOST_CC = gcc
HOST_CFLAGS = $(CFLAGS) -march=native
HOST_LDFLAGS = $(LDFLAGS) -lm
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
DEV_LDFLAGS = $(LDFLAGS) -lm -Llib/pigpio -lpigpio
DEV_BIN_DIR = target/device
DEV_OBJ = $(SOURCES:%.c=$(OBJ_DIR)/%_device.o)
DEV_BINARIES = $(BINARIES:%=$(DEV_BIN_DIR)/%)

all: host device

host: $(HOST_BINARIES) | $(HOST_BIN_DIR)
device: $(DEV_BINARIES) | $(DEV_BIN_DIR)

$(HOST_BIN_DIR) $(DEV_BIN_DIR): | $(OBJ_DIR)/src
	mkdir -p $@

$(OBJ_DIR)/src:
	mkdir -p $@

$(HOST_BIN_DIR)/%: $(HOST_OBJ) $(OBJ_DIR)/src/%_host.o | $(OBJ_DIR)/src $(HOST_BIN_DIR)
	$(HOST_CC) $^ $(HOST_LDFLAGS) -MMD -MP -o $@

$(DEV_BIN_DIR)/%: $(DEV_OBJ) $(OBJ_DIR)/src/%_device.o | $(OBJ_DIR)/src $(DEV_BIN_DIR)
	$(DEV_CC) $(DEV_LDFLAGS) -MMD -MP $^ -o $@

.SECONDARY:
$(OBJ_DIR)/%_host.o: %.c | $(OBJ_DIR)/src
	$(HOST_CC) $(HOST_CFLAGS) -MMD -MP -c $< -o $@

.SECONDARY:
$(OBJ_DIR)/%_device.o: %.c | $(OBJ_DIR)/src
	$(DEV_CC) $(DEV_CFLAGS) -MMD -MP -c $< -o $@

-include $(wildcard target/obj/src/*.d)

transfer: device
	rsync -chavP --stats $(DEV_BIN_DIR)/* thomas@pizero:/home/thomas/laser-build

clean:
	rm -rf target
