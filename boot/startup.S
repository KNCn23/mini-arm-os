// AArch64 boot stub for QEMU virt machine
// Sets up a stack and branches to kmain in C.

.section ".text.boot"
.global _start

_start:
    // Park all secondary cores: only CPU 0 continues.
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbnz    x0, halt

    // Set up the stack just below _start (grows downward).
    ldr     x0, =_stack_top
    mov     sp, x0

    // Zero the .bss section.
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
1:  cmp     x0, x1
    b.ge    2f
    str     xzr, [x0], #8
    b       1b
2:
    // Jump to C entry point.
    bl      kmain

    // Should never return; if it does, hang.
halt:
    wfe
    b       halt
