/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_EEPROM External EEPROM
 * @brief		EEPROM on CARME mainboard
 * @{
 *
 * @file		eeprom.c
 * @version		1.0
 * @date		2013-11-28
 * @author		zif1
 *
 * @brief		Write and read data to the onboard non volatile EEPROM
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
extern "C"
{
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <i2c.h>					/* CARME I2C definitions				*/
#include <eeprom.h>					/* CARME EEPROM							*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		Write an Array of 8-bit data to the Eeprom\n
 * 				This Function can take several ms to perfrom. 5ms are needed
 *				for a Pagewrite (8 Byte).
 *
 * @param[in]	data			data to write
 * @param[in]	nbrOfChar		number of bytes to write to the EEPROM
 * @param[in]	startAddress	Startaddress to write to
 * 								Range (0x0000 - 0x07FF)
 * @return		#ERROR_CODES
 *				@arg #CARME_NO_ERROR
 *				@arg #CARME_ERROR_EEPROM_OVERFLOW
 *				@arg #CARME_ERROR_EEPROM_BUSY
 *****************************************************************************
 */
ERROR_CODES CARME_EEPROM_Write(uint8_t *data, uint8_t nbrOfChar,
                               uint8_t startAddress) {

	uint8_t charstoWrite;
	uint32_t timeout;
	uint32_t i;

	/*Set timeout to ~6ms*/
	timeout = 200000;

	if ((startAddress + nbrOfChar) > 0x07FF) {
		return CARME_ERROR_EEPROM_OVERFLOW;
	}

	/*Align to Page*/
	charstoWrite = startAddress % 8;
	if (charstoWrite > 0) {
		charstoWrite = 8 - charstoWrite;
		CARME_I2C_Write(CARME_I2C_BOARD, CARME_EEPROM_I2C_ADDRESS,
		                startAddress, 1, (uint8_t *) data, charstoWrite);

		/*delay*/
		for (i = 0; i < timeout; i++)
			;

		nbrOfChar -= charstoWrite;
		data += charstoWrite;
		startAddress += charstoWrite;
	}

	while (nbrOfChar > 0) {
		if (nbrOfChar > 8) {
			charstoWrite = 8;
		}
		else {
			charstoWrite = nbrOfChar;
		}
		/*Write the remaining bytes*/
		CARME_I2C_Write(CARME_I2C_BOARD, CARME_EEPROM_I2C_ADDRESS,
		                startAddress, 1, (uint8_t *) data, charstoWrite);

		/*delay*/
		for (i = 0; i < timeout; i++)
			;

		nbrOfChar -= charstoWrite;
		data += charstoWrite;
		startAddress += charstoWrite;

	}

	return CARME_NO_ERROR;
}

/**
 *****************************************************************************
 * @brief		Read an Array of char to the Eeprom
 *
 * @param[out]	recdata			data to read
 * @param[in]	nbrOfChar		number of bytes to read from the EEPROM
 * @param[in]	startAddress	Startaddress to read from
 * 								Range (0x0000 - 0x07FF)
 * @return		#ERROR_CODES
 *				@arg #CARME_NO_ERROR
 *				@arg #CARME_ERROR_EEPROM_OVERFLOW
 *				@arg #CARME_ERROR_EEPROM_BUSY
 *****************************************************************************
 */
ERROR_CODES CARME_EEPROM_Read(uint8_t *recdata, uint8_t nbrOfChar,
                              uint8_t startAddress) {

	if ((startAddress + nbrOfChar) > 0x07FF) {
		return CARME_ERROR_EEPROM_OVERFLOW;
	}

	if (CARME_I2C_Read(CARME_I2C_BOARD, CARME_EEPROM_I2C_ADDRESS,
	                   startAddress, 1, (uint8_t *) recdata, nbrOfChar)) {
		return CARME_ERROR_EEPROM_BUSY;
	}

	return CARME_NO_ERROR;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
