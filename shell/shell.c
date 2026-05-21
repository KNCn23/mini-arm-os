#include "shell.h"
#include "uart.h"
#include "timer.h"
#include "mem.h"
#include "types.h"

#define LINE_MAX 128

/* ── tiny string helpers ──────────────────────────────────────────────── */
static int streq(const char *a, const char *b) {
    while (*a && *b) { if (*a++ != *b++) return 0; }
    return *a == *b;
}

static size_t slen(const char *s) {
    size_t n = 0; while (s[n]) n++; return n;
}

/* ── line editor: handles backspace and ASCII echo ────────────────────── */
static void read_line(char *buf, int maxlen) {
    int i = 0;
    while (1) {
        char c = uart_getc_block();
        if (c == '\r' || c == '\n') {
            buf[i] = 0;
            uart_putc('\n');
            return;
        }
        if ((c == 0x7F || c == 0x08) && i > 0) {
            i--;
            uart_puts("\b \b");
            continue;
        }
        if (c >= 32 && c < 127 && i < maxlen - 1) {
            buf[i++] = c;
            uart_putc(c);
        }
    }
}

/* ── built-in commands ────────────────────────────────────────────────── */
static void cmd_help(void) {
    uart_puts("Available commands:\n");
    uart_puts("  help      Show this message\n");
    uart_puts("  echo ...  Print arguments\n");
    uart_puts("  clear     Clear the screen (ANSI)\n");
    uart_puts("  uptime    Show milliseconds since boot\n");
    uart_puts("  mem       Show heap usage\n");
    uart_puts("  ticks     Show timer ticks and frequency\n");
    uart_puts("  banner    Reprint the boot banner\n");
    uart_puts("  reboot    Halt the CPU (QEMU will exit on shutdown)\n");
}

static void cmd_clear(void) {
    /* ANSI clear screen + cursor home */
    uart_puts("\x1b[2J\x1b[H");
}

static void cmd_uptime(void) {
    uart_puts("Uptime: ");
    uart_print_dec(timer_ms());
    uart_puts(" ms\n");
}

static void cmd_mem(void) {
    uart_puts("Heap: ");
    uart_print_dec(kheap_used());
    uart_puts(" / ");
    uart_print_dec(kheap_total());
    uart_puts(" bytes used\n");
}

static void cmd_ticks(void) {
    uart_puts("Ticks: "); uart_print_dec(timer_ticks());
    uart_puts("  Freq: "); uart_print_dec(timer_freq());
    uart_puts(" Hz\n");
}

static void cmd_echo(const char *args) {
    uart_puts(args);
    uart_putc('\n');
}

static void banner(void);

static void cmd_reboot(void) {
    uart_puts("Halting CPU.\n");
    for (;;) asm volatile("wfe");
}

/* ── dispatcher ──────────────────────────────────────────────────────── */
static void dispatch(char *line) {
    /* skip leading spaces */
    while (*line == ' ') line++;
    if (*line == 0) return;

    /* split into cmd / rest */
    char *rest = line;
    while (*rest && *rest != ' ') rest++;
    if (*rest == ' ') { *rest = 0; rest++; while (*rest == ' ') rest++; }

    if      (streq(line, "help"))   cmd_help();
    else if (streq(line, "echo"))   cmd_echo(rest);
    else if (streq(line, "clear"))  cmd_clear();
    else if (streq(line, "uptime")) cmd_uptime();
    else if (streq(line, "mem"))    cmd_mem();
    else if (streq(line, "ticks"))  cmd_ticks();
    else if (streq(line, "banner")) banner();
    else if (streq(line, "reboot")) cmd_reboot();
    else {
        uart_puts("unknown command: ");
        uart_puts(line);
        uart_puts(" (try 'help')\n");
    }
}

static void banner(void) {
    uart_puts("\n");
    uart_puts("  __  __ _       _    ___  ____\n");
    uart_puts(" |  \\/  (_)_ __ (_)  / _ \\/ ___|\n");
    uart_puts(" | |\\/| | | '_ \\| | | | | \\___ \\\n");
    uart_puts(" | |  | | | | | | | | |_| |___) |\n");
    uart_puts(" |_|  |_|_|_| |_|_|  \\___/|____/\n");
    uart_puts("       AArch64 bare-metal kernel\n\n");
}

void shell_run(void) {
    char line[LINE_MAX];
    banner();
    uart_puts("Type 'help' to see commands.\n\n");
    while (1) {
        uart_puts("mini> ");
        read_line(line, LINE_MAX);
        dispatch(line);
    }
}
