/*
 * Minimal startup file for STM32F103C8 (medium-density Cortex-M3):
 * vector table + Reset_Handler (copies .data, zeroes .bss, calls main).
 * All peripheral IRQs default to an infinite loop since this project
 * polls instead of using interrupts.
 */
    .syntax unified
    .cpu cortex-m3
    .thumb

    .global Reset_Handler
    .global g_vectors

/* ------------------------------------------------------------------ */
/* Vector table                                                        */
/* ------------------------------------------------------------------ */
    .section .isr_vector, "a", %progbits
    .type g_vectors, %object
g_vectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

    /* IRQ0..IRQ42 (medium-density STM32F103 peripheral interrupts) */
    .word WWDG_IRQHandler
    .word PVD_IRQHandler
    .word TAMPER_IRQHandler
    .word RTC_IRQHandler
    .word FLASH_IRQHandler
    .word RCC_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word DMA1_Channel1_IRQHandler
    .word DMA1_Channel2_IRQHandler
    .word DMA1_Channel3_IRQHandler
    .word DMA1_Channel4_IRQHandler
    .word DMA1_Channel5_IRQHandler
    .word DMA1_Channel6_IRQHandler
    .word DMA1_Channel7_IRQHandler
    .word ADC1_2_IRQHandler
    .word USB_HP_CAN1_TX_IRQHandler
    .word USB_LP_CAN1_RX0_IRQHandler
    .word CAN1_RX1_IRQHandler
    .word CAN1_SCE_IRQHandler
    .word EXTI9_5_IRQHandler
    .word TIM1_BRK_IRQHandler
    .word TIM1_UP_IRQHandler
    .word TIM1_TRG_COM_IRQHandler
    .word TIM1_CC_IRQHandler
    .word TIM2_IRQHandler
    .word TIM3_IRQHandler
    .word TIM4_IRQHandler
    .word I2C1_EV_IRQHandler
    .word I2C1_ER_IRQHandler
    .word I2C2_EV_IRQHandler
    .word I2C2_ER_IRQHandler
    .word SPI1_IRQHandler
    .word SPI2_IRQHandler
    .word USART1_IRQHandler
    .word USART2_IRQHandler
    .word USART3_IRQHandler
    .word EXTI15_10_IRQHandler
    .word RTCAlarm_IRQHandler
    .word USBWakeup_IRQHandler

/* ------------------------------------------------------------------ */
/* Reset handler                                                       */
/* ------------------------------------------------------------------ */
    .section .text.Reset_Handler, "ax", %progbits
    .type Reset_Handler, %function
Reset_Handler:
    ldr r0, =_estack
    mov sp, r0

    /* Copy .data from flash to RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
copy_data_loop:
    cmp r0, r1
    bcs copy_data_done
    ldr r3, [r2], #4
    str r3, [r0], #4
    b copy_data_loop
copy_data_done:

    /* Zero .bss */
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0
zero_bss_loop:
    cmp r0, r1
    bcs zero_bss_done
    str r2, [r0], #4
    b zero_bss_loop
zero_bss_done:

    bl main
hang:
    b hang
    .size Reset_Handler, . - Reset_Handler

/* ------------------------------------------------------------------ */
/* Default handler + weak aliases for everything unused                */
/* ------------------------------------------------------------------ */
    .section .text.Default_Handler, "ax", %progbits
Default_Handler:
    b Default_Handler
    .size Default_Handler, . - Default_Handler

    .macro weak_alias name
    .weak \name
    .thumb_set \name, Default_Handler
    .endm

    weak_alias NMI_Handler
    weak_alias HardFault_Handler
    weak_alias MemManage_Handler
    weak_alias BusFault_Handler
    weak_alias UsageFault_Handler
    weak_alias SVC_Handler
    weak_alias DebugMon_Handler
    weak_alias PendSV_Handler
    weak_alias SysTick_Handler

    weak_alias WWDG_IRQHandler
    weak_alias PVD_IRQHandler
    weak_alias TAMPER_IRQHandler
    weak_alias RTC_IRQHandler
    weak_alias FLASH_IRQHandler
    weak_alias RCC_IRQHandler
    weak_alias EXTI0_IRQHandler
    weak_alias EXTI1_IRQHandler
    weak_alias EXTI2_IRQHandler
    weak_alias EXTI3_IRQHandler
    weak_alias EXTI4_IRQHandler
    weak_alias DMA1_Channel1_IRQHandler
    weak_alias DMA1_Channel2_IRQHandler
    weak_alias DMA1_Channel3_IRQHandler
    weak_alias DMA1_Channel4_IRQHandler
    weak_alias DMA1_Channel5_IRQHandler
    weak_alias DMA1_Channel6_IRQHandler
    weak_alias DMA1_Channel7_IRQHandler
    weak_alias ADC1_2_IRQHandler
    weak_alias USB_HP_CAN1_TX_IRQHandler
    weak_alias USB_LP_CAN1_RX0_IRQHandler
    weak_alias CAN1_RX1_IRQHandler
    weak_alias CAN1_SCE_IRQHandler
    weak_alias EXTI9_5_IRQHandler
    weak_alias TIM1_BRK_IRQHandler
    weak_alias TIM1_UP_IRQHandler
    weak_alias TIM1_TRG_COM_IRQHandler
    weak_alias TIM1_CC_IRQHandler
    weak_alias TIM2_IRQHandler
    weak_alias TIM3_IRQHandler
    weak_alias TIM4_IRQHandler
    weak_alias I2C1_EV_IRQHandler
    weak_alias I2C1_ER_IRQHandler
    weak_alias I2C2_EV_IRQHandler
    weak_alias I2C2_ER_IRQHandler
    weak_alias SPI1_IRQHandler
    weak_alias SPI2_IRQHandler
    weak_alias USART1_IRQHandler
    weak_alias USART2_IRQHandler
    weak_alias USART3_IRQHandler
    weak_alias EXTI15_10_IRQHandler
    weak_alias RTCAlarm_IRQHandler
    weak_alias USBWakeup_IRQHandler

    .end
