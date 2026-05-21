# Auto-detect cross-compiler if CROSS isn't explicitly set.
ifeq ($(origin CROSS),undefined)
  ifneq ($(shell command -v aarch64-elf-gcc 2>/dev/null),)
    CROSS := aarch64-elf-
  else ifneq ($(shell command -v aarch64-unknown-linux-gnu-gcc 2>/dev/null),)
    CROSS := aarch64-unknown-linux-gnu-
  else ifneq ($(shell command -v aarch64-linux-gnu-gcc 2>/dev/null),)
    CROSS := aarch64-linux-gnu-
  else
    $(error No AArch64 cross-compiler found. Install one of: aarch64-elf-gcc, aarch64-unknown-linux-gnu-gcc, aarch64-linux-gnu-gcc)
  endif
endif

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

.PHONY: all clean run debug info

all: $(IMG)

info:
	@echo "Using cross-compiler prefix: $(CROSS)"

$(TARGET): $(OBJS) linker.ld
	$(LD) -T linker.ld -o $@ $(OBJS)

$(IMG): $(TARGET)
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

run: $(TARGET)
	@echo ""
	@echo "Booting mini-arm-os in QEMU. Exit with:  Ctrl-A  then  x"
	@echo ""
	qemu-system-aarch64 -M virt -cpu cortex-a72 -m 256M \
	  -nographic -kernel $(TARGET)

debug: $(TARGET)
	qemu-system-aarch64 -M virt -cpu cortex-a72 -m 256M \
	  -nographic -kernel $(TARGET) -s -S

clean:
	rm -f $(OBJS) $(TARGET) $(IMG)
