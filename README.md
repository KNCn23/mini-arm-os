# mini-arm-os

A tiny, readable bare-metal operating system for AArch64, written from scratch in C and assembly. Boots on QEMU's `virt` machine, prints a banner over PL011 UART, and drops the user into an interactive shell.

## What it does

- Boots on a Cortex-A72 (QEMU virt) and parks all secondary cores
- Initializes a 64 KB stack and zeroes the `.bss` section
- Drives the PL011 UART for blocking + non-blocking serial I/O
- Reads the AArch64 system timer (`CNTPCT_EL0`) for uptime + delays
- Provides a 1 MiB bump allocator (`kmalloc`)
- Runs a built-in shell with line editing (backspace, echo, ANSI clear)

## Architecture

```
mini-arm-os/
├── boot/
│   └── startup.S        # _start: stack setup, .bss zero, branch to kmain
├── kernel/
│   └── kernel.c         # kmain: init drivers, hand off to shell
├── drivers/
│   ├── uart.[ch]        # PL011 driver — putc, puts, getc, hex/dec print
│   ├── timer.[ch]       # ARM generic timer — ticks, ms, delay
│   └── mem.[ch]         # memset/memcpy + bump allocator
├── shell/
│   └── shell.[ch]       # Line editor + command dispatcher
├── include/
│   └── types.h          # u8/u16/u32/u64, size_t, NULL
├── linker.ld            # Memory layout: text at 0x40080000, 64KB stack
└── Makefile
```

## Build

You need an AArch64 cross-compiler and QEMU:

```bash
# macOS
brew install aarch64-elf-gcc qemu

# Debian / Ubuntu
sudo apt install gcc-aarch64-linux-gnu qemu-system-arm
# then build with the right prefix:
make CROSS=aarch64-linux-gnu-
```

Then:

```bash
make
make run        # boots in QEMU, -nographic mode
```

Exit QEMU with `Ctrl-A` then `x`.

## Shell commands

```
mini> help
Available commands:
  help      Show this message
  echo ...  Print arguments
  clear     Clear the screen (ANSI)
  uptime    Show milliseconds since boot
  mem       Show heap usage
  ticks     Show timer ticks and frequency
  banner    Reprint the boot banner
  reboot    Halt the CPU
```

## Memory map

| Region | Address | Notes |
|---|---|---|
| Kernel image | `0x40080000` | QEMU `-kernel` load address |
| PL011 UART | `0x09000000` | QEMU virt MMIO |
| Stack | grows down from `_stack_top` | 64 KB |
| Heap | `.bss` static array | 1 MiB bump allocator |

## Debugging

```bash
make debug          # starts QEMU paused on port 1234
# in another shell:
aarch64-elf-gdb kernel.elf
(gdb) target remote :1234
(gdb) break kmain
(gdb) continue
```

## Possible next steps

- **GIC + interrupt vectors** — replace polling with IRQ-driven UART RX
- **MMU + page tables** — set up identity-mapped TTBR0 and enable caches
- **VirtIO block device** — read sectors via the QEMU virtio-blk transport
- **Cooperative scheduler** — multiple kernel tasks sharing the CPU
- **VFS + RAM filesystem** — basic `open`/`read`/`write` and a few files

## License

MIT
