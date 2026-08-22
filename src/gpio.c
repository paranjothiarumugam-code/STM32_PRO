#include "gpio.h"

void gpio_clocks_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN |
                    RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
}

void gpio_set_mode(GPIO_TypeDef *port, uint8_t pin, uint32_t mode_cnf) {
    __IO uint32_t *reg = (pin < 8) ? &port->CRL : &port->CRH;
    uint8_t shift = (uint8_t)((pin % 8) * 4);
    uint32_t val = *reg;
    val &= ~(0xFUL << shift);
    val |= (mode_cnf & 0xFUL) << shift;
    *reg = val;
}
