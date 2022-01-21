/**
 * @file utils.c
 * @author Leuenberger Niklaus <leuen4@bfh.ch>
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

static struct {
    uint32_t start;
    uint32_t last_enter;
    uint32_t sum_diff;
    uint32_t num;
    uint32_t min;
    uint32_t max;
} g_profile_stats[UTIL_MAX_PROFILES];

void profile_start(int index) {
    g_profile_stats[index].min = UINT32_MAX;
    g_profile_stats[index].max = 0;
    g_profile_stats[index].sum_diff = 0;
    g_profile_stats[index].num = 0;
    g_profile_stats[index].start = get_ticks();
}

void profile_enter(int index) {
    g_profile_stats[index].last_enter = get_ticks();
}

void profile_leave(int index) {
    uint32_t diff = get_ticks() - g_profile_stats[index].last_enter;
    if (diff < g_profile_stats[index].min) {
        g_profile_stats[index].min = diff;
    } else if (diff > g_profile_stats[index].max) {
        g_profile_stats[index].max = diff;
    }
    g_profile_stats[index].sum_diff += diff;
    g_profile_stats[index].num++;
}

void profile_stop(int index, uint32_t *duration, uint32_t *min, uint32_t *max, uint32_t *avg, float *load) {
    uint32_t dur = get_ticks() - g_profile_stats[index].start;
    if (duration) {
        *duration = dur;
    }
    if (min) {
        *min = g_profile_stats[index].min;
    }
    if (max) {
        *max = g_profile_stats[index].max;
    }
    if (avg) {
        *avg = g_profile_stats[index].sum_diff / g_profile_stats[index].num;
    }
    if (load) {
        *load = (float)g_profile_stats[index].sum_diff / (float)dur;
    }
}
