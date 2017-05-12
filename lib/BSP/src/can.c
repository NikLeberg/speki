/**
 *****************************************************************************
 * @addtogroup 	CARME
 * @{
 * @defgroup	CARME_CAN_EXT CAN SJA1000 Controller
 * @brief		CARME-M4 CAN Interface
 * @{
 *
 * @file		can.c
 * @version		1.0
 * @date		2007-04-13
 * @author		M. Muehlemann
 *
 * @brief		Drivers for the CAN interface. Uses the SJA1000 CAN controller
 *				on the CARME motherboard.
 *
 * @todo		Bus off (write 255 to TXERR)
 * @todo		Sleep Mode
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
 * @example		can.c
 *				Explains how to communicate over the CAN bus.
 * @example		can_acceptancefilter.c
 *				Explains how to set a acceptance filter on the CAN transceiver.
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <string.h>					/* Memory and string functions			*/
#include <stdio.h>					/* Standard input and output			*/

#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <can_sja1000.h>			/* CAN Controller SJA1000 defines		*/
#include <can.h>					/* CARME CAN Module						*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
static void CARME_CAN_InitSTM(void);
static void CARME_CAN_SetBusTiming(uint8_t btr0, uint8_t btr1);

/*----- Data ---------------------------------------------------------------*/
static IRQ_CALLBACK CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_COUNT];

/*----- Implementation -----------------------------------------------------*/

/**
 *****************************************************************************
 * @brief		Initialize the stm32f4xx GPIOs and the nCAN_INT.\n
 *				The CAN interrupt is on CARME_CAN_nCAN_IRQn_CH with
 *				NVIC_IRQChannelPreemptionPriority = 0x0F and
 *				NVIC_IRQChannelSubPriority = 0x0F.
 *
 * @return		None
 *****************************************************************************
 */
static void CARME_CAN_InitSTM(void) {

	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Initialize the GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = CARME_CAN_nCAN_INT_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(CARME_CAN_nCAN_INT_PORT, &GPIO_InitStruct);

	/* Enable the external interrupt */
	SYSCFG_EXTILineConfig(CARME_GPIO_TO_EXTIPORTSOURCE(CARME_CAN_nCAN_INT_PORT),
			CARME_GPIO_TO_EXTIPINSOURCE(CARME_CAN_nCAN_INT_PIN));
	EXTI_InitStruct.EXTI_Line = CARME_GPIO_TO_EXTILINE(CARME_CAN_nCAN_INT_PIN);
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	/* Enable and set EXTI Line8 Interrupt to the lowest priority */
	NVIC_InitStruct.NVIC_IRQChannel = CARME_CAN_nCAN_IRQn_CH;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/**
 *****************************************************************************
 * @brief		Initialize GPIOs and the CAN-Controller.
 *
 * Initializes the global variables from the driver, the baudrate and
 * the acceptance-filter. This initialization function configures the
 * CAN-Controller for communication without interrupts.
 *
 * @param[in]	baud	Baud-rate. Refer CARME_CAN_setBaudrate() for valid
 * 						arguments.
 * @param[in]	flags	Driver Flags. The Flags can be OR-ed.
 * 						@arg CARME_CAN_DF_RESET Leaves the CAN Controller in
 * 						Reset-Mode.
 * 						@arg CARME_CAN_DF_NORMAL Put the CAN-Controller
 * 						in Operating-Mode after initialization.
 * 						@arg CARME_CAN_DF_LISTEN_ONLY Put the CAN-Controller
 * 						in Listen Only Mode after initialization.
 * @return		None
 *****************************************************************************
 */
void CARME_CAN_Init(uint32_t baud, uint8_t flags) {

	uint8_t i;

	/* initialize global variables */
	memset(CARME_CAN_IRQCallbacks, 0, sizeof(CARME_CAN_IRQCallbacks));

	/* initialize stm32f4xx registers */
	CARME_CAN_InitSTM();

	/* bypass RX comparator and set PeliCAN mode */
	CARME_CAN_Write_Register(SJA1000_CDR,
	                         SJA1000_CDR_CBP | SJA1000_CDR_CANMODE);

	/* output configuration, TX1: float, TX0: pushpull */
	CARME_CAN_Write_Register(SJA1000_OCR, 0x1a);

	/* set Baudrate */
	CARME_CAN_SetBaudrate(baud);

	/* configure acceptance filter to accept all CAN-Messages */
	for (i = 0; i < 4; i++) {
		CARME_CAN_Write_Register(SJA1000_ACR(i), 0x00); /* acceptance code	*/
		CARME_CAN_Write_Register(SJA1000_AMR(i), 0xff); /* acceptance mask	*/
	}

	CARME_CAN_Write_Register(SJA1000_IR, 0x00);	/* clear all interrupts		*/

	/* set the operation mode */
	i = 1;
	if (flags & CARME_CAN_DF_NORMAL) {
		i &= ~SJA1000_MOD_RM;			/* clear RM bit -> Normal Mode		*/
	}
	if (flags & CARME_CAN_DF_LISTEN_ONLY) {
		i |= SJA1000_MOD_LOM;			/* set LOM bit -> Listen only mode	*/
	}
	CARME_CAN_Write_Register(SJA1000_MOD, i);	/* switch in operating mode	*/
}

/**
 *****************************************************************************
 * @brief		Initialize GPIOs and the CAN-Controller.
 *
 * Initializes the global variables from the driver, the baudrate and
 * the acceptance-filter. This initialization function configures the
 * CAN-Controller for communication without interrupts.
 *
 * @param[in]	baud		Baud-rate. Refer CARME_CAN_setBaudrate() for valid
 * 							arguments.
 * @param[in]	flags		Driver Flags. The Flags can be OR-ed.
 * 							@arg CARME_CAN_DF_RESET Leaves the CAN Controller
 * 							in Reset-Mode.
 * 							@arg CARME_CAN_DF_NORMAL Put the CAN-Controller
 * 							in Operating-Mode after initialization.
 * 							@arg CARME_CAN_DF_LISTEN_ONLY Put the CAN-
 * 							Controller in Listen Only Mode after
 *							initialization.
 * @param[in]	interrupts	Enable specified Interrupts.
 *							The Flags can be OR-ed.
 *							@arg CARME_CAN_INT_BUSERR		Bus Error
 *							@arg CARME_CAN_INT_ARBIT_LOST	Arbitration Lost
 *							@arg CARME_CAN_INT_PASSIVE		Error Passive
 *							@arg CARME_CAN_INT_WAKEUP		Wake-up
 *							@arg CARME_CAN_INT_OVERRUN		Data overrun
 *							@arg CARME_CAN_INT_ERROR		Error Warning
 *							@arg CARME_CAN_INT_TX			Transmit
 *							@arg CARME_CAN_INT_RX			Receive
 * @return		None
 *****************************************************************************
 */
void CARME_CAN_InitI(uint32_t baud, uint8_t flags, uint32_t interrupts) {

	/* initialize normal settings, hold controller in Reset-Mode */
	CARME_CAN_Init(baud, CARME_CAN_DF_RESET);

	/* enable interrupts */
	CARME_CAN_Write_Register(SJA1000_IER, interrupts);

	CARME_CAN_SetMode(flags);
}

/**
 *****************************************************************************
 * @brief		Registers a callback function. Callback functions are called
 *				in Interruptmode. Use this function after CARME_CAN_init() or
 *				CARME_CAN_initI().
 *
 * @param[in]	id				id of the Callback function
 * @param[in]	pIRQCallback	pointer to the callback function
 * @return		None
 *****************************************************************************
 */
void CARME_CAN_RegisterIRQCallback(enum CARME_CAN_IRQ_CALLBACKS id,
                                   IRQ_CALLBACK pIRQCallback) {

	CARME_CAN_IRQCallbacks[id] = pIRQCallback;
}

/**
 *****************************************************************************
 * @brief		Unregisters a callback function. Callback functions are called
 *				in Interruptmode.
 *
 * @param[in]	id		id of the Callback function
 * @return		None
 *****************************************************************************
 */
void CARME_CAN_UnregisterIRQCallback(enum CARME_CAN_IRQ_CALLBACKS id) {

	CARME_CAN_IRQCallbacks[id] = NULL;
}

/**
 *****************************************************************************
 * @brief		Change operating mode from the CAN-Controller.
 *
 * This function is used to switch the controller in reset or normal mode
 * for change settings like acceptance-filter, baud rate, bus timing,...
 *
 * @param[in]	flags	Can be one of the following constants:
 * 						@arg #CARME_CAN_DF_NORMAL Switch in normal operating mode
 * 						@arg #CARME_CAN_DF_RESET	 Switch in reset mode
 * @return		 0 = #CARME_NO_ERROR\n
 * 				43 = #CARME_ERROR_CAN_INVALID_MODE
 *****************************************************************************
 */
ERROR_CODES CARME_CAN_SetMode(uint8_t flags) {

	uint8_t mod = CARME_CAN_Read_Register(SJA1000_MOD);

	switch (flags) {
	case CARME_CAN_DF_NORMAL:
		mod &= ~SJA1000_MOD_RM;
		break;
	case CARME_CAN_DF_RESET:
		mod |= SJA1000_MOD_RM;
		break;
	default:
		return CARME_ERROR_CAN_INVALID_MODE;
	}
	CARME_CAN_Write_Register(SJA1000_MOD, mod);	/* switch mode */

	return CARME_NO_ERROR;
}

/**
 *****************************************************************************
 * @brief		Reads a CAN-Message from the Receive-Buffer. If no message is
 *				in the buffer, the function returns
 *				CARME_ERROR_CAN_ERROR_RXFIFO_EMPTY.
 *
 * @param[in]	rxMsg	Pointer to a CAN_MESSAGE struct for the received
 * 						message.
 * @return		 0 = #CARME_NO_ERROR\n
 * 				42 = #CARME_ERROR_CAN_RXFIFO_EMPTY\n
 * 				46 = #CARME_ERROR_CAN_ERROR_STATUS
 *****************************************************************************
 */
ERROR_CODES CARME_CAN_Read(CARME_CAN_MESSAGE* rxMsg) {

	ERROR_CODES err = CARME_NO_ERROR;
	uint8_t sr = CARME_CAN_Read_Register(SJA1000_SR);

	if (sr & SJA1000_SR_ES) {
		err = CARME_ERROR_CAN_ERROR_STATUS;
	}

	if ((sr & SJA1000_SR_RBS) == 0) {
		err = CARME_ERROR_CAN_RXFIFO_EMPTY;
	}

	/* read first Frame Format Information */
	uint8_t ff = CARME_CAN_Read_Register(SJA1000_RX_BUF(0));

	/* Read Frame format */
	rxMsg->ext = (ff & SJA1000_FRAMEINFO_FF) != 0;

	/* read RTR */
	rxMsg->rtr = (ff & SJA1000_FRAMEINFO_RTR) != 0;

	/* read number of data bytes */
	rxMsg->dlc = ff & 0x0f;
	if (rxMsg->dlc > 8)
		rxMsg->dlc = 8;

	/* read identifier */
	if (rxMsg->ext) {
		rxMsg->id = CARME_CAN_Read_Register(SJA1000_RX_BUF(1)) << 21;
		rxMsg->id |= CARME_CAN_Read_Register(SJA1000_RX_BUF(2)) << 13;
		rxMsg->id |= CARME_CAN_Read_Register(SJA1000_RX_BUF(3)) << 5;
		rxMsg->id |= CARME_CAN_Read_Register(SJA1000_RX_BUF(4)) >> 3;
	}
	else {
		rxMsg->id = CARME_CAN_Read_Register(SJA1000_RX_BUF(1)) << 3;
		rxMsg->id |= CARME_CAN_Read_Register(SJA1000_RX_BUF(2)) >> 5;
	}

	/* read data bytes */
	int rxDataOffset;
	if (rxMsg->ext) {
		rxDataOffset = SJA1000_RX_BUF(5);
	}
	else {
		rxDataOffset = SJA1000_RX_BUF(3);
	}
	int i;
	for (i = 0; i < rxMsg->dlc; i++) {
		rxMsg->data[i] = CARME_CAN_Read_Register(rxDataOffset + i);
	}

	/* release buffer */
	CARME_CAN_Write_Register(SJA1000_CMR, SJA1000_CMR_RRB);
	return err;
}

/**
 *****************************************************************************
 * @brief		This function will be called from the Interrupt-Handler
 *				on a CAN-Interrupt.\n
 *				If it was not the EXTI Line from the CAN controller, then the
 *				functions MyEXTI9_5_IRQHandler is called.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_CAN_Interrupt_Handler(void) {

	uint8_t ir = CARME_CAN_Read_Register(SJA1000_IR);
	do {

		if (ir & SJA1000_IR_RI) {
			if (CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_RX_INTERRUPT]) {
				CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_RX_INTERRUPT]();
			}
		} else if (ir & SJA1000_IR_TI) {
			if (CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_TX_INTERRUPT]) {
				CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_TX_INTERRUPT]();
			}
		} else if (ir & SJA1000_IR_EI) {
			if (CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_ERROR_INTERRUPT]) {
				CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_ERROR_INTERRUPT]();
			}
		} else if (ir & SJA1000_IR_DOI) {
			if (CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_DATAOVERFLOW_INTERRUPT]) {
				CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_DATAOVERFLOW_INTERRUPT]();
			}
		} else if (ir & SJA1000_IR_WUI) {
			if (CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_WAKEUP_INTERRUPT]) {
				CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_WAKEUP_INTERRUPT]();
			}
		} else if (ir & SJA1000_IR_EPI) {
			if (CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_PASSIVE_INTERRUPT]) {
				CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_PASSIVE_INTERRUPT]();
			}
		} else if (ir & SJA1000_IR_ALI) {
			if (CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_ARITRATION_LOST_INTERRUPT]) {
				CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_ARITRATION_LOST_INTERRUPT]();
			}
		} else if (ir & SJA1000_IR_BEI) {
			if (CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_BUS_ERROR_INTERRUPT]) {
				CARME_CAN_IRQCallbacks[CARME_CAN_IRQID_BUS_ERROR_INTERRUPT]();
			}
		}

		ir = CARME_CAN_Read_Register(SJA1000_IR);

	} while (ir);
}

/**
 *****************************************************************************
 * @brief		Set the Bus-Timing-Registers of the SJA1000 directly.
 *
 * @param[in]	btr0	Content of the BTR0 register
 * @param[in]	btr1	Content of the BTR1 register
 * @return		None
 *****************************************************************************
 */
static void CARME_CAN_SetBusTiming(uint8_t btr0, uint8_t btr1) {

	CARME_CAN_Write_Register(SJA1000_BTR0, btr0);
	CARME_CAN_Write_Register(SJA1000_BTR1, btr1);
}

/**
 *****************************************************************************
 * @brief		Set the baudrate for the CAN-Communication.
 *
 * The Bus-Timing is set as follows for the different baudrates:\n
 * @arg 125kHz: SJW = 1, TSEG1 = 12, TSEG2 =  1, SAM (Trible) = 0\n
 * @arg 250kHz: SJW = 1, TSEG1 = 12, TSEG2 =  1, SAM (Trible) = 0\n
 * @arg 500kHz: SJW = 1, TSEG1 = 15, TSEG2 = 15, SAM (Trible) = 0\n
 * @arg   1MHz: SJW = 1, TSEG1 = 12, TSEG2 =  9, SAM (Trible) = 0
 *
 * @param[in]	baud Baudrate. Can be one of the following constants:\n
 *				@arg #CARME_CAN_BAUD_125K for 125 kBit/s\n
 *				@arg #CARME_CAN_BAUD_250K for 250 kBit/s\n
 *				@arg #CARME_CAN_BAUD_500K for 500 kBit/s\n
 *				@arg #CARME_CAN_BAUD_1M for 1 MBit/s
 * @return		 0 = #CARME_NO_ERROR\n
 * 				61 = #CARME_ERROR_CAN_INVALID_BAUDRATE
 *****************************************************************************
 */
ERROR_CODES CARME_CAN_SetBaudrate(uint32_t baud) {

	switch (baud) {
	case CARME_CAN_BAUD_125K:
		CARME_CAN_SetBusTiming(0x45, 0x1c);			/* 125k Baud	*/
		break;
	case CARME_CAN_BAUD_250K:
		CARME_CAN_SetBusTiming(0x42, 0x1c);			/* 250k Baud	*/
		break;
	case CARME_CAN_BAUD_500K:
		CARME_CAN_SetBusTiming(0x40, 0x6f);			/* 500k Baud	*/
		break;
	case CARME_CAN_BAUD_1M:
		CARME_CAN_SetBusTiming(0x40, 0x09);			/* 1M Baud		*/
		break;
	default:
		return CARME_ERROR_CAN_INVALID_BAUDRATE;
		break;
	}

	return CARME_NO_ERROR;
}

/**
 *****************************************************************************
 * @brief		Sends a CAN-Messsage over the CAN-Bus.
 *
 * @param[in]	txMsg	CAN-ID
 * @return		 0 = #CARME_NO_ERROR\n
 * 				66 = #CARME_ERROR_CAN_ERROR_STATUS
 *****************************************************************************
 */
ERROR_CODES CARME_CAN_Write(CARME_CAN_MESSAGE* txMsg) {

	uint8_t sr;
	uint8_t i;
	uint8_t txDataOffset;

	/* wait until transmit buffer is free */
	do {

		sr = CARME_CAN_Read_Register(SJA1000_SR);
		if (sr & SJA1000_SR_ES) {
			return CARME_ERROR_CAN_ERROR_STATUS;
		}
	} while ((sr & SJA1000_SR_TBS) == 0);

	/* prepare frame format information */
	unsigned char ff = 0;
	if (txMsg->ext) {
		/* set FF bit for extended frame (EFF) */
		ff |= SJA1000_FRAMEINFO_FF;
	}

	if (txMsg->rtr) {
		ff |= SJA1000_FRAMEINFO_RTR;
	}

	ff |= txMsg->dlc & 0xff;

	/* write frame format information */
	CARME_CAN_Write_Register(SJA1000_TX_BUF(0), ff);

	/* write identifier bytes */
	if (txMsg->ext) {
		CARME_CAN_Write_Register(SJA1000_TX_BUF(1), (txMsg->id >> 21) & 0xff);
		CARME_CAN_Write_Register(SJA1000_TX_BUF(2), (txMsg->id >> 13) & 0xff);
		CARME_CAN_Write_Register(SJA1000_TX_BUF(3), (txMsg->id >> 5) & 0xff);
		CARME_CAN_Write_Register(SJA1000_TX_BUF(4), (txMsg->id << 3) & 0xf8);
	}
	else {
		CARME_CAN_Write_Register(SJA1000_TX_BUF(1), (txMsg->id >> 3) & 0xff);
		CARME_CAN_Write_Register(SJA1000_TX_BUF(2), (txMsg->id << 5) & 0xe0);
	}

	if (txMsg->ext) {
		txDataOffset = SJA1000_TX_BUF(5);
	}
	else {
		txDataOffset = SJA1000_TX_BUF(3);
	}
	for (i = 0; i < max(txMsg->dlc, 8); i++) {
		CARME_CAN_Write_Register(txDataOffset + i, txMsg->data[i]);
	}

	/* send data */
	CARME_CAN_Write_Register(SJA1000_CMR, SJA1000_CMR_TR);

	return CARME_NO_ERROR;
}

/**
 *****************************************************************************
 * @brief
 * <b>ACCEPTANCE FILTER</b>\n
 * With the help of the acceptance filter the CAN controller is
 * able to allow passing of received messages to the RXFIFO
 * only when the identifier bits of the received message are
 * equal to the predefined ones within the acceptance filter
 * registers.\n
 * The acceptance filter is defined by the Acceptance Code
 * Registers (ACRn) and the Acceptance Mask Registers
 * (AMRn). The bit patterns of messages to be received are
 * defined within the acceptance code registers.
 * The corresponding acceptance mask registers allow to
 * define certain bit positions to be 'don't care'.
 * Two different filter modes are selectable within the mode
 * register:
 * @arg	Single filter mode (bit AFM is logic 1)
 * @arg	Dual filter mode (bit AFM is logic 0).
 *
 * @brief
 * <b> Single filter configuration</b>\n
 * In this filter configuration one long filter (4-bytes) could be
 * defined. The bit correspondences between the filter bytes
 * and the message bytes depend on the currently received
 * frame format.\n\n
 * <b> Standard frame:</b>  if a standard frame format message is
 * received, the complete identifier including the RTR bit and
 * the first two data bytes are used for acceptance filtering.
 * Messages may also be accepted if there are no data bytes
 * existing due to a set RTR bit or if there is none or only one
 * data byte because of the corresponding data length code.
 * For a successful reception of a message, all single bit
 * comparisons have to signal acceptance.\n
 * @note The 4 least significant bits of AMR1 and ACR1
 * are not used. In order to be compatible with future products
 * these bits should be programmed to be 'don't care' by
 * setting <em>AMR1.3, AMR1.2, AMR1.1 and AMR1.0 to logic 1</em>.\n\n
 *
 * @image html acceptanceFilter.png
 * "Single filter configuration, receiving standard frame messages."
 *
 * <b> Extended frame:</b>  if an extended frame format message is
 * received, the complete identifier including the RTR bit is
 * used for acceptance filtering.\n
 * For a successful reception of a message, all single bit
 * comparisons have to signal acceptance.\n
 * @note The 2 least significant bits of AMR3
 * and ACR3 are not used. In order to be compatible with
 * future products these bits should be programmed to be
 * 'don't care' by setting <em>AMR3.1 and AMR3.0 to logic 1</em>.\n\n
 * @brief
 * <b>Dual filter configuration</b>\n
 * In this filter configuration two short filters can be defined.
 * A received message is compared with both filters to
 * decide, whether this message should be copied into the
 * receive buffer or not. If at least one of the filters signals an
 * acceptance, the received message becomes valid. The bit
 * correspondences between the filter bytes and the
 * message bytes depends on the currently received frame
 * format.\n\n
 * <b>Standard frame:</b> if a standard frame message is received,
 * the two defined filters are looking different. The first filter
 * compares the complete standard identifier including the
 * RTR bit and the first data byte of the message. The second
 * filter just compares the complete standard identifier
 * including the RTR bit.\n
 * For a successful reception of a message, all single bit
 * comparisons of at least one complete filter have to signal
 * acceptance. In case of a set RTR bit or a data length code
 * of logic 0 no data byte is existing. Nevertheless a message
 * may pass filter 1, if the first part up to the RTR bit signals
 * acceptance.\n
 * @note If no data byte filtering is required for filter 1, the four least
 * significant bits of AMR1 and AMR3 have to be set to
 * logic 1 (don't care). Then both filters are working
 * identically using the standard identifier range including the
 * RTR bit.\n\n
 *
 * @image html DualAcceptanceFilterMode.png
 * "Dual filter configuration, receiving standard frame messages."
 *
 * @brief
 * <b>Extended frame:</b> if an extended frame message is received, the two
 * defined filters are looking identically. Both filters
 * are comparing the first two bytes of the extended identifier range only.\n
 * For a successful reception of a message, all single bit comparisons of at
 * least one complete filter have to indicate
 * acceptance.
 *
 * @param[in]	af	acceptance filter information. Refer the struct
 * 				#CARME_CAN_ACCEPTANCE_FILTER for details.
 * @return		 0 = #CARME_NO_ERROR\n
 *				44 = #CARME_ERROR_CAN_INVALID_OPMODE\n
 *				45 = #CARME_ERROR_CAN_INVALID_ACCEPTANCE_MODE
 *****************************************************************************
 */
ERROR_CODES CARME_CAN_SetAcceptaceFilter(CARME_CAN_ACCEPTANCE_FILTER* af) {

	ERROR_CODES err = CARME_NO_ERROR;
	uint8_t mod;
	uint8_t i;

	if (!(CARME_CAN_Read_Register(SJA1000_MOD) & SJA1000_MOD_RM)) {
		err = CARME_ERROR_CAN_INVALID_OPMODE;
	}

	/* set/clear AFM bit in MOD register */
	mod = CARME_CAN_Read_Register(SJA1000_MOD);
	if (af->afm == MODE_SINGLE) {
		mod |= SJA1000_MOD_AFM;
	}
	else if (af->afm == MODE_DUAL) {
		mod &= ~SJA1000_MOD_AFM;
	}
	else {
		err = CARME_ERROR_CAN_INVALID_ACCEPTANCE_MODE;
	}
	CARME_CAN_Write_Register(SJA1000_MOD, mod);

	/* write to acceptance mask and code registers */
	for (i = 0; i < 4; i++) {
		CARME_CAN_Write_Register(SJA1000_ACR(i), af->acr[i]);
		CARME_CAN_Write_Register(SJA1000_AMR(i), af->amr[i]);
	}

	return err;
}

/**
 *****************************************************************************
 * @brief		Get the Acceptance Filter settings.
 * @see			CARME_CAN_setAcceptaceFilter()
 *
 * @param[in]	af		acceptance filter information. Refer the struct
 * 						@ref CARME_CAN_ACCEPTANCE_FILTER for details.
 * @return		 0 = #CARME_NO_ERROR\n
 *				44 = #CARME_ERROR_CAN_INVALID_OPMODE
 *****************************************************************************
 */
ERROR_CODES CARME_CAN_GetAcceptaceFilter(CARME_CAN_ACCEPTANCE_FILTER* af) {

	ERROR_CODES err = CARME_NO_ERROR;
	uint8_t mod;
	uint8_t i;

	if (!(CARME_CAN_Read_Register(SJA1000_MOD) & SJA1000_MOD_RM)) {
		err = CARME_ERROR_CAN_INVALID_OPMODE;
	}

	/* read AFM bit in MOD register */
	mod = CARME_CAN_Read_Register(SJA1000_MOD);
	if (mod & SJA1000_MOD_AFM) {
		af->afm = MODE_SINGLE;
	}
	else {
		af->afm = MODE_DUAL;
	}

	/* read acceptance mask and code registers */
	for (i = 0; i < 4; i++) {
		af->acr[i] = CARME_CAN_Read_Register(SJA1000_ACR(i));
		af->amr[i] = CARME_CAN_Read_Register(SJA1000_AMR(i));
	}

	return err;
}

#ifdef CARME_CAN_DEBUG
/**
 *****************************************************************************
 * @brief		Print all SJA1000 registers to the stdout.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_CAN_PrintRegisters(void) {

	printf("PeliCAN Mode\n\r"
			"MOD = 0x%02x\n\r"
			"CMR = 0x%02x\n\r"
			"SR = 0x%02x\n\r"
			"IR = 0x%02x\n\r"
			"IER = 0x%02x\n\r"
			"OCR = 0x%02x\n\r"
			"BTR0 = 0x%02x\n\r"
			"BTR1 = 0x%02x\n\r"
			"RMC = 0x%02x\n\r"
			"RBSA = 0x%02x\n\r",
			CARME_CAN_Read_Register(SJA1000_MOD),
			CARME_CAN_Read_Register(SJA1000_CMR),
			CARME_CAN_Read_Register(SJA1000_SR),
			CARME_CAN_Read_Register(SJA1000_IR),
			CARME_CAN_Read_Register(SJA1000_IER),
			CARME_CAN_Read_Register(SJA1000_OCR),
			CARME_CAN_Read_Register(SJA1000_BTR0),
			CARME_CAN_Read_Register(SJA1000_BTR1),
			CARME_CAN_Read_Register(SJA1000_RMC),
			CARME_CAN_Read_Register(SJA1000_RBSA));
}

/**
 *****************************************************************************
 * @brief		Print all SJA1000 registers to a string.
 *
 * @param[out]	pStr	Buffer to write the string into.
 * @return		None
 *****************************************************************************
 */
void CARME_CAN_GetRegisterString(char* pStr) {

	sprintf(pStr, "PeliCAN Mode\n\r"
			"MOD = 0x%02x\n\r"
			"CMR = 0x%02x\n\r"
			"SR = 0x%02x\n\r"
			"IR = 0x%02x\n\r"
			"IER = 0x%02x\n\r"
			"OCR = 0x%02x\n\r"
			"BTR0 = 0x%02x\n\r"
			"BTR1 = 0x%02x\n\r"
			"RMC = 0x%02x\n\r"
			"RBSA = 0x%02x\n\r"
			"RXERR = 0x%02x\n\r"
			"TXERR = 0x%02x\n\r",
			CARME_CAN_Read_Register(SJA1000_MOD),
			CARME_CAN_Read_Register(SJA1000_CMR),
			CARME_CAN_Read_Register(SJA1000_SR),
			CARME_CAN_Read_Register(SJA1000_IR),
			CARME_CAN_Read_Register(SJA1000_IER),
			CARME_CAN_Read_Register(SJA1000_OCR),
			CARME_CAN_Read_Register(SJA1000_BTR0),
			CARME_CAN_Read_Register(SJA1000_BTR1),
			CARME_CAN_Read_Register(SJA1000_RMC),
			CARME_CAN_Read_Register(SJA1000_RBSA),
			CARME_CAN_Read_Register(SJA1000_RXERR),
			CARME_CAN_Read_Register(SJA1000_TXERR));
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
