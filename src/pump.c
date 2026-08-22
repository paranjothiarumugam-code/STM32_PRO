#include "pump.h"
#include "config.h"

static void pump_write(const pump_ctrl_t *p, int on) {
    gpio_write(p->pump_port, p->pump_pin, on ? 1 : 0);
}

void pump_ctrl_init(pump_ctrl_t *p, tank_t *tanks, int num_tanks,
                     GPIO_TypeDef *pump_port, uint8_t pump_pin) {
    p->tanks = tanks;
    p->num_tanks = num_tanks;
    p->pump_port = pump_port;
    p->pump_pin = pump_pin;

    gpio_set_mode(pump_port, pump_pin,
                  GPIO_MODE_OUT_2MHZ | GPIO_CNF_OUT_PUSHPULL);
    pump_write(p, 0);

    p->state = PUMP_IDLE;
    p->active_tank = -1;
    p->state_since_ms = 0;
    p->pump_off_since_ms = 0;
}

static int pick_next_tank(const pump_ctrl_t *p) {
    for (int i = 0; i < p->num_tanks; i++) {
        if (tank_needs_fill(&p->tanks[i])) {
            return i;
        }
    }
    return -1;
}

void pump_ctrl_step(pump_ctrl_t *p, uint32_t now_ms) {
    switch (p->state) {
    case PUMP_IDLE: {
        int next = pick_next_tank(p);
        if (next >= 0 &&
            (now_ms - p->pump_off_since_ms) >= PUMP_MIN_OFF_MS) {
            p->active_tank = next;
            tank_valve(&p->tanks[next], 1);
            pump_write(p, 1);
            p->state = PUMP_FILLING;
            p->state_since_ms = now_ms;
        }
        break;
    }

    case PUMP_FILLING: {
        tank_t *t = &p->tanks[p->active_tank];
        if (tank_is_full(t)) {
            tank_valve(t, 0);
            pump_write(p, 0);
            p->pump_off_since_ms = now_ms;
            p->state_since_ms = now_ms;
            p->state = PUMP_SWITCHING;
        }
        break;
    }

    case PUMP_SWITCHING: {
        if ((now_ms - p->state_since_ms) >= VALVE_SWITCH_DELAY_MS) {
            p->active_tank = -1;
            p->state = PUMP_IDLE;
        }
        break;
    }
    }
}
