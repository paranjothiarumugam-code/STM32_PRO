#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/* Blocking I2C1 master driver (100 kHz), PB6=SCL/PB7=SDA on the Blue
 * Pill. Every wait loop below has a hard timeout: a missing or stuck
 * display can never hang the pump/valve control loop that also runs
 * in main() — a failed transfer just returns 0 and the caller skips
 * that refresh. */
void i2c_init(void);

/* Writes len bytes to a 7-bit-addressed slave. Returns 1 on success,
 * 0 on timeout/NACK (bus released via STOP either way). */
int i2c_write(uint8_t addr7, const uint8_t *data, uint8_t len);

#endif /* I2C_H */
