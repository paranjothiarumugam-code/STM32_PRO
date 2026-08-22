#include "lcd.h"
#include "i2c.h"
#include "systick.h"
#include "config.h"

#define PCF_RS        0x01U
#define PCF_ENABLE    0x04U
#define PCF_BACKLIGHT 0x08U

static void expander_write(uint8_t data) {
    uint8_t d = data | PCF_BACKLIGHT;
    i2c_write(LCD_I2C_ADDR, &d, 1);
}

static void pulse_enable(uint8_t data) {
    expander_write(data | PCF_ENABLE);
    delay_us(1);
    expander_write(data & (uint8_t)~PCF_ENABLE);
    delay_us(50);
}

static void write4bits(uint8_t nibble_and_flags) {
    expander_write(nibble_and_flags);
    pulse_enable(nibble_and_flags);
}

static void lcd_send(uint8_t value, uint8_t mode) {
    write4bits((uint8_t)((value & 0xF0U) | mode));
    write4bits((uint8_t)(((value << 4) & 0xF0U) | mode));
}

static void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0);
}

static void lcd_data(uint8_t d) {
    lcd_send(d, PCF_RS);
}

void lcd_init(void) {
    delay_ms(50); /* HD44780 power-on settle time */

    /* Standard HD44780 "wake up" sequence to force 4-bit mode
     * regardless of whatever state the controller powered up in. */
    write4bits(0x30);
    delay_ms(5);
    write4bits(0x30);
    delay_us(150);
    write4bits(0x30);
    delay_us(150);
    write4bits(0x20); /* now in 4-bit mode */
    delay_us(150);

    lcd_command(0x28); /* function set: 4-bit, 2-line, 5x8 dots */
    lcd_command(0x08); /* display off */
    lcd_command(0x01); /* clear display */
    delay_ms(2);
    lcd_command(0x06); /* entry mode: auto-increment, no shift */
    lcd_command(0x0C); /* display on, cursor off, blink off */
}

void lcd_clear(void) {
    lcd_command(0x01);
    delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    static const uint8_t row_offset[4] = { 0x00, 0x40, 0x14, 0x54 };
    uint8_t r = (row < 4) ? row : 3;
    lcd_command((uint8_t)(0x80U | (row_offset[r] + col)));
}

void lcd_write_n(const char *s, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        lcd_data((uint8_t)s[i]);
    }
}
