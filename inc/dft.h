/**
 * @file dft.h
 * @author Reusser Adrian <reusa1@bfh.ch>
 * @brief Interface for calculation of the discrete fourier transform.
 * @version 0.1
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022 Adrian Reusser
 * 
 */

#pragma once

#ifndef ASM_SOURCE
#include <stddef.h>
#include <stdint.h>
#endif

/**
 * @brief How many stereo samples the dft function will get.
 * 
 * Depending on \ref DFT_UNDER_SAMPLING all or every n-th sample will be used.
 */
#define DFT_SAMPLE_SIZE (1920U)

/**
 * @brief Channel to use for the dft.
 * 
 * 0 = left, 1 = right
 */
#define DFT_SAMPLE_CHANNEL (0U)

/**
 * @brief Determines the undersampling of the given sample input.
 * 
 * For example if this is set to (1U), every sample will be used for the dft.
 * Or if it is set to (4U) every n-th sample of the input samples will be used.
 */
#define DFT_UNDER_SAMPLING (4U)

/**
 * @brief How many magnitudes should be calculated.
 * 
 */
#define DFT_MAGNITUDE_SIZE (30U)

/**
 * @brief Enable / disble ASM implementation
 * 
 * ASM implementation is enabled if this macro is defined, comment to disable.
 */
// #define DFT_USE_ASM (1U)

/**
 * @brief Calculate the batch size for the algorithm.
 * 
 * The given input samples are split up into multiple chunks.
 */
#define DFT_N (DFT_MAGNITUDE_SIZE * 2)

/**
 * @brief Calculate the batch count for the algorithm.
 * 
 * How many batches of sample data needs to be processed.
 */
#define DFT_PARTS_NUM (DFT_SAMPLE_SIZE / (DFT_N * DFT_UNDER_SAMPLING * 2))

/**
 * @brief Calculate the length of each sample batch.
 * 
 */
#define DFT_PARTS_LENGTH (DFT_SAMPLE_SIZE / DFT_PARTS_NUM)

/**
 * @brief Index offset into \ref g_twiddle_factors for sinus.
 * 
 */
#define DFT_SIN_OFFSET (3 * DFT_N / 4)


#ifndef ASM_SOURCE

/**
 * @brief Calculates the initial twiddle factors for dft algorithm.
 * 
 * @note Has to be called once before \ref dft_transform() can be used.
 */
void dft_init();

/**
 * @brief Calculate the dft magnitudes of the given samples.
 * 
 * The inner workings can be modified with the precompiler parameters
 * \ref DFT_SAMPLE_SIZE, \ref DFT_UNDER_SAMPLING and \ref DFT_MAGNITUDE_SIZE.
 * @note Before using this function, \ref dft_init() should be called once.
 * 
 * @param[in] samples arrary with samples of length DFT_SAMPLE_SIZE
 * @param[out] magnitude array with magnitudes DFT_MAGNITUDE_SIZE in length
 */
void dft_transform(int16_t *samples, uint32_t *magnitude);

#endif // ASM_SOURCE
