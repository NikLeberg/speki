/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_I2S I2S
 * @brief		CARME-M4 I2S high level functions
 * @{
 *
 * @file		i2s.c
 * @version		1.0
 * @date		2013-03-06
 * @author		rct1
 *
 * @brief		I2S board support package for the CARME-M4 module.
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
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <i2s.h>					/* CARME I2S definitions				*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	I2S Port and Pin association
 */
static CARME_Port_Pin_t CARME_I2S_Port_Pin[] = {
        /* I2S used for Audio codec */
        { GPIOC, GPIO_Pin_6, GPIO_Mode_AF, GPIO_AF_SPI2 },	/**< I2S MCK	*/
        { GPIOB, GPIO_Pin_9, GPIO_Mode_AF, GPIO_AF_SPI2 },	/**< I2S WS		*/
        { GPIOI, GPIO_Pin_1, GPIO_Mode_AF, GPIO_AF_SPI2 },	/**< I2S CK		*/
        { GPIOI, GPIO_Pin_2, GPIO_Mode_AF, GPIO_AF_SPI3 },	/**< I2S extSD	*/
        { GPIOI, GPIO_Pin_3, GPIO_Mode_AF, GPIO_AF_SPI2 },	/**< I2S SD		*/
};

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		I2S GPIO initialization
 *
 * @return		None
 *****************************************************************************
 */
void CARME_I2S_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure the GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	CARME_GPIO_Init(CARME_I2S_Port_Pin, &GPIO_InitStruct,
	                sizeof(CARME_I2S_Port_Pin) / sizeof(CARME_Port_Pin_t));
}

#ifdef __cplusplus
 }
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
