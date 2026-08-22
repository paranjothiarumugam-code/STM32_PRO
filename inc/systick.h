#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* Sets system clock to 72 MHz (HSE 8 MHz x PLL9) and starts SysTick
 * as a 1 ms tick + TIM2 as a free-running 1 us tick. Call once at boot. */
void clock_init(void);

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

/* Milliseconds since boot (wraps ~49 days in, irrelevant here). */
uint32_t millis(void);

#endif /* SYSTICK_H */
