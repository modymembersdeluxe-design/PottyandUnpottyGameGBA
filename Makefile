# Minimal devkitARM/libgba Makefile for a GBA ROM

TARGET := potty_unpotty
BUILD := build
SRC := src
INCLUDE := include

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy

CFLAGS := -mthumb -mthumb-interwork -mcpu=arm7tdmi -O2 -Wall -Wextra -ffreestanding -I$(INCLUDE)
LDFLAGS := -mthumb -mthumb-interwork -mcpu=arm7tdmi -specs=gba.specs
LIBS := -lgba

SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SRCS))

all: $(BUILD)/$(TARGET).gba

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

$(BUILD)/$(TARGET).gba: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD)

.PHONY: all clean
