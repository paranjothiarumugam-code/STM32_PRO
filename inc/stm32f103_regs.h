/*
 * Minimal register-level definitions for STM32F103C8 (Blue Pill).
 * No HAL / CMSIS device pack — just the peripherals this project touches.
 */
#ifndef STM32F103_REGS_H
#define STM32F103_REGS_H

#include <stdint.h>

#define __IO volatile

/* ------------------------------------------------------------------ */
/* Core peripherals (Cortex-M3)                                        */
/* ------------------------------------------------------------------ */
typedef struct {
    __IO uint32_t CTRL;
    __IO uint32_t LOAD;
    __IO uint32_t VAL;
    __IO uint32_t CALIB;
} SysTick_TypeDef;
#define SysTick ((SysTick_TypeDef *)0xE000E010UL)

#define SysTick_CTRL_ENABLE    (1UL << 0)
#define SysTick_CTRL_TICKINT   (1UL << 1)
#define SysTick_CTRL_CLKSOURCE (1UL << 2)
#define SysTick_CTRL_COUNTFLAG (1UL << 16)

/* ------------------------------------------------------------------ */
/* RCC                                                                  */
/* ------------------------------------------------------------------ */
typedef struct {
    __IO uint32_t CR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t APB2RSTR;
    __IO uint32_t APB1RSTR;
    __IO uint32_t AHBENR;
    __IO uint32_t APB2ENR;
    __IO uint32_t APB1ENR;
    __IO uint32_t BDCR;
    __IO uint32_t CSR;
} RCC_TypeDef;
#define RCC ((RCC_TypeDef *)0x40021000UL)

#define RCC_CR_HSION   (1UL << 0)
#define RCC_CR_HSIRDY  (1UL << 1)
#define RCC_CR_HSEON   (1UL << 16)
#define RCC_CR_HSERDY  (1UL << 17)
#define RCC_CR_PLLON   (1UL << 24)
#define RCC_CR_PLLRDY  (1UL << 25)

#define RCC_CFGR_SW_PLL      (2UL << 0)
#define RCC_CFGR_SWS_MASK    (3UL << 2)
#define RCC_CFGR_SWS_PLL     (2UL << 2)
#define RCC_CFGR_PPRE1_DIV2  (4UL << 8)
#define RCC_CFGR_PLLSRC_HSE  (1UL << 16)
#define RCC_CFGR_PLLMUL9     (7UL << 18)

#define RCC_APB2ENR_AFIOEN (1UL << 0)
#define RCC_APB2ENR_IOPAEN (1UL << 2)
#define RCC_APB2ENR_IOPBEN (1UL << 3)
#define RCC_APB2ENR_IOPCEN (1UL << 4)
#define RCC_APB2ENR_USART1EN (1UL << 14)

#define RCC_APB1ENR_TIM2EN (1UL << 0)
#define RCC_APB1ENR_I2C1EN (1UL << 21)

/* ------------------------------------------------------------------ */
/* GPIO                                                                 */
/* ------------------------------------------------------------------ */
typedef struct {
    __IO uint32_t CRL;
    __IO uint32_t CRH;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t BRR;
    __IO uint32_t LCKR;
} GPIO_TypeDef;

#define GPIOA ((GPIO_TypeDef *)0x40010800UL)
#define GPIOB ((GPIO_TypeDef *)0x40010C00UL)
#define GPIOC ((GPIO_TypeDef *)0x40011000UL)

/* CNF:MODE nibble values for the CRL/CRH 4-bit-per-pin fields */
#define GPIO_MODE_INPUT           0x0U /* MODE=00 */
#define GPIO_CNF_IN_ANALOG        0x0U
#define GPIO_CNF_IN_FLOATING      0x4U /* CNF=01 << 2 */
#define GPIO_CNF_IN_PULL          0x8U /* CNF=10 << 2 */

#define GPIO_MODE_OUT_2MHZ        0x2U
#define GPIO_MODE_OUT_50MHZ       0x3U
#define GPIO_CNF_OUT_PUSHPULL     0x0U
#define GPIO_CNF_OUT_AF_PUSHPULL  0x8U
#define GPIO_CNF_OUT_AF_OPENDRAIN 0xCU

/* ------------------------------------------------------------------ */
/* TIM2 (used as a free-running 1 us tick source for ultrasonic timing) */
/* ------------------------------------------------------------------ */
typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SMCR;
    __IO uint32_t DIER;
    __IO uint32_t SR;
    __IO uint32_t EGR;
    __IO uint32_t CCMR1;
    __IO uint32_t CCMR2;
    __IO uint32_t CCER;
    __IO uint32_t CNT;
    __IO uint32_t PSC;
    __IO uint32_t ARR;
} TIM_TypeDef;
#define TIM2 ((TIM_TypeDef *)0x40000000UL)

#define TIM_CR1_CEN (1UL << 0)

/* ------------------------------------------------------------------ */
/* USART1 (optional serial diagnostics console)                        */
/* ------------------------------------------------------------------ */
typedef struct {
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t BRR;
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t CR3;
    __IO uint32_t GTPR;
} USART_TypeDef;
#define USART1 ((USART_TypeDef *)0x40013800UL)

#define USART_SR_TXE  (1UL << 7)
#define USART_CR1_UE  (1UL << 13)
#define USART_CR1_TE  (1UL << 3)
#define USART_CR1_RE  (1UL << 2)

/* ------------------------------------------------------------------ */
/* I2C1 (drives the PCF8574-backed character LCD)                      */
/* ------------------------------------------------------------------ */
typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t OAR1;
    __IO uint32_t OAR2;
    __IO uint32_t DR;
    __IO uint32_t SR1;
    __IO uint32_t SR2;
    __IO uint32_t CCR;
    __IO uint32_t TRISE;
} I2C_TypeDef;
#define I2C1 ((I2C_TypeDef *)0x40005400UL)

#define I2C_CR1_PE    (1UL << 0)
#define I2C_CR1_START (1UL << 8)
#define I2C_CR1_STOP  (1UL << 9)
#define I2C_CR1_ACK   (1UL << 10)

#define I2C_SR1_SB    (1UL << 0)
#define I2C_SR1_ADDR  (1UL << 1)
#define I2C_SR1_BTF   (1UL << 2)
#define I2C_SR1_TXE   (1UL << 7)
#define I2C_SR2_BUSY  (1UL << 1)

#define SYSTEM_CLOCK_HZ 72000000UL
#define APB1_CLOCK_HZ (SYSTEM_CLOCK_HZ / 2UL) /* PPRE1 = /2 */

#endif /* STM32F103_REGS_H */
