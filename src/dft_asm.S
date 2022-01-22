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
 * @param r0 samples with a lenght of 2 * N * DFT_UNDER_SAMPLING
 * @param[out] r1 the calculated magnitudes of length DFT_MAGNITUDE_SIZE
 */
transform_part:
    push    {fp, lr}               // safe registers

    // Allocate space for local variables on the stack and save the resulting
    // stack pointer as frame pointer.
    //sub     sp, 4
    //mov     fp, sp

    // Load length of magnitude array (in words) into r2, zero into r0 and use
    // these to loop over each value and set it to zero. The address of the
    // magnitude array is still in r1 from the function call. After each write
    // r2 is decremented. If it reached 0, the loop ends.
    ldr     r2, =DFT_MAGNITUDE_SIZE
    ldr     r0, =0
loop:
    str     r0, [r1], 4
    subs    r2, 1
    bne     loop

    // Release allocated space on stack.
    //add     sp, 4

    pop     {fp, lr}                // restore registers
    mov     pc, lr                  // exit function

#endif // DFT_USE_ASM
