/**
 * @file dft_asm.S
 * @author Reusser Adrian <reusa1@bfh.ch>
 * @brief Assembler module for fast calculatione of the fourier transform.
 * @version 0.1
 * @date 2022-01-20
 * 
 * @copyright Copyright (c) 2022 Adrian Reusser
 *
 * If the preprocessor macro \ref DFT_USE_ASM is defined the ASM implementation
 * in this file will be used. Otherwise the dft module uses the C
 * implementation.
 */

.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

#define ASM_SOURCE
#include "dft.h"

.extern g_twiddle_factors
.global transform_part
.type transform_part, %function

.text                           // section text (executable code)

#ifdef DFT_USE_ASM

/**
 * @brief Implementation for the dft.
 * 
 * Depending it the \ref DFT_USE_ASM macro is defined, the ASM or the C
 * implementation will be used for this.
 * 
 * For the "why?" and "how?" of this ASM implementation, see the C file. It is
 * modelled after it.
 * 
 * @param r0 samples with a lenght of 2 * N * DFT_UNDER_SAMPLING
 * @param[out] r1 the calculated magnitudes of length DFT_MAGNITUDE_SIZE
 */
transform_part:
    push    {r4, r5, r6, r7, r8, r9, fp, lr}    // safe registers

    // Loop over the magnitudes that need to be calculated. (k loop)
    ldr     r2, =DFT_MAGNITUDE_SIZE-1

loop_magnitudes:

    ldr     r3, =0                  // offset a into twiddle factors, cosine
    ldr     r4, =DFT_SIN_OFFSET     // offset b into twiddle factors, sine
    vldr    s3, =0                  // init real part to zero
    vldr    s4, =0                  // init imaginary part to zero
    mov     r9, r0                  // store copy of pointer to samples in r9
    ldr     r7, =g_twiddle_factors  // preload address to twiddle factors in r7

    // Loop inside loop, iterate over all samples. (n loop)
    ldr     r5, =DFT_N-1

#if (DFT_SAMPLE_CHANNEL != 0U)
    add     r0, 1                   // add one if the right channel is specified
#endif
loop_samples:
    // Load a sample into r6. The sample comes from the copy of the given
    // parameter (r0) that was saved to r9. Instead of calculating every loop a
    // offset into the sample array we just move the r9 pointer. (*2 because
    // only every second sample is the same channel [left, right], *2 because
    // each sample is 16 bit)
    ldrsh   r6, [r9], 2*DFT_UNDER_SAMPLING*2

    // Load sample from r6 into s2 and convert it to float.
    vmov    s2, r6
    vcvt.f32.s32    s2, s2
    
    // Modulo for a and b offsets into twiddle factors. Instead of calculating a
    // real modulo (x % DFT_N) we just subtract the dividend if it is bigger.
    // r3 is a, and r4 is b
    cmp     r3, DFT_N-1
    it      pl
    subpl   r3, DFT_N
    cmp     r4, DFT_N-1
    it      pl
    subpl   r4, DFT_N

    // Load cosine and sine twiddle factor at the offsets (r3, r4) into fpu at
    // s0 and s1. In r7 is the address of the precalculated twiddle factors.
    // Those get accessed at the offset that r3 and r4 point to. But as the
    // factors are 32 bits, the offset is multiplied with 4 (or just 2 shifted).
    ldr     r8, [r7, r3, LSL 2]
    vmov    s0, r8
    ldr     r8, [r7, r4, LSL 2]
    vmov    s1, r8

    // Multiply the sample value with the cosine twiddle factor.
    // s3 holds the real part of the magnitude.
    vfma.f32        s3, s2, s0      // s3 += s2 * s0

    // Multiply the sample value with the sine twiddle factor.
    // s4 holds the imaginary part of the magnitude.
    vfms.f32        s4, s2, s1      // s4 -= s2 * s1

    // Increment twiddle offsets a and b with loop variable k (r2).
    add     r3, r2
    add     r4, r2

    // Decrement n loop variable an loop until n (r5) = -1.
    subs    r5, 1
    bpl     loop_samples

    // Calculate magnitude (s5) from real an imaginary parts.
    // s5 = s3 * s3 + s4 * s4
    vmul.f32        s5, s3, s3
    vfma.f32        s5, s4, s4

    // Convert magnitude (s5) back into integer (r8).
    vcvt.u32.f32    s5, s5
    vmov    r8, s5

    // Save magnitude into array pointed to by r1. Save it at offset r2 (k loop
    // variable) that is shifted by 2.
    str     r8, [r1, r2, LSL 2]

    // Decrement k loop variable and loop until k (r2) = -1.
    subs    r2, 1
    bpl     loop_magnitudes

    // Restore registers and exit
    pop     {r4, r5, r6, r7, r8, r9, fp, pc}

#endif // DFT_USE_ASM
