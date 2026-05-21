#ifndef UART_H
#define UART_H

#include "types.h"

/* PL011 UART base on QEMU virt machine */
#define UART0_BASE 0x09000000UL

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
int  uart_getc_nonblock(void);   /* -1 if no byte ready */
char uart_getc_block(void);

void uart_print_hex(u64 v);
void uart_print_dec(u64 v);

#endif
