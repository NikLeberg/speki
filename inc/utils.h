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
 * milisecond through an ISR. It starts with a value of 0. After 2^32-1
 * miliseconds the counter wraps around. This is after 49.7 days.
 * 
 * @return ticks in milliseconds
 */
uint32_t get_ticks();

/**
 * @brief Number of available concurrent profilers.
 * 
 * Parameter index of profile_* functions is valid from 0 up to
 * UTIL_MAX_PROFILES - 1.
 */
#define UTIL_MAX_PROFILES (5)

/**
 * @brief Start profiling.
 * 
 * Should be called once before the other profile_* functions will be called.
 * 
 * @param index Index of the profile. 0 up to UTIL_MAX_PROFILES - 1
 */
void profile_start(int index);

/**
 * @brief Enter the profiled section.
 * 
 * Should be called just before entering the function or section of code that is
 * to be profiled.
 * 
 * @param index Index of the profile. 0 up to UTIL_MAX_PROFILES - 1
 */
void profile_enter(int index);

/**
 * @brief Leave the profiled section.
 * 
 * Should be called after leaving the function or section of code that is to be
 * profiled.
 * 
 * @param index Index of the profile. 0 up to UTIL_MAX_PROFILES - 1
 */
void profile_leave(int index);

/**
 * @brief Stop profiling.
 * 
 * Call to get the profiling stats of the profiled section. Even though it is
 * called "stop" the profiling can be continued and at a later time this
 * function can be called again. To reset profiling one should call
 * profile_start().
 * 
 * @note Any parameter (except index) can be NULL if its value isnt of interest.
 * 
 * @param index Index of the profile. 0 up to UTIL_MAX_PROFILES - 1
 * @param[out] duration for how many ticks was profiled (from start to stop)
 * @param[out] min longest ticks took (between enter and leave)
 * @param[out] max minimal ticks took (between enter and leave)
 * @param[out] avg average ticks took (between enter and leave)
 * @param[out] load percentage of all the used ticks compared to the duration
 */
void profile_stop(int index, uint32_t *duration, uint32_t *min, uint32_t *max, uint32_t *avg, float *load);

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
    int ret_val;
    if (x > in_max) {
        ret_val = out_max;
    } else if (x < in_min) {
        ret_val = out_min;
    } else {
        ret_val = (int64_t)(x - in_min) * (int64_t)(out_max - out_min) / (int64_t)(in_max - in_min) + out_min;
    }
    return ret_val;
}

/**
 * @brief Maps a value from one range to another, unsigned variant.
 * 
 * Similar to \ref map_value() but has unsigned type. Special care needs to be
 * taken as subtraction sould underflow if it happended in unsigned
 * representation. Because of this the relevant values are casted to a signed
 * int of twice the size.
 * 
 * @param x value to map
 * @param in_min start of input range
 * @param in_max end of input range
 * @param out_min start of output range
 * @param out_max end of output range
 * @return remapped value
 */
static inline uint32_t map_value_u(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {
    int64_t ret_val;
    if (x > in_max) {
        ret_val = out_max;
    } else if (x < in_min) {
        ret_val = out_min;
    } else {
        int64_t in_range = (int64_t)in_max - (int64_t)in_min;
        int64_t out_range = (int64_t)out_max - (int64_t)out_min;
        ret_val = (x - in_min) * out_range / in_range + out_min;
    }
    return ret_val;
}
