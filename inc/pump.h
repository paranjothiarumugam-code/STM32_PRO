#ifndef PUMP_H
#define PUMP_H

#include "tank.h"
#include <stdint.h>

typedef enum {
    PUMP_IDLE = 0,
    PUMP_FILLING,
    PUMP_SWITCHING, /* valve just closed, settling before next action */
} pump_state_t;

typedef struct {
    tank_t *tanks;
    int num_tanks;
    GPIO_TypeDef *pump_port;
    uint8_t pump_pin;

    pump_state_t state;
    int active_tank;       /* index of tank currently being filled, -1 = none */
    uint32_t state_since_ms;
    uint32_t pump_off_since_ms;
} pump_ctrl_t;

void pump_ctrl_init(pump_ctrl_t *p, tank_t *tanks, int num_tanks,
                     GPIO_TypeDef *pump_port, uint8_t pump_pin);

/* Call every main-loop iteration once tank_poll() has refreshed sensor
 * data for the relevant tank(s). Drives the pump/valve state machine
 * (priority order: tanks[0] first, then tanks[1], ...). */
void pump_ctrl_step(pump_ctrl_t *p, uint32_t now_ms);

#endif /* PUMP_H */
