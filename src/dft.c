/**
 * @file dft.c
 * @author reusa1 (reusa1@bfh.ch)
 * @brief Module for calculating a discrete fourier transform.
 * @version 0.1
 * @date 2022-01-14
 * 
 * @copyright Copyright (c) 2022 Adrian Reusser
 *
 */


#include "dft.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define N 60

int dft_transform(int16_t samples[], uint32_t magnitude[]) {
    // Twiddle factors (60th roots of unity)
    const float W[N] = {
        1.00000, 0.99452, 0.97815, 0.95106, 0.91355, 0.86602, 0.80902, 0.74314,
        0.66913, 0.58778, 0.50000, 0.40673, 0.30901, 0.20791, 0.10453, -0.00000,
        -0.10453, -0.20792, -0.30902, -0.40674, -0.50000, -0.58779, -0.66913, -0.74315,
        -0.80902, -0.86603, -0.91355, -0.95106, -0.97815, -0.99452, -1.00000, -0.99452,
        -0.97815, -0.95105, -0.91354, -0.86602, -0.80901, -0.74314, -0.66912, -0.58778,
        -0.49999, -0.40673, -0.30901, -0.20790, -0.10452, 0.00001, 0.10454, 0.20792,
        0.30903, 0.40675, 0.50001, 0.58780, 0.66914, 0.74315, 0.80902, 0.86603,
        0.91355, 0.95106, 0.97815, 0.99452};

    float Xre[N / 2 + 1], Xim[N / 2 + 1]; // DFT of x (real and imaginary parts)

    // Calculate DFT and power spectrum up to Nyquist frequency
    int to_sin = 3 * N / 4; // index offset for sin
    int a, b;
    for (int k = 0; k <= N / 2; ++k) {
        Xre[k] = 0;
        Xim[k] = 0;
        a = 0;
        b = to_sin;
        for (int n = 0; n < N; ++n) {
            Xre[k] += samples[2 * n] * W[a % N];
            Xim[k] -= samples[2 * n] * W[b % N];
            a += k;
            b += k;
        }
        magnitude[k] = Xre[k] * Xre[k] + Xim[k] * Xim[k];
    }

    return 0;
}
