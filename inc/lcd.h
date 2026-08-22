#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/* HD44780-compatible 20x4 character LCD driven through a PCF8574 I2C
 * I/O expander backpack, in 4-bit mode. All calls are best-effort: a
 * disconnected/faulty display degrades to a no-op rather than
 * blocking (see i2c.h) — it must never stall the control loop. */
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);

/* Writes exactly len characters (space-padded content expected from
 * the caller), so every refresh fully overwrites the previous one and
 * never leaves stale characters from a longer previous string. */
void lcd_write_n(const char *s, uint8_t len);

#endif /* LCD_H */
