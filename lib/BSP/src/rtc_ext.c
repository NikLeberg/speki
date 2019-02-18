/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @addtogroup	CARME_RTC RTC
 * @{
 * @defgroup	CARME_RTC_EXT External RTC
 * @brief		RTC on CARME mainboard
 * @{
 *
 * @file		rtc_ext.c
 * @version		1.0
 * @date		2012-12-03
 * @author		rct1
 *
 * @brief		Communication interface to use the RTC on the CARME mainboard.
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
#include <i2c.h>					/* CARME I2C definitions				*/
#include <rtc.h>					/* CARME RTC definitions				*/
#include <rtc_ext.h>				/* CARME RTC Ext definitions			*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		Initialize the RTC on the CARME mainboard.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_RTC_Ext_Init(void) {

	static uint8_t temp;

	CARME_I2C_Init(CARME_I2C_BOARD);

	/* disable SQWE Output */
	temp = 0x00;
	CARME_I2C_Write(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_CTRL,
	                0, &temp, 1);
}

/**
 *****************************************************************************
 * @brief 		Set the Time of the external RTC on the mainboard.
 *
 * @param[in]	time	This time will be set as the new system time.\n
 *						The user has to verify that the given date is valid.
 * @return		None
 *****************************************************************************
 */
void CARME_RTC_Ext_SetTime(CARME_RTC_TIME_t *time) {

	static uint8_t temp;

	/* Write the seconds */
	temp = time->sec % 10;
	temp |= ((time->sec / 10) << 4);
	temp &= ~(1 << 7); /* Clear the CH Bit */
	CARME_I2C_Write(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_SEC, 0,
	                &temp, 1);

	/* Write the minutes */
	temp = time->min % 10;
	temp |= ((time->min / 10) << 4);
	CARME_I2C_Write(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_MIN, 0,
	                &temp, 1);

	/* Write the hour */
	temp = time->hour % 10;
	if (time->hour / 20 > 0) {
		temp |= (1 << 5);
	}
	else {
		if (time->hour / 10 > 0) {
			temp |= (1 << 4);
		}
	}
	temp &= ~(1 << 6); /* Set the hour in 24h format */
	CARME_I2C_Write(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_HOUR,
	                0, &temp, 1);

	/* Write the day of the week */
	temp = time->wday;
	CARME_I2C_Write(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_WDAY,
	                0, &temp, 1);

	/* Write the day */
	temp = time->day % 10;
	temp |= ((time->day / 10) << 4);
	CARME_I2C_Write(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_DAY, 0,
	                &temp, 1);

	/* Write the month */
	temp = time->month % 10;
	temp |= ((time->month / 10) << 4);
	CARME_I2C_Write(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_MONTH,
	                0, &temp, 1);

	/* Write the year */
	temp = time->year % 10;
	temp |= ((time->year / 10) << 4);
	CARME_I2C_Write(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_YEAR,
	                0, &temp, 1);
}

/**
 *****************************************************************************
 * @brief		Get the Time from the external RTC on mainboard.
 *
 * @param[out]	time	The current time is stored in this struct.
 * @return		None
 *****************************************************************************
 */
void CARME_RTC_Ext_GetTime(CARME_RTC_TIME_t *time) {

	static uint8_t temp;

	/* Get the year */
	CARME_I2C_Read(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_YEAR, 0,
	               &temp, 1);
	time->year = temp & 0x0F;
	time->year += ((temp & 0xF0) >> 4) * 10;

	/* Get the month */
	CARME_I2C_Read(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_MONTH,
	               0, &temp, 1);
	time->month = temp & 0x0F;
	time->month += ((temp & 0x10) >> 4) * 10;

	/* Get the day */
	CARME_I2C_Read(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_DAY, 0,
	               &temp, 1);
	time->day = temp & 0x0F;
	time->day += ((temp & 0x30) >> 4) * 10;

	/* Get the day of the week */
	CARME_I2C_Read(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_WDAY, 0,
	               &temp, 1);
	time->wday = temp & 0x07;

	/* Get the hour */
	CARME_I2C_Read(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_HOUR, 0,
	               &temp, 1);
	time->hour = temp & 0x0F;
	if (temp & 0x10) {
		time->hour += 10;
	}
	else {
		if (temp & 0x20) {
			time->hour += 20;
		}
	}

	/* Get the minute */
	CARME_I2C_Read(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_MIN, 0,
	               &temp, 1);
	time->min = temp & 0x0F;
	time->min += ((temp & 0x70) >> 4) * 10;

	/* Get the seconds */
	CARME_I2C_Read(CARME_I2C_BOARD, CARME_RTC_I2C_ADDR, CARME_RTC_REG_SEC, 0,
	               &temp, 1);
	time->sec = temp & 0x0F;
	time->sec += ((temp & 0x70) >> 4) * 10;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 * @}
 */
