/*
 * 3-tank overflow controller sharing a single 2 HP pump motor.
 *
 * Each tank has an ultrasonic sensor (level %) and a float switch
 * (hard overflow cutoff, wins over the ultrasonic reading). Only one
 * tank's solenoid valve is ever open at a time; the pump runs only
 * while a valve is open. Tanks are filled in priority order
 * (tank[0], then tank[1], then tank[2]) — see pump_ctrl_step().
 *
 * A 20x4 I2C LCD (see display.c) shows all 3 tank levels and pump
 * status; it's best-effort and can never block the control loop above.
 */
#include "config.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "tank.h"
#include "pump.h"
#include "display.h"

#define ULTRASONIC_POLL_MS 400
#define LOG_INTERVAL_MS    2000

static tank_t tanks[NUM_TANKS];
static pump_ctrl_t pump;

static void led_set(int on) {
    /* Blue Pill onboard LED is active LOW. */
    gpio_write(LED_PORT, LED_PIN, on ? 0 : 1);
}

static void log_status(void) {
    const char *state_str =
        pump.state == PUMP_IDLE ? "IDLE" :
        pump.state == PUMP_FILLING ? "FILLING" : "SWITCH";

    uart_puts("pump=");
    uart_puts(state_str);
    uart_puts(" active=");
    uart_put_i32(pump.active_tank);
    for (int i = 0; i < NUM_TANKS; i++) {
        uart_puts(" T");
        uart_put_i32(i);
        uart_puts("[d=");
        uart_put_i32(tanks[i].last_distance_cm);
        uart_puts("cm ovf=");
        uart_put_i32(tanks[i].overflow);
        uart_puts("]");
    }
    uart_puts("\r\n");
}

int main(void) {
    gpio_clocks_init();
    clock_init();
    uart_init();

    gpio_set_mode(LED_PORT, LED_PIN,
                  GPIO_MODE_OUT_2MHZ | GPIO_CNF_OUT_PUSHPULL);
    led_set(0);

    tank_init(&tanks[0], US_PORT, US_TRIG_PIN_T1, US_ECHO_PIN_T1, FLOAT_PORT,
              FLOAT_PIN_T1, ACT_PORT, VALVE_PIN_T1);
    tank_init(&tanks[1], US_PORT, US_TRIG_PIN_T2, US_ECHO_PIN_T2, FLOAT_PORT,
              FLOAT_PIN_T2, ACT_PORT, VALVE_PIN_T2);
    tank_init(&tanks[2], US_PORT, US_TRIG_PIN_T3, US_ECHO_PIN_T3, FLOAT_PORT,
              FLOAT_PIN_T3, ACT_PORT, VALVE_PIN_T3);

    pump_ctrl_init(&pump, tanks, NUM_TANKS, ACT_PORT, PUMP_PIN);
    display_init();

    uart_puts("\r\n3-tank overflow controller booted\r\n");

    uint32_t next_ultrasonic_ms = 0;
    uint32_t next_log_ms = 0;
    uint32_t next_display_ms = 0;
    int round_robin = 0;

    for (;;) {
        uint32_t now = millis();

        /* Float switches are cheap digital reads: poll every tank on
         * every loop iteration so overflow cutoff is never delayed by
         * the ultrasonic scheduling below. */
        for (int i = 0; i < NUM_TANKS; i++) {
            tank_poll(&tanks[i], 0);
        }

        if ((int32_t)(now - next_ultrasonic_ms) >= 0) {
            int idx = (pump.state == PUMP_FILLING) ? pump.active_tank
                                                    : round_robin;
            tank_poll(&tanks[idx], 1);
            if (pump.state != PUMP_FILLING) {
                round_robin = (round_robin + 1) % NUM_TANKS;
            }
            next_ultrasonic_ms = now + ULTRASONIC_POLL_MS;
        }

        pump_ctrl_step(&pump, now);

        led_set(pump.state == PUMP_FILLING);

        if ((int32_t)(now - next_log_ms) >= 0) {
            log_status();
            next_log_ms = now + LOG_INTERVAL_MS;
        }

        if ((int32_t)(now - next_display_ms) >= 0) {
            display_update(tanks, NUM_TANKS, &pump);
            next_display_ms = now + DISPLAY_REFRESH_MS;
        }
    }
}
