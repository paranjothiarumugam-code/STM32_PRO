#ifndef DISPLAY_H
#define DISPLAY_H

#include "tank.h"
#include "pump.h"

/* Status display: all 3 tank levels + pump state on one 20x4 screen.
 * Best-effort — see lcd.h/i2c.h; a missing display never affects
 * fill/overflow control. */
void display_init(void);
void display_update(const tank_t *tanks, int num_tanks, const pump_ctrl_t *pump);

#endif /* DISPLAY_H */
