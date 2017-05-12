/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_I2C I2C
 * @brief		CARME-M4 I2C high level functions
 * @{
 *
 * @file		i2c.c
 * @version		1.0
 * @date		2013-01-31
 * @author		rct1
 *
 * @brief		I2C board support package for the CARME module.
 *
 * @bug			The I2C system don't work correctly, when code optimization
 *				O2 is enabled.
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
 * @example		i2c.c
 *				Explains how to send values to the 7-digit module over I2C.
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <i2c.h>

/*----- Macros -------------------------------------------------------------*/
#define TIMEOUT_MAX			0x3000		/**< I2C timeout in systicks		*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
static void CARME_I2C_Settings(I2C_TypeDef *I2Cx);
static ERROR_CODES CARME_I2C_Timeout(I2C_TypeDef* I2Cx);

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	I2C Port and Pin association
 */
static CARME_Port_Pin_t CARME_I2C_Port_Pin[] = {
	/* CARME I2C Board */
	{ GPIOH, GPIO_Pin_4, GPIO_Mode_AF, GPIO_AF_I2C2 },	/**< SCL	*/
	{ GPIOH, GPIO_Pin_5, GPIO_Mode_AF, GPIO_AF_I2C2 },	 /**< SDA	*/
	/* CARME I2C Audio Codec */
	{ GPIOH, GPIO_Pin_7, GPIO_Mode_AF, GPIO_AF_I2C3 },	/**< SCL	*/
	{ GPIOH, GPIO_Pin_8, GPIO_Mode_AF, GPIO_AF_I2C3 }	/**< SDA	*/
};

/**
 * @brief	I2C init structures to reinitialize the interface,
 *			if it is crashed.
 */
static I2C_InitTypeDef I2C_InitStruct[2];

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		I2C initialization
 *
 * @param[in]	I2Cx	The I2C port which will be used. Possible values are
 *						#CARME_I2C_BOARD and #CARME_I2C_AUDIO.
 * @return		None
 *****************************************************************************
 */
void CARME_I2C_Init(I2C_TypeDef *I2Cx) {

	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);

	/* Configure the GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	CARME_GPIO_Init(CARME_I2C_Port_Pin, &GPIO_InitStruct,
			sizeof(CARME_I2C_Port_Pin) / sizeof(CARME_Port_Pin_t));

	/* Configure the I2C */
	if (I2Cx == CARME_I2C_BOARD ) {

		I2C_StructInit(&I2C_InitStruct[0]);
		I2C_InitStruct[0].I2C_ClockSpeed = CARME_I2C_SPEED;
		I2C_InitStruct[0].I2C_OwnAddress1 = CARME_I2C_ADDR_BOARD;
		I2C_InitStruct[0].I2C_Ack = I2C_Ack_Enable;
	} else if (I2Cx == CARME_I2C_AUDIO ) {

		I2C_StructInit(&I2C_InitStruct[1]);
		I2C_InitStruct[1].I2C_ClockSpeed = CARME_I2C_SPEED;
		I2C_InitStruct[1].I2C_OwnAddress1 = CARME_I2C_ADDR_AUDIO;
		I2C_InitStruct[1].I2C_Ack = I2C_Ack_Enable;
	}
	CARME_I2C_Settings(I2Cx);
}

/**
 *****************************************************************************
 * @brief		I2C interface initialization
 *
 * @param[in]	I2Cx	The I2C port which will be used. Possible values are
 *						#CARME_I2C_BOARD and #CARME_I2C_AUDIO.
 * @return		None
 *****************************************************************************
 */
static void CARME_I2C_Settings(I2C_TypeDef *I2Cx) {

	uint8_t i;

	if (I2Cx == CARME_I2C_BOARD ) {
		I2C_Cmd(CARME_I2C_BOARD, DISABLE);
		I2C_DeInit(CARME_I2C_BOARD );
		I2C_Init(I2Cx, &I2C_InitStruct[0]);
		I2C_Cmd(I2Cx, ENABLE);
	} else if (I2Cx == CARME_I2C_AUDIO ) {
		I2C_Cmd(CARME_I2C_AUDIO, DISABLE);
		I2C_DeInit(CARME_I2C_AUDIO );
		I2C_Init(I2Cx, &I2C_InitStruct[1]);
		I2C_Cmd(I2Cx, ENABLE);
	}

	i = 0xFF;
	while (i--)
		/* Wait until hardware is running */
		;
}

/**
 *****************************************************************************
 * @brief		Restart the I2C periph if a timeout has occurred.
 *
 * @param[in]	I2Cx	The I2C port which will be used. Possible values are
 *						#CARME_I2C_BOARD and #CARME_I2C_AUDIO.
 * @return		#ERROR_CODES
 *				@arg #CARME_NO_ERROR
 *				@arg #CARME_ERROR_I2C_TIMEOUT
 *****************************************************************************
 */
static ERROR_CODES CARME_I2C_Timeout(I2C_TypeDef* I2Cx) {

	I2C_GenerateSTOP(I2Cx, ENABLE);
	I2C_SoftwareResetCmd(I2Cx, ENABLE);
	I2C_SoftwareResetCmd(I2Cx, DISABLE);
	CARME_I2C_Settings(I2Cx);

	return CARME_ERROR_I2C_TIMEOUT;
}

/**
 *****************************************************************************
 * @brief		Write data to an I2C slave device on a specific register
 *				address.
 *
 * @param[in]	I2Cx	The I2C port which will be used. Possible values are
 *						#CARME_I2C_BOARD and #CARME_I2C_AUDIO.
 * @param[in]	addr	Address of the slave device.
 * @param[in]	reg		Register address of the slave device.
 * @param[in]	twoByte	If the register address is two Byte long (ex. 0x01F3)
 *						set this parameter to 1.
 * @param[in]	pdata	Pointer to the data array.
 * @param[in]	count	Number of bytes in the array.
 * @return		#ERROR_CODES
 *				@arg #CARME_NO_ERROR
 *				@arg #CARME_ERROR_I2C_TIMEOUT
 *****************************************************************************
 */
ERROR_CODES CARME_I2C_Write(I2C_TypeDef *I2Cx, uint8_t addr, uint16_t reg,
		uint8_t twoByte, uint8_t *pdata, uint16_t count) {

	uint32_t timeout = TIMEOUT_MAX;
	uint16_t i;

	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY )) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	I2C_GenerateSTART(I2Cx, ENABLE);
	timeout = TIMEOUT_MAX;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT )) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	I2C_Send7bitAddress(I2Cx, addr, I2C_Direction_Transmitter );
	timeout = TIMEOUT_MAX;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	if (twoByte == 1) {
		I2C_SendData(I2Cx, (uint8_t) (reg >> 8));
		timeout = TIMEOUT_MAX;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
			if (--timeout == 0) {
				return CARME_I2C_Timeout(I2Cx);
			}
		}

		I2C_SendData(I2Cx, (uint8_t) reg);
		timeout = TIMEOUT_MAX;
		while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF )) {
			if (--timeout == 0) {
				CARME_I2C_Timeout(I2Cx);
			}
		}
	} else {
		I2C_SendData(I2Cx, (uint8_t) reg);
		timeout = TIMEOUT_MAX;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
			if (--timeout == 0) {
				return CARME_I2C_Timeout(I2Cx);
			}
		}
	}

	for (i = 0; i < count; i++) {
		I2C_SendData(I2Cx, pdata[i]);
		timeout = TIMEOUT_MAX;
		while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF )) {
			if (--timeout == 0) {
				CARME_I2C_Timeout(I2Cx);
			}
		}
	}

	I2C_GenerateSTOP(I2Cx, ENABLE);
	return CARME_NO_ERROR;
}

/**
 *****************************************************************************
 * @brief		Read data from an I2C slave device on a specific register
 *				address.
 *
 * @param[in]	I2Cx	The I2C port which will be used. Possible values are
 *						#CARME_I2C_BOARD and #CARME_I2C_AUDIO.
 * @param[in]	addr	Address of the slave device.
 * @param[in]	reg		Register address of the slave device.
 * @param[in]	twoByte	If the register address is two Byte long (ex. 0x01F3)
 *						set this parameter to 1.
 * @param[out]	pdata	Pointer to the data array.
 * @param[in]	count	Number of bytes in the array.
 * @return		#ERROR_CODES
 *				@arg #CARME_NO_ERROR
 *				@arg #CARME_ERROR_I2C_TIMEOUT
 *****************************************************************************
 */
ERROR_CODES CARME_I2C_Read(I2C_TypeDef *I2Cx, uint8_t addr, uint16_t reg,
		uint8_t twoByte, uint8_t *pdata, uint16_t count) {

	uint32_t timeout = TIMEOUT_MAX;
	uint16_t i;

	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY )) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	I2C_GenerateSTART(I2Cx, ENABLE);
	timeout = TIMEOUT_MAX;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT )) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	I2C_Send7bitAddress(I2Cx, addr, I2C_Direction_Transmitter );
	timeout = TIMEOUT_MAX;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	if (twoByte == 1) {
		I2C_SendData(I2Cx, (uint8_t) (reg >> 8));
		timeout = TIMEOUT_MAX;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
			if (--timeout == 0) {
				return CARME_I2C_Timeout(I2Cx);
			}
		}

		I2C_SendData(I2Cx, (uint8_t) reg);
		timeout = TIMEOUT_MAX;
		while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF )) {
			if (--timeout == 0) {
				CARME_I2C_Timeout(I2Cx);
			}
		}
	} else {
		I2C_SendData(I2Cx, (uint8_t) reg);
		timeout = TIMEOUT_MAX;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {
			if (--timeout == 0) {
				return CARME_I2C_Timeout(I2Cx);
			}
		}
	}

	I2C_GenerateSTART(I2Cx, ENABLE);
	timeout = TIMEOUT_MAX;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT )) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	I2C_Send7bitAddress(I2Cx, addr, I2C_Direction_Receiver );
	timeout = TIMEOUT_MAX;
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR ) == RESET) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	for (i = 0; i < count - 1; i++) {
		timeout = TIMEOUT_MAX;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED )) {
			if (--timeout == 0) {
				return CARME_I2C_Timeout(I2Cx);
			}
		}
		pdata[i] = I2C_ReceiveData(I2Cx);
	}

	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	(void) I2Cx->SR2;

	I2C_GenerateSTOP(I2Cx, ENABLE);
	timeout = TIMEOUT_MAX;
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE ) == RESET) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	pdata[count - 1] = I2C_ReceiveData(I2Cx);
	timeout = TIMEOUT_MAX;
	while (I2Cx->CR1 & I2C_CR1_STOP ) {
		if (--timeout == 0) {
			return CARME_I2C_Timeout(I2Cx);
		}
	}

	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	I2C_ClearFlag(I2Cx, I2C_FLAG_AF );

	return CARME_NO_ERROR;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
