#include "display.h"
#include "config.h"

#if ENABLE_DISPLAY

#include "i2c.h"
#include "lcd.h"
#include <string.h>

#define LINE_LEN 20

static void put_str(char *buf, int pos, int width, const char *s) {
    int i = 0;
    for (; i < width && s[i]; i++) {
        buf[pos + i] = s[i];
    }
    for (; i < width; i++) {
        buf[pos + i] = ' ';
    }
}

/* Writes exactly 3 zero-padded decimal digits (0-999, clamped). */
static void put_udec3(char *buf, int pos, int value) {
    if (value < 0) {
        value = 0;
    }
    if (value > 999) {
        value = 999;
    }
    buf[pos + 0] = (char)('0' + (value / 100) % 10);
    buf[pos + 1] = (char)('0' + (value / 10) % 10);
    buf[pos + 2] = (char)('0' + value % 10);
}

static void build_tank_line(char *buf, int tank_index, const tank_t *t) {
    memset(buf, ' ', LINE_LEN);

    char label[4] = { 'T', (char)('1' + tank_index), ' ', '\0' };
    put_str(buf, 0, 3, label);

    int pct = tank_percent(t);
    if (pct < 0) {
        buf[3] = buf[4] = buf[5] = buf[6] = '-';
    } else {
        put_udec3(buf, 3, pct);
        buf[6] = '%';
    }
    buf[7] = ' ';

    const char *status;
    if (t->overflow) {
        status = "OVERFLOW";
    } else if (pct < 0) {
        status = "SENSOR ERR";
    } else if (pct >= 100) {
        status = "FULL";
    } else {
        status = "OK";
    }
    put_str(buf, 8, LINE_LEN - 8, status);
}

static void build_pump_line(char *buf, const pump_ctrl_t *pump) {
    memset(buf, ' ', LINE_LEN);
    put_str(buf, 0, 6, "PUMP: ");

    char status[15];
    if (pump->state == PUMP_FILLING && pump->active_tank >= 0) {
        status[0] = 'F'; status[1] = 'I'; status[2] = 'L'; status[3] = 'L';
        status[4] = ' '; status[5] = 'T';
        status[6] = (char)('1' + pump->active_tank);
        status[7] = '\0';
    } else if (pump->state == PUMP_SWITCHING) {
        strcpy(status, "SWITCHING");
    } else {
        strcpy(status, "IDLE");
    }
    put_str(buf, 6, LINE_LEN - 6, status);
}

void display_init(void) {
    i2c_init();
    lcd_init();
    lcd_clear();
}

void display_update(const tank_t *tanks, int num_tanks, const pump_ctrl_t *pump) {
    char line[LINE_LEN];

    /* Rows 0-2 = tanks, row 3 = pump status: sized for this project's
     * fixed 3-tank/1-pump layout on a 20x4 display. */
    for (int i = 0; i < num_tanks && i < 3; i++) {
        build_tank_line(line, i, &tanks[i]);
        lcd_set_cursor((uint8_t)i, 0);
        lcd_write_n(line, LINE_LEN);
    }

    build_pump_line(line, pump);
    lcd_set_cursor(3, 0);
    lcd_write_n(line, LINE_LEN);
}

#else /* !ENABLE_DISPLAY */

void display_init(void) {}
void display_update(const tank_t *tanks, int num_tanks, const pump_ctrl_t *pump) {
    (void)tanks;
    (void)num_tanks;
    (void)pump;
}

#endif
