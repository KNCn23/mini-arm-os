#include "uart.h"

/* PL011 register offsets */
#define UART_DR     0x000   /* Data register     */
#define UART_FR     0x018   /* Flag register     */
#define UART_IBRD   0x024
#define UART_FBRD   0x028
#define UART_LCRH   0x02C
#define UART_CR     0x030

#define FR_TXFF     (1 << 5)   /* TX FIFO full   */
#define FR_RXFE     (1 << 4)   /* RX FIFO empty  */

static volatile u32 *reg(unsigned long off) {
    return (volatile u32 *)(UART0_BASE + off);
}

void uart_init(void) {
    /* Disable UART before configuring. */
    *reg(UART_CR)   = 0;
    /* 115200 baud @ 24 MHz reference (QEMU virt): integer=13, frac=0 */
    *reg(UART_IBRD) = 13;
    *reg(UART_FBRD) = 0;
    /* 8N1, FIFO enabled */
    *reg(UART_LCRH) = (3 << 5) | (1 << 4);
    /* Enable UART, TX and RX */
    *reg(UART_CR)   = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_putc(char c) {
    while (*reg(UART_FR) & FR_TXFF) { }
    *reg(UART_DR) = (u32)c;
    if (c == '\n') uart_putc('\r');
}

void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

int uart_getc_nonblock(void) {
    if (*reg(UART_FR) & FR_RXFE) return -1;
    return (int)(*reg(UART_DR) & 0xFF);
}

char uart_getc_block(void) {
    while (*reg(UART_FR) & FR_RXFE) { }
    return (char)(*reg(UART_DR) & 0xFF);
}

void uart_print_hex(u64 v) {
    static const char hex[] = "0123456789abcdef";
    uart_puts("0x");
    int started = 0;
    for (int i = 60; i >= 0; i -= 4) {
        char c = hex[(v >> i) & 0xF];
        if (c != '0') started = 1;
        if (started || i == 0) uart_putc(c);
    }
}

void uart_print_dec(u64 v) {
    char buf[24];
    int  i = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v) { buf[i++] = '0' + (v % 10); v /= 10; }
    while (i--) uart_putc(buf[i]);
}
