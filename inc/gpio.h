#ifndef GPIO_H
#define GPIO_H

#include "stm32f103_regs.h"

/* Configure one pin's CRL/CRH nibble (mode|cnf combined, e.g.
 * GPIO_MODE_OUT_2MHZ | GPIO_CNF_OUT_PUSHPULL). */
void gpio_set_mode(GPIO_TypeDef *port, uint8_t pin, uint32_t mode_cnf);

static inline void gpio_write(GPIO_TypeDef *port, uint8_t pin, int level) {
    port->BSRR = level ? (1UL << pin) : (1UL << (pin + 16));
}

static inline int gpio_read(GPIO_TypeDef *port, uint8_t pin) {
    return (port->IDR >> pin) & 1U;
}

/* Enables clocks for GPIOA/B/C + AFIO. Call once at startup. */
void gpio_clocks_init(void);

#endif /* GPIO_H */
