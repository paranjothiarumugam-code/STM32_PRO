#ifndef UART_H
#define UART_H

#include <stdint.h>

/* USART1 on PA9 (TX) / PA10 (RX), 115200 8N1. Used only for diagnostic
 * logging; the control logic does not depend on it being connected. */
void uart_init(void);
void uart_puts(const char *s);
void uart_put_u32(uint32_t v);
void uart_put_i32(int32_t v);

#endif /* UART_H */
