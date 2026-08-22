#include "tank.h"
#include "config.h"

void tank_init(tank_t *t, GPIO_TypeDef *us_port, uint8_t trig_pin,
               uint8_t echo_pin, GPIO_TypeDef *float_port, uint8_t float_pin,
               GPIO_TypeDef *valve_port, uint8_t valve_pin) {
    ultrasonic_init(&t->sensor, us_port, trig_pin, echo_pin);

    t->float_port = float_port;
    t->float_pin = float_pin;
    gpio_set_mode(float_port, float_pin, GPIO_MODE_INPUT | GPIO_CNF_IN_PULL);
    gpio_write(float_port, float_pin, 1); /* ODR=1 with CNF=10 => pull-up */

    t->valve_port = valve_port;
    t->valve_pin = valve_pin;
    gpio_set_mode(valve_port, valve_pin,
                  GPIO_MODE_OUT_2MHZ | GPIO_CNF_OUT_PUSHPULL);
    tank_valve(t, 0);

    t->last_distance_cm = -1;
    t->overflow = 0;
}

void tank_poll(tank_t *t, int do_ultrasonic) {
    /* Float switch closes to GND at high level -> IDR reads 0. */
    t->overflow = (gpio_read(t->float_port, t->float_pin) == 0) ? 1 : 0;

    if (do_ultrasonic) {
        int32_t d = ultrasonic_read_cm(&t->sensor);
        if (d >= 0 && d <= TANK_MAX_CM) {
            t->last_distance_cm = d;
        } else {
            t->last_distance_cm = -1; /* sensor fault / out of range */
        }
    }
}

int tank_needs_fill(const tank_t *t) {
    if (t->overflow) {
        return 0; /* float switch says it's already full */
    }
    if (t->last_distance_cm < 0) {
        /* Ultrasonic sensor unavailable: fall back to the float switch
         * only, refuse to start a fill blind. Operator should fix the
         * sensor; this just prevents dry-running the pump on bad data. */
        return 0;
    }
    return t->last_distance_cm >= TANK_REFILL_CM;
}

int tank_is_full(const tank_t *t) {
    if (t->overflow) {
        return 1; /* hardware safety cutoff always wins */
    }
    if (t->last_distance_cm < 0) {
        return 0;
    }
    return t->last_distance_cm <= TANK_FULL_CM;
}
