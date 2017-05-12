/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_RTC RTC
 * @brief		RTC communication interface
 * @{
 *
 * @file		rtc.c
 * @version		1.0
 * @date		2012-10-24
 * @author		rct1
 *
 * @brief		Communication interface to use the RTC on the controller.
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
#include <assert.h>					/* Assertion handling					*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <i2c.h>					/* CARME I2C definitions				*/
#include <rtc_ext.h>				/* CARME RTC Ext definitions			*/
#include <rtc.h>					/* CARME RTC definitions				*/

/*----- Macros -------------------------------------------------------------*/
#define CARME_RTC_STATE_REG			RTC_BKP_DR0	/**< Status register		*/
#define CARME_RTC_STATE_INIT_OK		0x35AC		/**< RTC initialized		*/
#define CARME_RTC_STATE_TIME_OK		0xA3C5		/**< RTC set				*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief 		Initialize and start the internal RTC of the STM32F4xx.\n
 *				The date and time is set from the external RTC on the
 *				mainboard.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_RTC_Init(void) {

	RTC_InitTypeDef RTC_InitStruct;
	static CARME_RTC_TIME_t time;

	/* Initialize the external rtc */
	CARME_RTC_Ext_Init();

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2) {
		/* RTC configuration */

		/* Enable the LSE OSC */
		RCC_LSEConfig(RCC_LSE_ON);

		/* Wait till LSE is ready */
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
			;

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

		/* Enable the RTC Clock */
		RCC_RTCCLKCmd(ENABLE);

		/* Wait for RTC APB registers synchronization */
		RTC_WaitForSynchro();

		/* Configure the RTC data register and RTC prescaler */
		RTC_InitStruct.RTC_AsynchPrediv = 0x7F;
		RTC_InitStruct.RTC_SynchPrediv = 0xFF;
		RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;

		/* Check on RTC init */
		if (RTC_Init(&RTC_InitStruct) == ERROR) {
			for (;;)
				;
		}
	}

	/* set the rtc from the external rtc on mainboard */
	CARME_RTC_Ext_GetTime(&time);
	if (CARME_RTC_SetTime(&time) == CARME_NO_ERROR) {
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
	}
}

/**
 *****************************************************************************
 * @brief 		Set the internal rtc date and time.
 *
 * @param[in]	time	Date and time to write to the internal rtc.
 * @return		#ERROR_CODES
 *				@arg #CARME_NO_ERROR
 *				@arg #CARME_ERROR_RTC
 *****************************************************************************
 */
ERROR_CODES CARME_RTC_SetTime(CARME_RTC_TIME_t *time) {

	static RTC_TimeTypeDef RTC_TimeStruct;
	static RTC_DateTypeDef RTC_DateStruct;

	assert(time->year < 100 && time->year >= 0);
	assert(time->month <= 12 && time->month >= 1);
	assert(time->day <= 31 && time->day >= 1);
	assert(time->wday <= 7 && time->wday >= 1);
	assert(time->hour < 24 && time->hour >= 0);
	assert(time->min < 60 && time->min >= 0);
	assert(time->sec < 60 && time->sec >= 0);

	/* set the time */
	RTC_TimeStruct.RTC_Hours = time->hour;
	RTC_TimeStruct.RTC_Minutes = time->min;
	RTC_TimeStruct.RTC_Seconds = time->sec;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

	/* set the date */
	RTC_DateStruct.RTC_Date = time->day;
	RTC_DateStruct.RTC_Month = time->month;
	RTC_DateStruct.RTC_Year = time->year;
	RTC_DateStruct.RTC_WeekDay = time->wday;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);

	return CARME_NO_ERROR;
}

/**
 *****************************************************************************
 * @brief 		Get the date and time from the internal rtc.
 *
 * @param[out]	time	Buffer structure to save the date and time.
 * @return		None
 *****************************************************************************
 */
void CARME_RTC_GetTime(CARME_RTC_TIME_t *time) {

	static RTC_TimeTypeDef RTC_TimeStruct;
	static RTC_DateTypeDef RTC_DateStruct;

	/* get the date and time from the internal rtc */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

	time->year = RTC_DateStruct.RTC_Year;
	time->month = RTC_DateStruct.RTC_Month;
	time->day = RTC_DateStruct.RTC_Date;
	time->wday = RTC_DateStruct.RTC_WeekDay;
	time->hour = RTC_TimeStruct.RTC_Hours;
	time->min = RTC_TimeStruct.RTC_Minutes;
	time->sec = RTC_TimeStruct.RTC_Seconds;
}

#if CARME_RTC_USE_WAKEUP_ISR==1
/**
 *****************************************************************************
 * @brief 		Set a wakeup interrupt
 *
 * @param[in]	wakeup	wakeup interrupt interval
 * @return		None
 *****************************************************************************
 */
void CARME_RTC_SetWakeUpInterrupt(CARME_RTC_WAKEUP_t wakeup) {

	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	static uint32_t wakeup_time = 0x7FF;

	if (wakeup == CARME_RTC_WAKEUP_STOP) { /* disable wakeup interrupt */

		RTC_WakeUpCmd(DISABLE);
		RTC_ITConfig(RTC_IT_WUT, DISABLE); /* disable Interrupt */

		/* NVIC disable */
		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* ext Interrupt 22 disable */
		EXTI_ClearITPendingBit(EXTI_Line22 );
		EXTI_InitStructure.EXTI_Line = EXTI_Line22;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
		EXTI_Init(&EXTI_InitStructure);
	}
	else { /* enable wakeup interrupt */

		/* initialize NVIC */
		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* set EXT interrupt 22 for wakeup */
		EXTI_ClearITPendingBit(EXTI_Line22 );
		EXTI_InitStructure.EXTI_Line = EXTI_Line22;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* disable wakeup while configuring */
		RTC_WakeUpCmd(DISABLE);

		switch (wakeup) {
		case RTC_WAKEUP_30s:
			wakeup_time = 0xEFFF;
			break;
		case RTC_WAKEUP_10s:
			wakeup_time = 0x4FFF;
			break;
		case RTC_WAKEUP_5s:
			wakeup_time = 0x27FF;
			break;
		case RTC_WAKEUP_1s:
			wakeup_time = 0x7FF;
			break;
		case RTC_WAKEUP_30s:
			wakeup_time = 0x3FF;
			break;
		case RTC_WAKEUP_500ms:
			wakeup_time = 0x1FF;
			break;
		case RTC_WAKEUP_125ms:
			wakeup_time = 0xFF;
			break;
		}

		/* divider 16 => 32,768kHz/16 => 2048 Hz */
		RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16 );
		RTC_SetWakeUpCounter(wakeup_time);

		RTC_ITConfig(RTC_IT_WUT, ENABLE); /* enable interrupt */
		RTC_WakeUpCmd(ENABLE); /* enable wakeup */

	}
}
#endif

/**
 *****************************************************************************
 * @brief		Get the time from the RTC.\n
 *				This function is used from the FatFs and the function
 *				prototype is given by the FatFs module.
 *
 * @return		Bit 25 to 31:	Year from 1980 (0..127)\n
 *				Bit 21 to 24:	Month (1..12)\n
 *				Bit 16 to 20:	Day in month (1..31)\n
 *				Bit 11 to 15:	Hour (0..23)\n
 *				Bit  5 to 10:	Minute (0..59)\n
 *				Bit  1 to  4:	Second / 2 (0..29)\n
 *****************************************************************************
 */
uint32_t get_fattime(void) {

	static CARME_RTC_TIME_t time;

	CARME_RTC_GetTime(&time);

	return (((uint32_t) time.year + 20) << 25)
	        | ((uint32_t) time.month << 21)
	        | ((uint32_t) time.day << 16)
	        | ((uint32_t) time.hour << 11)
	        | ((uint32_t) time.min << 5)
	        | ((uint32_t) time.sec << 1);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
