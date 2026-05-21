#include "types.h"
#include "uart.h"
#include "timer.h"
#include "mem.h"
#include "shell.h"

void kmain(void) {
    uart_init();
    timer_init();

    uart_puts("[boot] UART ready\n");
    uart_puts("[boot] timer freq = ");
    uart_print_dec(timer_freq());
    uart_puts(" Hz\n");
    uart_puts("[boot] heap = ");
    uart_print_dec(kheap_total());
    uart_puts(" bytes\n");
    uart_puts("[boot] handing control to shell\n");

    shell_run();

    /* Should never reach here */
    for (;;) asm volatile("wfe");
}
