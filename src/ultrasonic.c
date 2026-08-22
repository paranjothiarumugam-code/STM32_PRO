#include "ultrasonic.h"
#include "gpio.h"
#include "systick.h"

/* HC-SR04-style timing: trigger 10 us high pulse, wait for echo pulse
 * whose width (us) is round-trip time; distance = width / 58 (cm).
 * Both timeouts below are generous for a ~4-6 m sensor. */
#define ECHO_START_TIMEOUT_US 30000U
#define ECHO_PULSE_TIMEOUT_US 30000U

void ultrasonic_init(ultrasonic_t *u, GPIO_TypeDef *port, uint8_t trig_pin,
                     uint8_t echo_pin) {
    u->port = port;
    u->trig_pin = trig_pin;
    u->echo_pin = echo_pin;

    gpio_set_mode(port, trig_pin, GPIO_MODE_OUT_2MHZ | GPIO_CNF_OUT_PUSHPULL);
    gpio_write(port, trig_pin, 0);
    gpio_set_mode(port, echo_pin, GPIO_MODE_INPUT | GPIO_CNF_IN_FLOATING);
}

int32_t ultrasonic_read_cm(const ultrasonic_t *u) {
    gpio_write(u->port, u->trig_pin, 0);
    delay_us(2);
    gpio_write(u->port, u->trig_pin, 1);
    delay_us(10);
    gpio_write(u->port, u->trig_pin, 0);

    uint32_t waited = 0;
    while (gpio_read(u->port, u->echo_pin) == 0) {
        delay_us(1);
        if (++waited > ECHO_START_TIMEOUT_US) {
            return -1;
        }
    }

    uint16_t start = (uint16_t)TIM2->CNT;
    waited = 0;
    while (gpio_read(u->port, u->echo_pin) == 1) {
        delay_us(1);
        if (++waited > ECHO_PULSE_TIMEOUT_US) {
            return -1;
        }
    }
    uint16_t end = (uint16_t)TIM2->CNT;

    uint16_t pulse_us = (uint16_t)(end - start);
    return (int32_t)(pulse_us / 58U);
}
