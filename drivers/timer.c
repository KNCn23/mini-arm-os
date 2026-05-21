#include "timer.h"

static u64 boot_ticks;

static inline u64 read_cntfrq(void) {
    u64 v; asm volatile("mrs %0, cntfrq_el0" : "=r"(v)); return v;
}
static inline u64 read_cntpct(void) {
    u64 v; asm volatile("mrs %0, cntpct_el0" : "=r"(v)); return v;
}

void timer_init(void) {
    boot_ticks = read_cntpct();
}

u64 timer_freq(void)  { return read_cntfrq(); }
u64 timer_ticks(void) { return read_cntpct() - boot_ticks; }
u64 timer_ms(void)    { return (timer_ticks() * 1000UL) / read_cntfrq(); }

void timer_delay_ms(u64 ms) {
    u64 end = timer_ms() + ms;
    while (timer_ms() < end) { asm volatile("yield"); }
}
