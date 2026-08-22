#include "i2c.h"
#include "stm32f103_regs.h"
#include "gpio.h"
#include "systick.h"

#define I2C_TIMEOUT_MS 5

static int timed_out(uint32_t deadline) {
    return (int32_t)(millis() - deadline) >= 0;
}

void i2c_init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    /* PB6=SCL, PB7=SDA: alternate-function open-drain, 50 MHz. Needs
     * external (or module-provided) pull-ups to work. */
    gpio_set_mode(GPIOB, 6, GPIO_MODE_OUT_50MHZ | GPIO_CNF_OUT_AF_OPENDRAIN);
    gpio_set_mode(GPIOB, 7, GPIO_MODE_OUT_50MHZ | GPIO_CNF_OUT_AF_OPENDRAIN);

    I2C1->CR1 = 0; /* disable while configuring */
    I2C1->CR2 = (APB1_CLOCK_HZ / 1000000UL) & 0x3FUL;
    I2C1->CCR = (APB1_CLOCK_HZ / (2UL * 100000UL)) & 0xFFFUL; /* 100 kHz standard mode */
    I2C1->TRISE = (APB1_CLOCK_HZ / 1000000UL) + 1UL;
    I2C1->CR1 = I2C_CR1_PE;
}

int i2c_write(uint8_t addr7, const uint8_t *data, uint8_t len) {
    uint32_t deadline = millis() + I2C_TIMEOUT_MS;
    while (I2C1->SR2 & I2C_SR2_BUSY) {
        if (timed_out(deadline)) return 0;
    }

    I2C1->CR1 |= I2C_CR1_START;
    deadline = millis() + I2C_TIMEOUT_MS;
    while (!(I2C1->SR1 & I2C_SR1_SB)) {
        if (timed_out(deadline)) return 0;
    }

    I2C1->DR = (uint32_t)(addr7 << 1); /* write */
    deadline = millis() + I2C_TIMEOUT_MS;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
        if (timed_out(deadline)) {
            I2C1->CR1 |= I2C_CR1_STOP;
            return 0;
        }
    }
    (void)I2C1->SR1;
    (void)I2C1->SR2; /* clears ADDR */

    for (uint8_t i = 0; i < len; i++) {
        deadline = millis() + I2C_TIMEOUT_MS;
        while (!(I2C1->SR1 & I2C_SR1_TXE)) {
            if (timed_out(deadline)) {
                I2C1->CR1 |= I2C_CR1_STOP;
                return 0;
            }
        }
        I2C1->DR = data[i];
    }

    deadline = millis() + I2C_TIMEOUT_MS;
    while (!(I2C1->SR1 & I2C_SR1_BTF)) {
        if (timed_out(deadline)) {
            I2C1->CR1 |= I2C_CR1_STOP;
            return 0;
        }
    }

    I2C1->CR1 |= I2C_CR1_STOP;
    return 1;
}
