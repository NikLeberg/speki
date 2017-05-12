/**
 *****************************************************************************
 * @defgroup	CARME CARME
 * @brief		CARME-M4 Additional functions. This library defines high level
 *				functions on base of the StdPeriph_Driver.
 * @{
 * @defgroup	CARME_Core CARME-M4 Core Module
 * @brief		CARME Core Module functions and defines.
 * @{
 *
 * @file		carme.c
 * @version		1.0
 * @date		2013-01-09
 * @author		rct1
 *
 * @todo		Activate and test Flash.
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
 * @example		dma.c
 *				Explains how to use the DMA controller.
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/

/*----- Macros -------------------------------------------------------------*/
#define CARME_ACTIVATE_EXT_FLASH	0	/**< Activate external Flash		*/

/**
 * @brief	Get the pin moder from the pin number.
 */
#define CARME_GPIO_PIN_TO_MODER(GPIO_PIN)	(									\
		((GPIO_PIN) & GPIO_Pin_0) ? GPIO_MODER_MODER0 :		/* bit 0 is set? */	\
		((GPIO_PIN) & GPIO_Pin_1) ? GPIO_MODER_MODER1 :		/* bit 1 is set? */	\
		((GPIO_PIN) & GPIO_Pin_2) ? GPIO_MODER_MODER2 :		/* bit 2 is set? */	\
		((GPIO_PIN) & GPIO_Pin_3) ? GPIO_MODER_MODER3 :		/* bit 3 is set? */	\
		((GPIO_PIN) & GPIO_Pin_4) ? GPIO_MODER_MODER4 :		/* bit 4 is set? */	\
		((GPIO_PIN) & GPIO_Pin_5) ? GPIO_MODER_MODER5 :		/* bit 5 is set? */	\
		((GPIO_PIN) & GPIO_Pin_6) ? GPIO_MODER_MODER6 :		/* bit 6 is set? */	\
		((GPIO_PIN) & GPIO_Pin_7) ? GPIO_MODER_MODER7 :		/* bit 7 is set? */	\
		((GPIO_PIN) & GPIO_Pin_8) ? GPIO_MODER_MODER8 :		/* bit 8 is set? */	\
		((GPIO_PIN) & GPIO_Pin_9) ? GPIO_MODER_MODER9 :		/* bit 9 is set? */	\
		((GPIO_PIN) & GPIO_Pin_10) ? GPIO_MODER_MODER10 :	/* bit 10 is set? */\
		((GPIO_PIN) & GPIO_Pin_11) ? GPIO_MODER_MODER11 :	/* bit 11 is set? */\
		((GPIO_PIN) & GPIO_Pin_12) ? GPIO_MODER_MODER12 :	/* bit 12 is set? */\
		((GPIO_PIN) & GPIO_Pin_13) ? GPIO_MODER_MODER13 :	/* bit 13 is set? */\
		((GPIO_PIN) & GPIO_Pin_14) ? GPIO_MODER_MODER14 :	/* bit 14 is set? */\
		((GPIO_PIN) & GPIO_Pin_15) ? GPIO_MODER_MODER15 :	/* bit 15 is set? */\
		-1)													/* no bits are set */

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
static void CARME_FSMC_Init(void);
static void CARME_FSMC_GPIO_Init(void);
static void CARME_AGPIO_Init(void);
static uint8_t CARME_AGPIO_GetMode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	CARME AGPIO Port and Pin association
 */
static CARME_Port_Pin_t CARME_AGPIO_Port_Pin[] = {
        { CARME_AGPIO_116, GPIO_Mode_IN },		/**< AGPIO 116				*/
        { CARME_AGPIO_115, GPIO_Mode_IN },		/**< AGPIO 115				*/
        { CARME_AGPIO_108, GPIO_Mode_IN },		/**< AGPIO 108				*/
        { CARME_AGPIO_105, GPIO_Mode_IN },		/**< AGPIO 105				*/
        { CARME_AGPIO_103, GPIO_Mode_IN },		/**< AGPIO 103				*/
        { CARME_AGPIO_102, GPIO_Mode_IN },		/**< AGPIO 102				*/
        { CARME_AGPIO_101, GPIO_Mode_IN },		/**< AGPIO 101				*/
        { CARME_AGPIO_99, GPIO_Mode_IN },		/**< AGPIO 99				*/
        { CARME_AGPIO_97, GPIO_Mode_IN },		/**< AGPIO 97				*/
        { CARME_AGPIO_96, GPIO_Mode_IN },		/**< AGPIO 96				*/
        { CARME_AGPIO_94, GPIO_Mode_IN },		/**< AGPIO 94				*/
        { CARME_AGPIO_93, GPIO_Mode_IN },		/**< AGPIO 93				*/
        { CARME_AGPIO_91, GPIO_Mode_IN },		/**< AGPIO 91				*/
	    { CARME_AGPIO_90, GPIO_Mode_IN },		/**< AGPIO 90				*/
        { CARME_AGPIO_22, GPIO_Mode_IN },		/**< AGPIO 22				*/
        { CARME_AGPIO_21, GPIO_Mode_IN },		/**< AGPIO 21				*/
        { CARME_AGPIO_13, GPIO_Mode_IN },		/**< AGPIO 13				*/
        { CARME_AGPIO_9, GPIO_Mode_IN },		/**< AGPIO 9				*/
        { GPIOI, GPIO_Pin_6, GPIO_Mode_OUT },	/**< LED Green				*/
        { GPIOI, GPIO_Pin_7, GPIO_Mode_OUT },	/**< LED Red				*/
};

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		CARME-M4 module initialization
 *
 * @return		None
 *****************************************************************************
 */
void CARME_Init(void) {

	/* Configure the RCC */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* DeInitialize the GPIO */
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOE);
	GPIO_DeInit(GPIOF);
	GPIO_DeInit(GPIOG);
	GPIO_DeInit(GPIOH);
	GPIO_DeInit(GPIOI);

	/* Initialize the AGPIO */
	CARME_AGPIO_Init();

	/* Initialize the FSMC */
	CARME_FSMC_GPIO_Init();
	CARME_FSMC_Init();
}

/**
 *****************************************************************************
 * @brief		Initialization of the FSMC GPIO pins. This bus is used for
 *				the connection to the extension modules, the CAN driver
 *				(SJA1000) and the LCD driver.
 *
 * @return		None
 *****************************************************************************
 */
static void CARME_FSMC_GPIO_Init(void) {

	/**
	 * @brief	CARME FSMC Port and Pin association
	 */
	/*
	 +------------------+---------------------+--------------------+------------------+-------------------+
	 +                  +                      FSMC pins assignment                                       +
	 +------------------+---------------------+--------------------+------------------+-------------------+
	 | PB7  <-> FSMC_NL | PD0  <-> FSMC_D2    | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0  <-> FSMC_A10 |
	 |                  | PD1  <-> FSMC_D3    | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1  <-> FSMC_A11 |
	 |                  | PD3  <-> FSMC_CLK   | PE2  <-> FSMC_A23  | PF2  <-> FSMC_A2 | PG2  <-> FSMC_A12 |
	 |                  | PD4  <-> FSMC_NOE   | PE3  <-> FSMC_A19  | PF3  <-> FSMC_A3 | PG3  <-> FSMC_A13 |
	 |                  | PD5  <-> FSMC_NWE   | PE4  <-> FSMC_A20  | PF4  <-> FSMC_A4 | PG4  <-> FSMC_A14 |
	 |                  | PD6  <-> FSMC_NWAIT | PE5  <-> FSMC_A21  | PF5  <-> FSMC_A5 | PG5  <-> FSMC_A15 |
	 |                  | PD7  <-> FSMC_NE1   | PE6  <-> FSMC_A22  | PF12 <-> FSMC_A6 | PG9  <-> FSMC_NE2 |
	 |                  | PD8  <-> FSMC_D13   | PE7  <-> FSMC_D4   | PF13 <-> FSMC_A7 | PG10 <-> FSMC_NE3 |
	 |                  | PD9  <-> FSMC_D14   | PE8  <-> FSMC_D5   | PF14 <-> FSMC_A8 | PG12 <-> FSMC_NE4 |
	 |                  | PD10 <-> FSMC_D15   | PE9  <-> FSMC_D6   | PF15 <-> FSMC_A9 | PG13 <-> FSMC_A24 |
	 |                  | PD11 <-> FSMC_A16   | PE10 <-> FSMC_D7   |                  | PG14 <-> FSMC_A25 |
	 |                  | PD12 <-> FSMC_A17   | PE11 <-> FSMC_D8   |------------------+-------------------+
	 |                  | PD13 <-> FSMC_A18   | PE12 <-> FSMC_D9   |
	 |                  | PD14 <-> FSMC_D0    | PE13 <-> FSMC_D10  |
	 |                  | PD15 <-> FSMC_D1    | PE14 <-> FSMC_D11  |
	 |                  |                     | PE15 <-> FSMC_D12  |
	 +------------------+---------------------+--------------------+
	 */
	CARME_Port_Pin_t CARME_FSMC_Port_Pin[] = {
	        /* Address bus */
	        { GPIOF, GPIO_Pin_0, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A0	*/
	        { GPIOF, GPIO_Pin_1, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A1	*/
	        { GPIOF, GPIO_Pin_2, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A2	*/
	        { GPIOF, GPIO_Pin_3, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A3	*/
	        { GPIOF, GPIO_Pin_4, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A4	*/
	        { GPIOF, GPIO_Pin_5, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A5	*/
	        { GPIOF, GPIO_Pin_12, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A6	*/
	        { GPIOF, GPIO_Pin_13, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A7	*/
	        { GPIOF, GPIO_Pin_14, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A8	*/
	        { GPIOF, GPIO_Pin_15, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A9	*/
	        { GPIOG, GPIO_Pin_0, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A10	*/
	        { GPIOG, GPIO_Pin_1, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A11	*/
	        { GPIOG, GPIO_Pin_2, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A12	*/
	        { GPIOG, GPIO_Pin_3, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A13	*/
	        { GPIOG, GPIO_Pin_4, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A14	*/
	        { GPIOG, GPIO_Pin_5, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A15	*/
	        { GPIOD, GPIO_Pin_11, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A16	*/
	        { GPIOD, GPIO_Pin_12, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A17	*/
	        { GPIOD, GPIO_Pin_13, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A18	*/
	        { GPIOE, GPIO_Pin_3, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A19	*/
	        { GPIOE, GPIO_Pin_4, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A20	*/
	        { GPIOE, GPIO_Pin_5, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A21	*/
	        { GPIOE, GPIO_Pin_6, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A22	*/
	        { GPIOE, GPIO_Pin_2, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A23	*/
	        { GPIOG, GPIO_Pin_13, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A24	*/
	        { GPIOG, GPIO_Pin_14, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_A25	*/
	        /* Data bus */
	        { GPIOD, GPIO_Pin_14, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D0	*/
	        { GPIOD, GPIO_Pin_15, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D1	*/
	        { GPIOD, GPIO_Pin_0, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D2	*/
	        { GPIOD, GPIO_Pin_1, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D3	*/
	        { GPIOE, GPIO_Pin_7, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D4	*/
	        { GPIOE, GPIO_Pin_8, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D5	*/
	        { GPIOE, GPIO_Pin_9, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D6	*/
	        { GPIOE, GPIO_Pin_10, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D7	*/
	        { GPIOE, GPIO_Pin_11, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D8	*/
	        { GPIOE, GPIO_Pin_12, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D9	*/
	        { GPIOE, GPIO_Pin_13, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D10	*/
	        { GPIOE, GPIO_Pin_14, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D11	*/
	        { GPIOE, GPIO_Pin_15, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D12	*/
	        { GPIOD, GPIO_Pin_8, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D13	*/
	        { GPIOD, GPIO_Pin_9, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D14	*/
	        { GPIOD, GPIO_Pin_10, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_D15	*/
	        /* Control signals */
	        { GPIOD, GPIO_Pin_7, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NE1	*/
	        { GPIOG, GPIO_Pin_9, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NE2	*/
	        { GPIOG, GPIO_Pin_10, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NE3	*/
	        { GPIOG, GPIO_Pin_12, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NE4	*/
	        { GPIOB, GPIO_Pin_7, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NL	*/
	        { GPIOD, GPIO_Pin_3, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_CLK	*/
	        { GPIOD, GPIO_Pin_4, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NOE	*/
	        { GPIOD, GPIO_Pin_5, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NWE	*/
	        { GPIOD, GPIO_Pin_6, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NWAIT	*/
	        { GPIOE, GPIO_Pin_0, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NBL0	*/
	        { GPIOE, GPIO_Pin_1, GPIO_Mode_AF, GPIO_AF_FSMC },	/**< FSMC_NBL1	*/
	};

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	CARME_GPIO_Init(CARME_FSMC_Port_Pin, &GPIO_InitStruct,
	                sizeof(CARME_FSMC_Port_Pin) / sizeof(CARME_Port_Pin_t));
}

/**
 *****************************************************************************
 * @brief		Initialization of the FSMC. This bus is used for the
 *				connection to the CarmeIO1 and CarmeIO2 module, the
 *				CAN driver (SJA1000) and the LCD driver.
 *
 * @return		None
 *****************************************************************************
 */
static void CARME_FSMC_Init(void) {

	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStruct;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStruct;

	/* Enable the FSMC interface clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/*--- FSMC Configuration -----------------------------------------------*/
	FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM1);
	FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM2);
	FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM3);
	FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM4);

	FSMC_NORSRAMInitStruct.FSMC_ReadWriteTimingStruct =
	        &FSMC_NORSRAMTimingInitStruct;
	FSMC_NORSRAMInitStruct.FSMC_WriteTimingStruct =
	        &FSMC_NORSRAMTimingInitStruct;

#if CARME_ACTIVATE_EXT_FLASH
	/**
	 * Address and databus to Flash.
	 *
	 * FSMC configuration:
	 * - Data/Address MUX = Disable
	 * - Memory Type = SRAM
	 * - Data Width = 16bit
	 * - Write Operation = Enable
	 * - Extended Mode = Enable
	 * - Asynchronous Wait = Disable
	 *
	 * FSMC Timing configuration:
	 * - Clock division = 1 -> CLK = 168 MHz
	 * - Address setup time = 5 * 6 ns
	 * - Address hold time = 0 * 6 ns
	 * - Data setup time = 9 * 6 ns
	 * - Bus turn around duration = 0 * 6 ns
	 * - Data latency time = 0 * 6 ns
	 * - Access Mode A
	 */
	FSMC_NORSRAMTimingInitStruct.FSMC_AddressSetupTime = 5;
	FSMC_NORSRAMTimingInitStruct.FSMC_AddressHoldTime = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_DataSetupTime = 9;
	FSMC_NORSRAMTimingInitStruct.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_CLKDivision = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode =
	        FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait =
	        FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity =
	        FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive =
	        FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
#endif /* CARME_ACTIVATE_EXT_FLASH */

	/**
	 * Address and databus to RAM.
	 *
	 * FSMC configuration:
	 * - Data/Address MUX = Disable
	 * - Memory Type = SRAM
	 * - Data Width = 16bit
	 * - Write Operation = Enable
	 * - Extended Mode = Enable
	 * - Asynchronous Wait = Disable
	 *
	 * FSMC Timing configuration:
	 * - Clock division = 1 -> CLK = 168 MHz
	 * - Address setup time = 5 * 6 ns
	 * - Address hold time = 0 * 6 ns
	 * - Data setup time = 9 * 6 ns
	 * - Bus turn around duration = 0 * 6 ns
	 * - Data latency time = 0 * 6 ns
	 * - Access Mode A
	 */
	FSMC_NORSRAMTimingInitStruct.FSMC_AddressSetupTime = 5;
	FSMC_NORSRAMTimingInitStruct.FSMC_AddressHoldTime = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_DataSetupTime = 9;
	FSMC_NORSRAMTimingInitStruct.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_CLKDivision = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM2;
	FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_PSRAM;
	FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode =
	        FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait =
	        FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity =
	        FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive =
	        FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);

	/**
	 * Address and databus to display controller.
	 *
	 * FSMC configuration:
	 * - Data/Address MUX = Disable
	 * - Memory Type = SRAM
	 * - Data Width = 16bit
	 * - Write Operation = Enable
	 * - Extended Mode = Enable
	 * - Asynchronous Wait = Disable
	 *
	 * FSMC Timing configuration:
	 * - Clock division = 1 -> CLK = 168 MHz
	 * - Address setup time = 5 * 6 ns
	 * - Address hold time = 0 * 6 ns
	 * - Data setup time = 9 * 6 ns
	 * - Bus turn around duration = 0 * 6 ns
	 * - Data latency time = 0 * 6 ns
	 * - Access Mode A
	 */
	FSMC_NORSRAMTimingInitStruct.FSMC_AddressSetupTime = 5;
	FSMC_NORSRAMTimingInitStruct.FSMC_AddressHoldTime = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_DataSetupTime = 9;
	FSMC_NORSRAMTimingInitStruct.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_CLKDivision = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode =
	        FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait =
	        FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity =
	        FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive =
	        FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);

	/**
	 * Address and databus to CARME IO1 and IO2 and SJA1000 CAN controller.
	 *
	 * FSMC configuration:
	 * - Data/Address MUX = Disable
	 * - Memory Type = SRAM
	 * - Data Width = 8bit
	 * - Write Operation = Enable
	 * - Extended Mode = Enable
	 * - Asynchronous Wait = Disable
	 *
	 * FSMC Timing configuration:
	 * - Clock division = 1 -> CLK = 168 MHz
	 * - Address setup time = 1 * 6 ns
	 * - Address hold time = 8 * 6 ns
	 * - Data setup time = 15 * 6 ns
	 * - Bus turn around duration = 0 * 6 ns
	 * - Data latency time = 5 * 6 ns
	 * - Access Mode D
	 */
	FSMC_NORSRAMTimingInitStruct.FSMC_AddressSetupTime = 1;
	FSMC_NORSRAMTimingInitStruct.FSMC_AddressHoldTime = 8;
	FSMC_NORSRAMTimingInitStruct.FSMC_DataSetupTime = 15;
	FSMC_NORSRAMTimingInitStruct.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_CLKDivision = 0;
	FSMC_NORSRAMTimingInitStruct.FSMC_DataLatency = 5;
	FSMC_NORSRAMTimingInitStruct.FSMC_AccessMode = FSMC_AccessMode_D;

	FSMC_NORSRAMInitStruct.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStruct.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStruct.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode =
	        FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait =
	        FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity =
	        FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStruct.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive =
	        FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStruct.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

/**
 *****************************************************************************
 * @brief		CARME AGPIO initialization
 *
 * @return		None
 *****************************************************************************
 */
static void CARME_AGPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure the GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	CARME_GPIO_Init(CARME_AGPIO_Port_Pin, &GPIO_InitStruct,
	                sizeof(CARME_AGPIO_Port_Pin) / sizeof(CARME_Port_Pin_t));
}

/**
 *****************************************************************************
 * @brief		Initialize GPIO ports with a CARME_Port_Pin_t table.
 *
 * @param[in]	pPortPinAssociation	CARME_Port_Pin_t Port and Pin association
 * @param[in]	pGPIO_InitStruct	GPIO_InitTypeDef GPIO init type definition
 * @param[in]	size				Size of the struct
 * @return		None
 *****************************************************************************
 */
void CARME_GPIO_Init(CARME_Port_Pin_t *pPortPinAssociation,
                     GPIO_InitTypeDef *pGPIO_InitStruct, uint8_t size) {

	uint8_t i;

	for (i = 0; i < size; i++) {
		pGPIO_InitStruct->GPIO_Pin = pPortPinAssociation[i].GPIO_Pin;
		pGPIO_InitStruct->GPIO_Mode = pPortPinAssociation[i].GPIO_Mode;
		GPIO_Init(pPortPinAssociation[i].GPIOx, pGPIO_InitStruct);
		if (pPortPinAssociation[i].GPIO_Mode == GPIO_Mode_AF) {
			GPIO_PinAFConfig(
			        pPortPinAssociation[i].GPIOx,
			        CARME_GPIO_PIN_TO_SOURCE(pPortPinAssociation[i].GPIO_Pin),
			        pPortPinAssociation[i].GPIO_AF);
		}
	}
}

/**
 *****************************************************************************
 * @brief		Set the AGPIO state.
 *
 * @param[in]	write	Word with the new state.
 * @param[in]	mask	Word with the mask.
 * @return		None
 *****************************************************************************
 */
void CARME_AGPIO_Set(uint32_t write, uint32_t mask) {

	uint8_t i;
	BitAction BitVal;

	for (i = 0; i < sizeof(CARME_AGPIO_Port_Pin) / sizeof(CARME_Port_Pin_t);
	        i++) {

		if (mask & (1 << i)) {

			BitVal = (write & (1 << i)) ? Bit_SET : Bit_RESET;
			GPIO_WriteBit(CARME_AGPIO_Port_Pin[i].GPIOx,
			              CARME_AGPIO_Port_Pin[i].GPIO_Pin, BitVal);
		}
	}
}

/**
 *****************************************************************************
 * @brief		Get the AGPIO state.
 *
 * @param[out]	pStatus	Word with the actual state.
 * @return		None
 *****************************************************************************
 */
void CARME_AGPIO_Get(uint32_t *pStatus) {

	uint8_t i;
	uint32_t BitStatus = 0;

	for (i = 0; i < sizeof(CARME_AGPIO_Port_Pin) / sizeof(CARME_Port_Pin_t);
	        i++) {

		if (CARME_AGPIO_GetMode(CARME_AGPIO_Port_Pin[i].GPIOx,
		                        CARME_AGPIO_Port_Pin[i].GPIO_Pin)
		        == GPIO_Mode_IN) {

			if (GPIO_ReadInputDataBit(CARME_AGPIO_Port_Pin[i].GPIOx,
			                          CARME_AGPIO_Port_Pin[i].GPIO_Pin)
			        != Bit_RESET) {

				BitStatus |= (1 << i);
			}
		}
		else if (CARME_AGPIO_GetMode(CARME_AGPIO_Port_Pin[i].GPIOx,
		                             CARME_AGPIO_Port_Pin[i].GPIO_Pin)
		        == GPIO_Mode_OUT) {

			if (GPIO_ReadOutputDataBit(CARME_AGPIO_Port_Pin[i].GPIOx,
			                           CARME_AGPIO_Port_Pin[i].GPIO_Pin)
			        != Bit_RESET) {

				BitStatus |= (1 << i);
			}
		}
	}
	*pStatus = BitStatus;
}

/**
 *****************************************************************************
 * @brief		Get the AGPIO mode.
 *
 * @param[in]	GPIOx		Where x can be (A..I) to select the GPIO
 *							peripheral.
 * @param[in]	GPIO_Pin	specifies the port bit to be read.
 * @return		0 = GPIO_Mode_IN\n
 *				1 = GPIO_Mode_OUT\n
 *				2 = GPIO_Mode_AF\n
 *				3 = GPIO_Mode_AN
 *****************************************************************************
 */
static uint8_t CARME_AGPIO_GetMode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {

	uint32_t mask;

	mask = GPIOx->MODER & CARME_GPIO_PIN_TO_MODER(GPIO_Pin);
	return mask >> 2 * CARME_GPIO_PIN_TO_SOURCE(GPIO_Pin);
}

/**
 *****************************************************************************
 * @brief		Set the green LED on the CARME Module.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_LED_Green_Set(void) {

	CARME_AGPIO_Set(CARME_LED_PIN_GREEN, CARME_LED_PIN_GREEN);
}

/**
 *****************************************************************************
 * @brief		Reset the green LED on the CARME Module.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_LED_Green_Reset(void) {

	CARME_AGPIO_Set(0, CARME_LED_PIN_GREEN);
}

/**
 *****************************************************************************
 * @brief		Set the red LED on the CARME Module.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_LED_Red_Set(void) {

	CARME_AGPIO_Set(CARME_LED_PIN_RED, CARME_LED_PIN_RED);
}

/**
 *****************************************************************************
 * @brief		Reset the red LED on the CARME Module.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_LED_Red_Reset(void) {

	CARME_AGPIO_Set(0, CARME_LED_PIN_RED);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
