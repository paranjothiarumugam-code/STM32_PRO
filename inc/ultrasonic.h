#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "stm32f103_regs.h"
#include <stdint.h>

typedef struct {
    GPIO_TypeDef *port;
    uint8_t trig_pin;
    uint8_t echo_pin;
} ultrasonic_t;

void ultrasonic_init(ultrasonic_t *u, GPIO_TypeDef *port, uint8_t trig_pin,
                     uint8_t echo_pin);

/* Triggers one measurement and returns distance in cm, or -1 on timeout
 * (no echo / out of range). Blocking; takes a few ms worst case. */
int32_t ultrasonic_read_cm(const ultrasonic_t *u);

#endif /* ULTRASONIC_H */
