#include "systick.h"
#include "stm32f103_regs.h"

static volatile uint32_t g_millis = 0;

void SysTick_Handler(void) {
    g_millis++;
}

static void rcc_clock_72mhz(void) {
    /* Enable HSE and wait for it to stabilize. */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}

    /* 2 wait states are required in flash ACR above 48 MHz @ 3.3V. */
    volatile uint32_t *FLASH_ACR = (volatile uint32_t *)0x40022000UL;
    *FLASH_ACR = (*FLASH_ACR & ~0x7UL) | 0x02UL;

    /* APB1 (36 MHz max) = SYSCLK/2, APB2 = SYSCLK, AHB = SYSCLK. */
    RCC->CFGR = (RCC->CFGR & ~(0x7UL << 8)) | RCC_CFGR_PPRE1_DIV2;

    /* PLL source = HSE (not divided), multiplier x9 => 8 MHz x 9 = 72 MHz. */
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL9;

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    RCC->CFGR = (RCC->CFGR & ~0x3UL) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL) {}
}

static void tim2_us_timebase_init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = (SYSTEM_CLOCK_HZ / 1000000UL) - 1UL; /* 1 tick = 1 us */
    TIM2->ARR = 0xFFFFUL;
    TIM2->CNT = 0;
    TIM2->CR1 = TIM_CR1_CEN;
}

void clock_init(void) {
    rcc_clock_72mhz();

    SysTick->LOAD = (SYSTEM_CLOCK_HZ / 1000UL) - 1UL; /* 1 ms tick */
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE | SysTick_CTRL_TICKINT |
                    SysTick_CTRL_ENABLE;

    tim2_us_timebase_init();
}

uint32_t millis(void) {
    return g_millis;
}

void delay_ms(uint32_t ms) {
    uint32_t start = g_millis;
    while ((g_millis - start) < ms) {}
}

void delay_us(uint32_t us) {
    uint16_t start = (uint16_t)TIM2->CNT;
    while ((uint16_t)((uint16_t)TIM2->CNT - start) < (uint16_t)us) {}
}
