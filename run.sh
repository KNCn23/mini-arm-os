#!/usr/bin/env bash
# Zero-config launcher: build then boot mini-arm-os in QEMU.
set -e
cd "$(dirname "$0")"

if ! command -v qemu-system-aarch64 >/dev/null; then
    echo "ERROR: qemu-system-aarch64 not found."
    echo "  macOS:  brew install qemu"
    echo "  Linux:  sudo apt install qemu-system-arm"
    exit 1
fi

make -s
echo ""
echo "─────────────────────────────────────────────────────────────"
echo "  Booting mini-arm-os.  To exit QEMU:  press Ctrl-A then x"
echo "─────────────────────────────────────────────────────────────"
echo ""
exec qemu-system-aarch64 -M virt -cpu cortex-a72 -m 256M \
     -nographic -kernel kernel.elf
