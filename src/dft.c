/**
 * @file dft.c
 * @author Reusser Adrian <reusa1@bfh.ch>
 * @brief Module for calculating a discrete fourier transform.
 * @version 0.1
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022 Adrian Reusser
 *
 * Theoretical source for this implementation:
 * https://batchloaf.wordpress.com/2013/12/07/simple-dft-in-c/
 */


#include "dft.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI2 (6.2832f)

/**
 * @brief Twiddle factors of cosine.
 * 
 * Holds the pre calculated cosine twiddle factors, e.g. the n-th roots of
 * unity.
 * @note The values are only valid after a call to \ref dft_init has been made.
 */
float g_twiddle_factors[DFT_N];

/**
 * @brief Implementation for the dft.
 * 
 * Depending it the \ref DFT_USE_ASM macro is defined, the ASM or the C
 * implementation will be used for this.
 * 
 * @param samples samples with a lenght of 2 * N * DFT_UNDER_SAMPLING
 * @param[out] magnitude the calculated magnitudes of length DFT_MAGNITUDE_SIZE
 */
#ifndef DFT_USE_ASM
static void transform_part(int16_t *samples, uint32_t *magnitude);
#else
extern void transform_part(int16_t *samples, uint32_t *magnitude);
#endif

void dft_init() {
    // pre calculate the cosine twiddle factors, e.g. the n-th roots of unity
    for (int n = 0; n < DFT_N; ++n) {
        g_twiddle_factors[n] = cosf(n * PI2 / DFT_N);
    }
}

void dft_transform(int16_t *samples, uint32_t *magnitude) {
    uint32_t p[DFT_PARTS_NUM][DFT_MAGNITUDE_SIZE];
    // run algorithm in batches
    for (int i = 0; i < DFT_PARTS_NUM; ++i) {
        transform_part(samples + i * DFT_PARTS_LENGTH, p[i]);
    }
    // calculate average of magnitudes
    for (int j = 0; j < DFT_MAGNITUDE_SIZE; ++j) {
        uint64_t average = 0;
        for (int i = 0; i < DFT_PARTS_NUM; ++i) {
            average += p[i][j];
        }
        magnitude[j] = average / DFT_PARTS_NUM;
    }
}

#ifndef DFT_USE_ASM
static void transform_part(int16_t *samples, uint32_t *magnitude) {
    float Xre[DFT_N / 2] = {0};
    float Xim[DFT_N / 2] = {0};
    for (int k = 0; k < DFT_N / 2; ++k) {
        int a = 0;
        int b = DFT_SIN_OFFSET;
        for (int n = 0; n < DFT_N; ++n) {
            int32_t s = samples[DFT_SAMPLE_CHANNEL + 2 * n * DFT_UNDER_SAMPLING];
            Xre[k] += s * g_twiddle_factors[a % DFT_N];
            Xim[k] -= s * g_twiddle_factors[b % DFT_N];
            a += k;
            b += k;
        }
        float P = Xre[k] * Xre[k] + Xim[k] * Xim[k];
        // clamp float value, otherwise the conversation is undefined behaviour
        magnitude[k] = P > (float)(UINT32_MAX) ? UINT32_MAX : P;
    }
}
#endif
