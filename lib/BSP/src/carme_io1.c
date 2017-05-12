/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_IO1 CARME IO1
 * @brief		CARME IO1 high level functions
 * @{
 *
 * @file		carme_io1.c
 * @version		1.0
 * @date		2012-12-19
 * @author		rct1
 *
 * @brief		CARME IO1 extension module board support package.
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
 * @example		gpio_bsp.c
 *				This example shows how to create an output gpio pin with the
 *				carme board support package.
 * @example		gpio_stdperiph.c
 *				This example shows how to create an output gpio pin only with
 *				the STMicroelectonics board support package.
 * @example		gpio_asm.s
 *				This example shows how to create an output gpio pin in
 *				assembler.
 * @example		taster_interrupt.s
 *				This example shows how to use Button0 from CARME-IO1 with
 *				interrupts.
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <carme_io1.h>				/* CARME IO1 Module						*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	Pointer to the register which controls the CARME IO1 LEDs.
 */
static __IO uint8_t *LED_Port = (__IO uint8_t *) (FSMC_CARME_EXTENSION1_BASE + 0x200);

/**
 * @brief	Pointer to the register which controls the CARME IO1 Switchs.
 */
volatile static __IO uint8_t *Switch_Port = (__IO uint8_t *) (FSMC_CARME_EXTENSION1_BASE + 0x400);

/**
 * @brief	Pointer to the register which controls the CARME IO1 PIO PortA Value.
 */
static __IO uint8_t *PIO_PortA = (__IO uint8_t *) (FSMC_CARME_EXTENSION1_BASE + 0x800);

/**
 * @brief	Pointer to the register which controls the CARME IO1 PIO PortB Value.
 */
//static __IO uint8_t *PIO_PortB = (__IO uint8_t *) (FSMC_CARME_EXTENSION1_BASE + 0x808);

/**
 * @brief	Pointer to the register which controls the CARME IO1 PIO PortC Value.
 */
static __IO uint8_t *PIO_PortC = (__IO uint8_t *) (FSMC_CARME_EXTENSION1_BASE + 0x810);

/**
 * @brief	Pointer to the register which controls the CARME IO1 PIO Control Value.
 */
static __IO uint8_t *PIO_Control = (__IO uint8_t *) (FSMC_CARME_EXTENSION1_BASE + 0x818);

/**
 * @brief	CARME IO1 Port and Pin association
 */
static CARME_Port_Pin_t CARME_IO1_Port_Pin[] = {
        { CARME_IO1_BUTTON0_PORT, CARME_IO1_BUTTON0_PIN, GPIO_Mode_IN }, /**< Button 0	*/
        { CARME_IO1_BUTTON1_PORT, CARME_IO1_BUTTON1_PIN, GPIO_Mode_IN }, /**< Button 1	*/
        { CARME_IO1_BUTTON2_PORT, CARME_IO1_BUTTON2_PIN, GPIO_Mode_IN }, /**< Button 2	*/
        { CARME_IO1_BUTTON3_PORT, CARME_IO1_BUTTON3_PIN, GPIO_Mode_IN }, /**< Button 3	*/
};

/**
 * @brief	CARME IO1 current led value
 */
static __IO uint8_t LED_Value = 0x00;

/**
 * @brief	CARME IO1 current PortA value
 */
static __IO uint8_t PortA_Value = 0x00;

/**
 * @brief	CARME IO1 current PortC value
 */
static __IO uint8_t PortC_Value = 0x00;

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		CARME IO1 initialization.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* Initialize the GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	CARME_GPIO_Init(CARME_IO1_Port_Pin, &GPIO_InitStruct,
	                sizeof(CARME_IO1_Port_Pin) / sizeof(CARME_Port_Pin_t));
}

/**
 *****************************************************************************
 * @brief		Set a byte to the CARME IO1 LEDs.
 *
 * @param[in]	write	Byte with the new LED status.
 * @param[in]	mask	Mask which LED should be written.
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_LED_Set(uint8_t write, uint8_t mask) {

	uint8_t i;

	for (i = 0; i < 8; i++) {

		if (mask & (1 << i)) {

			if (write & (1 << i)) {
				LED_Value |= (1 << i);
			}
			else {
				LED_Value &= ~(1 << i);
			}
		}
	}
	*LED_Port = LED_Value;
}

/**
 *****************************************************************************
 * @brief		Get the CARME IO1 LED status.
 *
 * @param[out]	pStatus	The current LED status.
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_LED_Get(uint8_t *pStatus) {

	*pStatus = LED_Value;
}

/**
 *****************************************************************************
 * @brief		Get the CARME IO1 switch status.
 *
 * @param[out]	pStatus	The current switch status.
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_SWITCH_Get(uint8_t *pStatus) {

	*pStatus = *Switch_Port;
}

/**
 *****************************************************************************
 * @brief		Get the CARME IO1 button status.
 *
 * @param[out]	pStatus	The current button status.
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_BUTTON_Get(uint8_t *pStatus) {

	uint32_t agpio_state;
	uint8_t button_state = 0;

	CARME_AGPIO_Get(&agpio_state);
	button_state |= (agpio_state & CARME_AGPIO_PIN_99) ? 0x1 : 0;	// T0
	button_state |= (agpio_state & CARME_AGPIO_PIN_101) ? 0x2 : 0;	// T1
	button_state |= (agpio_state & CARME_AGPIO_PIN_102) ? 0x4 : 0;	// T2
	button_state |= (agpio_state & CARME_AGPIO_PIN_103) ? 0x8 : 0;	// T3
	*pStatus = button_state;
}

/**
 *****************************************************************************
 * @brief		Set the CARME IO1 interrupt enable/disable.
 *
 * @param[in]	state	The new state.
 * @return		None
 *****************************************************************************
 */
extern void CARME_IO1_BUTTON_Interrupt(FunctionalState state) {

	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	if (state == ENABLE) {
		/* Enable the external interrupt */
		SYSCFG_EXTILineConfig(
				CARME_GPIO_TO_EXTIPORTSOURCE(CARME_IO1_BUTTON0_PORT),
				CARME_GPIO_TO_EXTIPINSOURCE(CARME_IO1_BUTTON0_PIN));
		SYSCFG_EXTILineConfig(
				CARME_GPIO_TO_EXTIPORTSOURCE(CARME_IO1_BUTTON1_PORT),
				CARME_GPIO_TO_EXTIPINSOURCE(CARME_IO1_BUTTON1_PIN));
		SYSCFG_EXTILineConfig(
				CARME_GPIO_TO_EXTIPORTSOURCE(CARME_IO1_BUTTON2_PORT),
				CARME_GPIO_TO_EXTIPINSOURCE(CARME_IO1_BUTTON2_PIN));
		SYSCFG_EXTILineConfig(
				CARME_GPIO_TO_EXTIPORTSOURCE(CARME_IO1_BUTTON3_PORT),
				CARME_GPIO_TO_EXTIPINSOURCE(CARME_IO1_BUTTON3_PIN));

		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON0_PIN);
		EXTI_Init(&EXTI_InitStruct);
		EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON1_PIN);
		EXTI_Init(&EXTI_InitStruct);
		EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON2_PIN);
		EXTI_Init(&EXTI_InitStruct);
		EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON3_PIN);
		EXTI_Init(&EXTI_InitStruct);

		/* Enable and set EXTI Line8 Interrupt to the lowest priority */
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_Init(&NVIC_InitStruct);
		NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_Init(&NVIC_InitStruct);
		NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
		NVIC_Init(&NVIC_InitStruct);
	}
	else {
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStruct.EXTI_LineCmd = DISABLE;
		EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON0_PIN);
		EXTI_Init(&EXTI_InitStruct);
		EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON1_PIN);
		EXTI_Init(&EXTI_InitStruct);
		EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON2_PIN);
		EXTI_Init(&EXTI_InitStruct);
		EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON3_PIN);
		EXTI_Init(&EXTI_InitStruct);
	}
}

/**
 *****************************************************************************
 * @brief		Sets the PIOs in order with the input-value.
 *
 * @param[in]	port	Port (A or C)
 * @param[in]	data	input value to set
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_PIO_Write(CARME_IO1_PIO_PORT port, uint8_t data) {

	if (port == CARME_IO1_PORT_A) {
		PortA_Value = data;
		*PIO_PortA = PortA_Value;
	}
	if (port == CARME_IO1_PORT_C) {
		PortC_Value = data;
		*PIO_PortC = PortC_Value;
	}
	if (port == CARME_IO1_PORT_C_L) {
		PortC_Value &= 0xF0;
		PortC_Value |= (0xF & data);
		*PIO_PortC = PortC_Value;
	}
	if (port == CARME_IO1_PORT_C_H) {
		PortC_Value &= 0xF;
		PortC_Value |= ((0xF & data) << 4);
		*PIO_PortC = PortC_Value;
	}
}

/**
 *****************************************************************************
 * @brief		Read the PIO value.
 *
 * @param[in]	port	Port (A or C)
 * @param[out]	data	Pointer to the user pio variable
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_PIO_Read(CARME_IO1_PIO_PORT port, uint8_t *data) {

	uint8_t tmp;

	if (port == CARME_IO1_PORT_A) {
		*data = *PIO_PortA;
	}
	if (port == CARME_IO1_PORT_C) {
		*data = *PIO_PortC;
	}
	if (port == CARME_IO1_PORT_C_L) {
		tmp = *PIO_PortC;
		*data = (tmp & 0xF);
	}
	if (port == CARME_IO1_PORT_C_H) {
		tmp = *PIO_PortC;
		*data = ((tmp & 0xF0) >> 4);
	}
}

/**
 *****************************************************************************
 * @brief		Sets the PIO on the position of the input value.\n
 *				For example the input value 0x4 sets the pin number 4.
 *				The other pins are not affected.
 *
 * @param[in]	port	Port (A or C)
 * @param[out]	pio		PIO to set [0..7]
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_PIO_Set(CARME_IO1_PIO_PORT port, uint8_t pio) {

	switch (port) {
		case CARME_IO1_PORT_A:
			PortA_Value |= (1 << pio);
			*PIO_PortA = PortA_Value;
			break;
		case CARME_IO1_PORT_C_L:
		case CARME_IO1_PORT_C_H:
		case CARME_IO1_PORT_C:
			PortC_Value |= (1 << pio);
			*PIO_PortC = PortC_Value;
			break;
	}
}

/**
 *****************************************************************************
 * @brief		Clears the PIO on the position of the input value.\n
 *				For example the input value 0x4 clears the pin number 4.
 *				The other pins are not affected.
 *
 * @param[in]	port	Port (A or C)
 * @param[out]	pio		PIO to clear [0..7]
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_PIO_Clear(CARME_IO1_PIO_PORT port, uint8_t pio) {

	switch (port) {
		case CARME_IO1_PORT_A:
			PortA_Value &= ~(1 << pio);
			*PIO_PortA = PortA_Value;
			break;
		case CARME_IO1_PORT_C_L:
		case CARME_IO1_PORT_C_H:
		case CARME_IO1_PORT_C:
			PortC_Value &= ~(1 << pio);
			*PIO_PortC = PortC_Value;
			break;
	}
}

/**
 *****************************************************************************
 * @brief		Sets the PIO Control Register.
 *				This function must be called to change the Port direction.
 * @note		This function should not be interrupted from a function who
 *				access the address- and databus.
 *
 * @param[in]	cmd		See CARME_IO1_PIO_CONTROL for allowed values.
 * @return		None
 *****************************************************************************
 */
void CARME_IO1_PIO_Control(CARME_IO1_PIO_CONTROL cmd) {

	uint8_t PIO_Controlword = 0x80;

	if (cmd & CARME_IO1_PORT_A_IN) {
		PIO_Controlword |= 0x10;
	}
	if (cmd & CARME_IO1_PORT_A_OUT) {
		PIO_Controlword &= ~0x10;
	}
	if (cmd & CARME_IO1_PORT_C_IN) {
		PIO_Controlword |= 0x09;
	}
	if (cmd & CARME_IO1_PORT_C_OUT) {
		PIO_Controlword &= ~0x09;
	}
	if (cmd & CARME_IO1_PORT_C_L_IN) {
		PIO_Controlword |= 0x01;
	}
	if (cmd & CARME_IO1_PORT_C_L_OUT) {
		PIO_Controlword &= ~0x01;
	}
	if (cmd & CARME_IO1_PORT_C_H_IN) {
		PIO_Controlword |= 0x08;
	}
	if (cmd & CARME_IO1_PORT_C_H_OUT) {
		PIO_Controlword &= ~0x08;
	}

	*PIO_Control = PIO_Controlword;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
