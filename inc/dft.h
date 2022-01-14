/**
 * @file dft.c
 * @author reusa1 (reusa1@bfh.ch)
 * @brief Implementation of calculating a discrete fourier transform.
 * @version 0.1
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022 Adrian Reusser
 * 
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief 
 * 
 */
#define DFT_SAMPLE_SIZE (60U)

/**
 * @brief 
 * 
 */
#define DFT_MAGNITUDE_SIZE (30U)

/**
 * @brief 
 * calculates magnitudes for the spectogram
 * uses first 60 data points of 1024
 * uses one channel to calculate
 * cos is stored as an array of parameters so calculation can bea avoided 
 * 
 * @param samples 
 * @param magnitude 
 * @retval
 */
int dft_transform(int16_t samples[], uint32_t magnitude[]);
