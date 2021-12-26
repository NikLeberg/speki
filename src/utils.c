/**
 * @file utils.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Module for some random utility functions.
 * @version 0.1
 * @date 2021-12-26
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 */

#include <stm32f4xx.h>

#include "utils.h"

void utils_init() {
    // set system tick interrupt to be called every ms (1 kHz)
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);
    SysTick_Config(clocks.HCLK_Frequency / 1000 - 1);
}

static __IO uint32_t system_ticks;

void delay_ms(uint16_t ms) {
    int32_t start_ticks;
    start_ticks = system_ticks;
    while ((system_ticks - start_ticks) < ms) {
        ; // adds approx 110us
    }
}

void SysTick_Handler(void) {
    system_ticks++;
}

uint32_t get_ticks() {
    return system_ticks;
}
