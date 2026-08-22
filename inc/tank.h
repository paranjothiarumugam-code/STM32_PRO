#ifndef TANK_H
#define TANK_H

#include "stm32f103_regs.h"
#include "ultrasonic.h"
#include "gpio.h"
#include <stdint.h>

typedef struct {
    ultrasonic_t sensor;
    GPIO_TypeDef *float_port;
    uint8_t float_pin;
    GPIO_TypeDef *valve_port;
    uint8_t valve_pin;

    int32_t last_distance_cm; /* -1 = sensor fault/no echo */
    uint8_t overflow;         /* 1 = float switch triggered (hard cutoff) */
} tank_t;

void tank_init(tank_t *t, GPIO_TypeDef *us_port, uint8_t trig_pin,
               uint8_t echo_pin, GPIO_TypeDef *float_port, uint8_t float_pin,
               GPIO_TypeDef *valve_port, uint8_t valve_pin);

/* Refreshes last_distance_cm and overflow from the live sensors. Float
 * switch is polled every call (cheap); ultrasonic is only re-sampled
 * when do_ultrasonic is non-zero (it's a slow, blocking measurement). */
void tank_poll(tank_t *t, int do_ultrasonic);

/* True once the ultrasonic level backs off past TANK_REFILL_CM and the
 * float switch is not currently triggered. */
int tank_needs_fill(const tank_t *t);

/* True once "full": float switch triggered OR ultrasonic reads within
 * TANK_FULL_CM. Float switch always wins even if the ultrasonic sensor
 * is faulty (no echo, stuck reading), so overflow protection never
 * depends on the ultrasonic sensor alone. */
int tank_is_full(const tank_t *t);

/* Fill percentage for the status display only (0-100), derived from
 * last_distance_cm and TANK_FULL_CM/TANK_EMPTY_CM. Returns -1 if the
 * ultrasonic sensor has no reading (fault/no echo) — this never feeds
 * back into fill/overflow control, which relies on tank_needs_fill()/
 * tank_is_full() instead. */
int tank_percent(const tank_t *t);

static inline void tank_valve(const tank_t *t, int open) {
    gpio_write(t->valve_port, t->valve_pin, open ? 1 : 0);
}

#endif /* TANK_H */
