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

.text                           // section text (executable code)

#ifdef DFT_USE_ASM

// r0: samples, r1: mangitude
.type transform_part, %function
transform_part:
    push {r4}                   // safe registers
    ldr r3, =0                  // counter for loop
loop:
    str r3, [r1, r3]            // store twiddle as magnitude value
    add r3, r3, #1              // increment loop counter
    cmp r3, DFT_MAGNITUDE_SIZE
    beq loop                    // end loop when all magnitudes were saved
end:
    pop {r4}                    // restore used registers
    mov pc, lr                  // exit function

#endif // DFT_USE_ASM
