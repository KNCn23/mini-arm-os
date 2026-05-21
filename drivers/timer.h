#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void  timer_init(void);
u64   timer_freq(void);
u64   timer_ticks(void);
u64   timer_ms(void);
void  timer_delay_ms(u64 ms);

#endif
