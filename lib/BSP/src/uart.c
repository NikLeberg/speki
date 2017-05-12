/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @addtogroup	USART
 * @{
 *
 * @file		uart.c
 * @version		1.0
 * @date		2013-01-10
 * @author		rct1
 *
 * @brief		The CARME UART Module provides a function to initialize the
 *				GPIOs for the CARME UART ports.
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
 * @example		usart.c
 *				Explains how to send a string over the stdout to the CARME
 *				UART0.
 * @example		usart_interrupt.c
 *				This example shows how to receive bytes over the USART with interrupts.
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <uart.h>					/* CARME BSP UART port					*/

/*----- Macros -------------------------------------------------------------*/
#define RCC_APB2Periph_CARME_UART0	RCC_APB2Periph_USART1		/**< RCC for the CARME UART0	*/
#define RCC_APB1Periph_CARME_UART1	RCC_APB1Periph_USART3		/**< RCC for the CARME UART1	*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	UART Port and Pin association
 */
static CARME_Port_Pin_t CARME_UART_Port_Pin[] = {
        /* UART0 (FF) */
        { GPIOA, GPIO_Pin_9, GPIO_Mode_AF, GPIO_AF_USART1 },	/**< TxD, Transmitted data		*/
        { GPIOA, GPIO_Pin_10, GPIO_Mode_AF, GPIO_AF_USART1 },	/**< RxD, Received data			*/
        { GPIOA, GPIO_Pin_11, GPIO_Mode_AF, GPIO_AF_USART1 },	/**< CTS, Clear to send			*/
        { GPIOA, GPIO_Pin_12, GPIO_Mode_AF, GPIO_AF_USART1 },	/**< RTS, Request to send		*/
        { GPIOH, GPIO_Pin_13, GPIO_Mode_OUT },					/**< DTR, Data terminal ready	*/
        { GPIOH, GPIO_Pin_14, GPIO_Mode_IN },					/**< DSR, Data set ready		*/
        { GPIOH, GPIO_Pin_15, GPIO_Mode_IN },					/**< DCD, Carrier detect		*/
        /* UART1 (BT) */
        { GPIOB, GPIO_Pin_10, GPIO_Mode_AF, GPIO_AF_USART3 },	/**< TxD, Transmitted data		*/
        { GPIOB, GPIO_Pin_11, GPIO_Mode_AF, GPIO_AF_USART3 },	/**< RxD, Received data			*/
        { GPIOH, GPIO_Pin_9, GPIO_Mode_IN },					/**< CTS, Clear to send			*/
        { GPIOH, GPIO_Pin_6, GPIO_Mode_OUT }					/**< RTS, Request to send		*/
};

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		UART initialization
 *
 * @param[in]	UARTx	The UART port which will be used. Possible values are
 *						#CARME_UART0 and #CARME_UART1.
 * @param[in]	pUSART_InitStruct	Pointer to the USART init structure.
 * @return		None
 *****************************************************************************
 */
void CARME_UART_Init(USART_TypeDef *UARTx,
                     USART_InitTypeDef *pUSART_InitStruct) {

	USART_ClockInitTypeDef USART_ClockInitStruct;
	uint8_t i = 0xFF;

	CARME_UART_GPIO_Init();		/* Initialize the CARME UART Port GPIOs		*/

	if (UARTx == CARME_UART0) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_CARME_UART0, ENABLE);
	}
	else {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CARME_UART1, ENABLE);
	}
	USART_DeInit(UARTx);

	/* Initialize the USART clock */
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(UARTx, &USART_ClockInitStruct);

	/* Delete all content on the input buffer */
	while (USART_GetFlagStatus(UARTx, USART_FLAG_RXNE ) == SET) {
		USART_ReceiveData(UARTx);
	}

	USART_Cmd(UARTx, DISABLE);
	USART_Init(UARTx, pUSART_InitStruct);
	USART_Cmd(UARTx, ENABLE);

	while (i--) {
		/* Wait until hardware is running */
	}
}

/**
 *****************************************************************************
 * @brief		CARME UART port GPIO initialization.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_UART_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure the GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	CARME_GPIO_Init(CARME_UART_Port_Pin, &GPIO_InitStruct,
	                sizeof(CARME_UART_Port_Pin) / sizeof(CARME_Port_Pin_t));
}

/**
 *****************************************************************************
 * @brief		Send a char over the UART port.
 *
 * @param[in]	UARTx	The UART port you want to use. Possible values are
 *						#CARME_UART0 and #CARME_UART1.
 * @param[in]	c		Character you want to send.
 * @return		None
 *****************************************************************************
 */
void CARME_UART_SendChar(USART_TypeDef *UARTx, char c) {

	while (USART_GetFlagStatus(UARTx, USART_FLAG_TC) == RESET) {
	}
	USART_SendData(UARTx, c);
}

/**
 *****************************************************************************
 * @brief		Send a string over the UART port. You can use printf() to send
 * 				a formated String over the #CARME_UART0
 *
 * @param[in]	UARTx	The UART port you want to use. Possible values are
 *						#CARME_UART0 and #CARME_UART1.
 * @param[in]	pStr	Pointer to the string you want to send.
 * @return		None
 *****************************************************************************
 */
void CARME_UART_SendString(USART_TypeDef *UARTx, char *pStr) {

	while (*pStr) {
		CARME_UART_SendChar(UARTx, *pStr);
		pStr++;
	}
}

/**
 *****************************************************************************
 * @brief		Receive a char from the UART input buffer.
 *
 * @param[in]	UARTx	The UART port you want to use. Possible values are
 *						#CARME_UART0 and #CARME_UART1.
 * @param[out]	pc		Pointer to unsigned char to write data.
 * @return		 0 = #CARME_NO_ERROR\n
 *				30 = #CARME_ERROR_UART_NO_DATA
 *****************************************************************************
 */
ERROR_CODES CARME_UART_ReceiveChar(USART_TypeDef *UARTx, char *pc) {

	uint16_t i;
	ERROR_CODES res = CARME_ERROR_UART_NO_DATA;

	if (USART_GetFlagStatus(UARTx, USART_FLAG_RXNE ) == SET) {
		i = USART_ReceiveData(UARTx);
		if (i <= 0xFF) {
			*pc = (uint8_t) (i & 0xFF);
			res = CARME_NO_ERROR;
		}
	}
	return res;
}

/**
 *****************************************************************************
 * @brief		Receive a string from the UART input buffer until new line or
 *				carriage return or there is no more data.
 *
 * @param[in]	UARTx	The UART port you want to use. Possible values are
 *						#CARME_UART0 and #CARME_UART1.
 * @param[out]	pc		Pointer to the start of the string
 * @param[in]	count	Maximal length of the string
 * @return		 0 = #CARME_NO_ERROR\n
 *				30 = #CARME_ERROR_UART_NO_DATA
 *****************************************************************************
 */
ERROR_CODES CARME_UART_ReceiveString(USART_TypeDef *UARTx, char *pc,
                                     uint8_t count) {

	ERROR_CODES res = 0;
	uint8_t i;

	for (i = 0; i < count; i++) {

		res = CARME_UART_ReceiveChar(UARTx, pc);
		if ((*pc == '\r') || (*pc == '\n')
				|| (res == CARME_ERROR_UART_NO_DATA)) {
			break;
		}
		pc++;
	}
	return res;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
