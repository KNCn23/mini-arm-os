# Cross-compiler. Override with `make CROSS=...` if needed.
CROSS   ?= aarch64-elf-
# Linux distros typically ship the GNU toolchain as aarch64-linux-gnu-
# CROSS ?= aarch64-linux-gnu-

CC      = $(CROSS)gcc
LD      = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

CFLAGS  = -ffreestanding -fno-builtin -fno-stack-protector \
          -nostdlib -mcpu=cortex-a72 -Wall -Wextra -O2 \
          -Iinclude -Idrivers -Ishell
ASFLAGS = -mcpu=cortex-a72

SRC_S   = boot/startup.S
SRC_C   = kernel/kernel.c \
          drivers/uart.c drivers/timer.c drivers/mem.c \
          shell/shell.c

OBJS    = $(SRC_S:.S=.o) $(SRC_C:.c=.o)

TARGET  = kernel.elf
IMG     = kernel.img

.PHONY: all clean run debug

all: $(IMG)

$(TARGET): $(OBJS) linker.ld
	$(LD) -T linker.ld -o $@ $(OBJS)

$(IMG): $(TARGET)
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

run: $(TARGET)
	qemu-system-aarch64 -M virt -cpu cortex-a72 -m 256M \
	  -nographic -kernel $(TARGET)

debug: $(TARGET)
	qemu-system-aarch64 -M virt -cpu cortex-a72 -m 256M \
	  -nographic -kernel $(TARGET) -s -S

clean:
	rm -f $(OBJS) $(TARGET) $(IMG)
