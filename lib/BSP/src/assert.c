/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_Assert Assertion
 * @brief		CARME-M4 Assertion handling
 * @{
 *
 * @file		assert.c
 * @version		1.0
 * @date		2013-09-02
 * @author		rct1
 *
 * @brief		Print assert messages to the stdout.\n
 *				If stdout is a serial port, please initialize the port at
 *				startup.
 *
 *****************************************************************************
 * @copyright
 * @{
 *
 * This software can be used by students and other personal of the Bern
 * University of Applied Sciences under the terms of the MIT license.
 * For other persons this software is under the terms of the GNU General
 * Public License version 2.
 *
 * Copyright &copy; 2013, Bern University of Applied Sciences.
 * All rights reserved.
 *
 *
 * ##### MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *
 * ##### GNU GENERAL PUBLIC LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 * @}
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <assert.h>					/* Assertion handling					*/
#include <stdio.h>					/* Standard input and output			*/

#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <uart.h>					/* CARME BSP UART port					*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/
#ifndef NDEBUG
/**
 *****************************************************************************
 * @brief		Reports the name of the source file and the source line number
 *				where the assert error has occurred.
 *
 * @param[in]	file	pointer to the source file name
 * @param[in]	line	assert error line source number
 * @param[in]	func	pointer to the source function name
 * @return		None
 *****************************************************************************
 */
void __assert(const char *file, int line, const char *func) {

	printf("Assert failed in file %s on line %d in function %s\n", file, line,
	       func);

	while (1)
		;
}

/**
 *****************************************************************************
 * @brief		Reports the name of the source file and the source line number
 *				where the assert error has occurred.
 *
 * @param[in]	file		pointer to the source file name
 * @param[in]	line		assert_param error line source number
 * @param[in]	func		pointer to the source function name
 * @param[in]	additional	pointer to additional informations
 * @return		None
 *****************************************************************************
 */
void __assert_func(const char *file, int line, const char *func,
                   const char *additional) {

	printf("Assert failed in file %s on line %d in function %s, additional %s\n",
	       file, line, func, additional);

	while (1)
		;
}
#endif /* NDEBUG */

#ifdef USE_FULL_ASSERT
/**
 *****************************************************************************
 * @brief		Reports the name of the source file and the source line number
 *				where the assert_param error has occurred.
 *
 * @param[in]	file	pointer to the source file name
 * @param[in]	line	assert_param error line source number
 *
 * @return		None
 *****************************************************************************
 */
void assert_failed(uint8_t* file, uint32_t line) {

	printf("Assert failed in file %s on line %lu\n", file, line);

	while (1)
		;
}
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
