/**
 * @file utils.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Interface for some random utility functions.
 * @version 0.1
 * @date 2021-12-26
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 */

#pragma once

#include <stdint.h>

/**
 * @brief Initialize utility functions.
 * 
 */
void utils_init();

/**
 * @brief Delay for given duration in milliseconds.
 * 
 * @param ms delay in milliseconds
 */
void delay_ms(uint16_t ms);

/**
 * @brief Get the current system ticks.
 * 
 * After calling \ref utils_init(), an internal counter is incremented every
 * milisecond through an ISR. It starts with a value of 0. After \ref UINT32_MAX
 * miliseconds the counter wraps around. This is after 49.7 days.
 * 
 * @return ticks in milliseconds
 */
uint32_t get_ticks();

/**
 * @brief Maps a value from one range to another.
 * 
 * @note It is save for any of the ranges to have a reversed polarity e.g. when
 * max is lower than min.
 * 
 * @see https://www.arduino.cc/reference/de/language/functions/math/map/
 * 
 * @param x value to map
 * @param in_min start of input range
 * @param in_max end of input range
 * @param out_min start of output range
 * @param out_max end of output range
 * @return remapped value
 */
static inline int map_value(int x, int in_min, int in_max, int out_min, int out_max) {
    return (int64_t)(x - in_min) * (int64_t)(out_max - out_min) / (int64_t)(in_max - in_min) + out_min;
}

/**
 * @brief Maps a value from one range to another range, with 0 as common start.
 * 
 * Similar to \ref map_value(). Only difference is, that the lower bound of the
 * ranges is assumed to be 0.
 * 
 * @param x value to map
 * @param in_max end of input range
 * @param out_max end of output range
 * @return remapped value
 */
static inline int map_value2(int x, int in_max, int out_max) {
    return (int64_t)(x) * (int64_t)(out_max) / (int64_t)(in_max);
}
