#include "uart.h"
#include "stm32f103_regs.h"
#include "gpio.h"
#include "config.h"

#if ENABLE_UART_LOG

void uart_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

    /* PA9 = TX: AF push-pull, 50 MHz. PA10 = RX: floating input. */
    gpio_set_mode(GPIOA, 9, GPIO_MODE_OUT_50MHZ | GPIO_CNF_OUT_AF_PUSHPULL);
    gpio_set_mode(GPIOA, 10, GPIO_MODE_INPUT | GPIO_CNF_IN_FLOATING);

    /* BRR = f_PCLK2 / baud, 16x oversampling (STM32F1 default). */
    USART1->BRR = SYSTEM_CLOCK_HZ / UART_BAUD;
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

static void uart_putc(char c) {
    while (!(USART1->SR & USART_SR_TXE)) {}
    USART1->DR = (uint8_t)c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

void uart_put_u32(uint32_t v) {
    char buf[11];
    int i = 10;
    buf[10] = '\0';
    if (v == 0) {
        uart_putc('0');
        return;
    }
    while (v > 0 && i > 0) {
        buf[--i] = (char)('0' + (v % 10));
        v /= 10;
    }
    uart_puts(&buf[i]);
}

void uart_put_i32(int32_t v) {
    if (v < 0) {
        uart_putc('-');
        v = -v;
    }
    uart_put_u32((uint32_t)v);
}

#else /* !ENABLE_UART_LOG */

void uart_init(void) {}
void uart_puts(const char *s) { (void)s; }
void uart_put_u32(uint32_t v) { (void)v; }
void uart_put_i32(int32_t v) { (void)v; }

#endif
