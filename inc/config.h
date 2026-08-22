/*
 * Pin map and tuning constants for the 3-tank / single-pump overflow
 * controller. Edit this file to match your wiring instead of digging
 * through the driver source.
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f103_regs.h"

#define NUM_TANKS 3

/* ---- Float switches: one per tank, safety overflow cutoff ----------
 * Wired normally-open to GND, pin uses internal pull-up.
 * IDR bit reads 0 ("triggered") when the switch closes at high level. */
#define FLOAT_PORT   GPIOA
#define FLOAT_PIN_T1 0
#define FLOAT_PIN_T2 1
#define FLOAT_PIN_T3 2

/* ---- Ultrasonic sensors (HC-SR04 / JSN-SR04T style): one per tank --- */
#define US_PORT        GPIOA
#define US_TRIG_PIN_T1 3
#define US_ECHO_PIN_T1 4
#define US_TRIG_PIN_T2 5
#define US_ECHO_PIN_T2 6
#define US_TRIG_PIN_T3 7
#define US_ECHO_PIN_T3 8

/* ---- Actuator outputs: relay module inputs (active HIGH) -----------
 * PUMP_PIN drives the relay/contactor coil circuit for the single
 * 2 HP pump motor. VALVE_PIN_Tn drives that tank's solenoid valve.
 * Only one valve is ever open at a time (sequential fill). */
#define ACT_PORT      GPIOB
#define PUMP_PIN      12
#define VALVE_PIN_T1  13
#define VALVE_PIN_T2  14
#define VALVE_PIN_T3  15

/* ---- Onboard status LED (Blue Pill: PC13, active LOW) ---------------- */
#define LED_PORT GPIOC
#define LED_PIN  13

/* ---- Tank geometry / thresholds --------------------------------------
 * Distances are measured from the ultrasonic sensor (mounted at the
 * tank top) down to the water surface, in centimetres. A SMALLER
 * distance means a HIGHER water level. Tune per physical tank. */
#define TANK_FULL_CM   15  /* stop filling once within this distance   */
#define TANK_REFILL_CM 60  /* start filling once level drops past this */
#define TANK_MAX_CM    400 /* sensor read above this = treat as no-echo/empty */

/* Minimum pump stop time before it is allowed to restart, to protect
 * the 2 HP motor from short-cycling (seconds). */
#define PUMP_MIN_OFF_MS      5000
/* Settle time after closing a valve before switching to the next tank
 * or stopping the pump, to avoid water hammer (milliseconds). */
#define VALVE_SWITCH_DELAY_MS 1500

/* Serial diagnostics (USART1, PA9=TX/PA10=RX, 115200 8N1). Set to 0 to
 * disable and free those two pins. */
#define ENABLE_UART_LOG 1
#define UART_BAUD 115200

#endif /* CONFIG_H */
