/**
 * @file dft.c
 * @author reusa1 (reusa1@bfh.ch)
 * @brief Implementation of calculating a discrete fourier transform.
 * @version 0.1
 * @date 2022-01-07
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
 * 
 * @param samples 
 * @param magnitude 
 * @retval
 */
int dft_transform(int16_t samples[], uint16_t magnitude[]);
