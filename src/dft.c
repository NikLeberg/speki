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

/**
 * @brief Calculate the batch size for the algorithm.
 * 
 * The given input samples are split up into multiple chunks.
 */
#define N (DFT_MAGNITUDE_SIZE * 2)

/**
 * @brief Calculate the batch count for the algorithm.
 * 
 * How many batches of sample data needs to be processed.
 */
#define PARTS_NUM (DFT_SAMPLE_SIZE / (N * DFT_UNDER_SAMPLING * 2))

/**
 * @brief Calculate the length of each sample batch.
 * 
 */
#define PARTS_LENGTH (DFT_SAMPLE_SIZE / PARTS_NUM)

#define PI2 (6.2832)

/**
 * @brief Index offset into \ref g_twiddle_factors for sinus.
 * 
 */
#define SIN_OFFSET (3 * N / 4)

static float g_twiddle_factors[N];

void transform_part(int16_t *samples, uint32_t *magnitude);

void dft_init() {
    // pre calculate the cosine twiddle factors, e.g. the n-th roots of unity
    for (int n = 0; n < N; ++n) {
        g_twiddle_factors[n] = cos(n * PI2 / N);
    }
}

void dft_transform(int16_t *samples, uint32_t *magnitude) {
    uint32_t p[PARTS_NUM][DFT_MAGNITUDE_SIZE];
    // run algorithm in batches
    for (int i = 0; i < PARTS_NUM; ++i) {
        transform_part(samples + i * PARTS_LENGTH, p[i]);
    }
    // calculate average of manitudes
    for (int j = 0; j < DFT_MAGNITUDE_SIZE; ++j) {
        uint64_t average = 0;
        for (int i = 0; i < PARTS_NUM; ++i) {
            average += p[i][j];
        }
        magnitude[j] = average / PARTS_NUM;
    }
}

void transform_part(int16_t *samples, uint32_t *magnitude) {
    float Xre[N / 2] = {0};
    float Xim[N / 2] = {0};
    for (int k = 0; k < N / 2; ++k) {
        int a = 0;
        int b = SIN_OFFSET;
        for (int n = 0; n < N; ++n) {
            int32_t s = samples[DFT_SAMPLE_CHANNEL + 2 * n * DFT_UNDER_SAMPLING];
            Xre[k] += s * g_twiddle_factors[a % N];
            Xim[k] -= s * g_twiddle_factors[b % N];
            a += k;
            b += k;
        }
        float P = Xre[k] * Xre[k] + Xim[k] * Xim[k];
        // clamp float value, otherwise the conversation is undefined behaviour
        magnitude[k] = P > (float)(UINT32_MAX) ? UINT32_MAX : P;
    }
}
