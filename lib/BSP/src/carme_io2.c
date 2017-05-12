/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_IO2 CARME IO2
 * @brief		CARME IO2 high level functions
 * @{
 *
 * @file		carme_io2.c
 * @version		1.0
 * @date		2013-01-09
 * @author		rct1
 *
 * @brief		CARME IO2 extension module board support package.
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
 * @example		spi.c
 *				Explains how to communicate over the SPI bus.
 * @example		pwm.c
 *				Explains how to read from ADC and control the PWM.
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <carme.h>					/* CARME Module							*/
#include <carme_io2.h>				/* CARME IO2 Module						*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	CARME IO2 Port and Pin association
 */
static CARME_Port_Pin_t CARME_IO2_Port_Pin[] = {
		{ GPIOB, GPIO_Pin_0, GPIO_Mode_AN },				/**< ADC Port 0	*/
		{ GPIOC, GPIO_Pin_0, GPIO_Mode_AN },				/**< ADC Port 1	*/
		{ GPIOC, GPIO_Pin_2, GPIO_Mode_AN },				/**< ADC Port 2	*/
		{ GPIOG, GPIO_Pin_8, GPIO_Mode_IN },				/**< GPIO In 0	*/
		{ GPIOG, GPIO_Pin_6, GPIO_Mode_IN },				/**< GPIO In 1	*/
		{ GPIOG, GPIO_Pin_7, GPIO_Mode_IN },				/**< GPIO In 2	*/
		{ CARME_AGPIO_108, GPIO_Mode_IN },					/**< GPIO In 3	*/
		{ CARME_AGPIO_21, GPIO_Mode_OUT },					/**< PWM Phase	*/
		{ GPIOA, GPIO_Pin_0, GPIO_Mode_OUT, GPIO_AF_TIM5 },	/**< PWM 0		*/
		{ GPIOH, GPIO_Pin_11, GPIO_Mode_OUT, GPIO_AF_TIM5 },/**< PWM 1		*/
		{ GPIOH, GPIO_Pin_12, GPIO_Mode_OUT, GPIO_AF_TIM5 },/**< PWM 2		*/
		{ GPIOA, GPIO_Pin_3, GPIO_Mode_AF, GPIO_AF_TIM5 },	/**< PWM 3		*/
		{ GPIOB, GPIO_Pin_5, GPIO_Mode_AF, GPIO_AF_SPI1 },	/**< SPI1 MOSI	*/
		{ GPIOA, GPIO_Pin_6, GPIO_Mode_AF, GPIO_AF_SPI1 },	/**< SPI1 MISO	*/
		{ GPIOA, GPIO_Pin_5, GPIO_Mode_AF, GPIO_AF_SPI1 },	/**< SPI1 CLK	*/
		{ GPIOA, GPIO_Pin_4, GPIO_Mode_OUT },				/**< SPI1 NSS	*/
		{ CARME_AGPIO_22, GPIO_Mode_OUT },					/**< nPSC SEL	*/
};

/**
 * @brief	CARME IO2 GPIO Out Port and Pin association
 */
static CARME_Port_Pin_t CARME_IO2_GPIO_Out_Port_Pin[] = {
		{ GPIOA, GPIO_Pin_0, GPIO_Mode_OUT, GPIO_AF_TIM5 },	/**< GPIO Out 0	*/
		{ GPIOH, GPIO_Pin_11, GPIO_Mode_OUT, GPIO_AF_TIM5 },/**< GPIO Out 1	*/
		{ GPIOH, GPIO_Pin_12, GPIO_Mode_OUT, GPIO_AF_TIM5 },/**< GPIO Out 2	*/
		{ CARME_AGPIO_105, GPIO_Mode_OUT },					/**< GPIO Out 3	*/
};

/**
 * @brief	CARME IO2 OPTO Out Port and Pin association
 */
static CARME_Port_Pin_t CARME_IO2_OPTO_Out_Port_Pin[] = {
        { CARME_AGPIO_96, GPIO_Mode_OUT },					/**< Opto Out 0	*/
		{ CARME_AGPIO_97, GPIO_Mode_OUT },					/**< Opto Out 1	*/
};

/**
 * @brief	CARME IO2 OPTO In Port and Pin association
 */
static CARME_Port_Pin_t CARME_IO2_OPTO_In_Port_Pin[] = {
        { CARME_AGPIO_115, GPIO_Mode_IN },					/**< Opto In 0	*/
		{ CARME_AGPIO_116, GPIO_Mode_IN },					/**< Opto In 1	*/
};

/**
 * @brief	CARME IO2 ADC channel list
 */
static uint8_t CARME_IO2_ADC_Channels[] = {
	ADC_Channel_8,	/**< Analog IN0		*/
	ADC_Channel_10,	/**< Analog IN1		*/
	ADC_Channel_12	/**< Analog IN2		*/
};

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		CARME IO2 initialization.
 *
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	/* Initialize the GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	CARME_GPIO_Init(
	        CARME_IO2_GPIO_Out_Port_Pin, &GPIO_InitStruct,
	        sizeof(CARME_IO2_GPIO_Out_Port_Pin) / sizeof(CARME_Port_Pin_t));

	CARME_GPIO_Init(
	        CARME_IO2_OPTO_Out_Port_Pin, &GPIO_InitStruct,
	        sizeof(CARME_IO2_OPTO_Out_Port_Pin) / sizeof(CARME_Port_Pin_t));

	CARME_GPIO_Init(
	        CARME_IO2_OPTO_In_Port_Pin, &GPIO_InitStruct,
	        sizeof(CARME_IO2_OPTO_In_Port_Pin) / sizeof(CARME_Port_Pin_t));

	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	CARME_GPIO_Init(CARME_IO2_Port_Pin, &GPIO_InitStruct,
	                sizeof(CARME_IO2_Port_Pin) / sizeof(CARME_Port_Pin_t));

	/* Initialize the ADC */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit();
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_10b;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStruct);
	ADC_Cmd(ADC1, ENABLE);

	/* Initialize the SPI */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	CARME_IO2_SPI_CS_Out(Bit_SET);	/* set CS high */
	SPI_DeInit(SPI1);
	/**
	 * CARME IO2 SPI bus
	 *	- Mode = Master
	 *	- DataSize = 16 Bit
	 *	- CPOL = high
	 *	- CPHA = 2 Edge
	 *	- NSS = Soft internal
	 *	- Baud rate prescaler = 64 (APB2 / 64)
	 */
	SPI_StructInit(&SPI_InitStruct);
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_Cmd(SPI1, ENABLE);

	/* Initialize the PWM */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
}

/**
 *****************************************************************************
 * @brief		Configure the CARME IO2 GPIO Out 0..2 as GPIO or PWM.
 *
 * @param[in]	pin		Pins to configure. This parameter can be any value of
 *						the following
 *						@arg CARME_IO2_GPIO_OUT_PIN0:	GPIO OUT0
 *						@arg CARME_IO2_GPIO_OUT_PIN1:	GPIO OUT1
 *						@arg CARME_IO2_GPIO_OUT_PIN2:	GPIO OUT2
 * @param[in]	mode	This parameter can be one of the following
 *						@arg CARME_IO2_GPIO_OUT_MODE_GPIO:	Pin is configured
 *															as GPIO
 *						@arg CARME_IO2_GPIO_OUT_MODE_PWM:	Pin is configured
 *															as PWM
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_GPIO_OUT_Settings(uint8_t pin, CARME_IO2_GPIO_OUT_MODE mode) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* Initialize the GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	if (pin & CARME_IO2_GPIO_OUT_PIN0) {
		CARME_IO2_GPIO_Out_Port_Pin[0].GPIO_Mode = mode;
	}
	if (pin & CARME_IO2_GPIO_OUT_PIN1) {
		CARME_IO2_GPIO_Out_Port_Pin[1].GPIO_Mode = mode;
	}
	if (pin & CARME_IO2_GPIO_OUT_PIN2) {
		CARME_IO2_GPIO_Out_Port_Pin[2].GPIO_Mode = mode;
	}

	CARME_GPIO_Init(
	        CARME_IO2_GPIO_Out_Port_Pin, &GPIO_InitStruct,
	        sizeof(CARME_IO2_GPIO_Out_Port_Pin) / sizeof(CARME_Port_Pin_t));
}

/**
 *****************************************************************************
 * @brief		Get the value of an ADC channel.
 *
 * @param[in]	channel	ADC channel which are defined in CARME_IO2_ADC_CHANNEL
 * @param[out]	pValue	ADC value
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_ADC_Get(CARME_IO2_ADC_CHANNEL channel, uint16_t *pValue) {

	ADC_RegularChannelConfig(ADC1, CARME_IO2_ADC_Channels[channel], 1,
	                         ADC_SampleTime_15Cycles);
	ADC_SoftwareStartConv(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	*pValue = ADC_GetConversionValue(ADC1) & 0x03FF;
}

/**
 *****************************************************************************
 * @brief		Set the value of an DAC channel.
 *
 * @param[in]	channel	DAC channel which are defined in
 *						#CARME_IO2_DAC_CHANNEL
 * @param[out]	Value	DAC value (12 Bit, 1mV/Bit)
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_DAC_Set(CARME_IO2_DAC_CHANNEL channel, uint16_t Value) {

	CARME_IO2_SPI_Select(CARME_IO2_nPSC0);	/* Use SPI0 to DAC				*/
	CARME_IO2_SPI_CS_Out(Bit_RESET);		/* set CS low					*/
	CARME_IO2_SPI_Send(CARME_IO2_LTC2622_CMD_WUn << 4 | channel);
	CARME_IO2_SPI_Send(Value << 4);			/* 12 Bit value and 4 don't care*/
	CARME_IO2_SPI_CS_Out(Bit_SET);			/* set CS high					*/
}

/**
 *****************************************************************************
 * @brief		Select the peripheral SPI chip.
 *
 * @param[in]	select	Select nPSC0 or nPSC1\n
 *						@arg nPSC0:	DAC (SPI0)\n
 *						@arg nPSC1:	External (SPI1)
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_SPI_Select(CARME_IO2_SPI_CHANNEL select) {

	CARME_AGPIO_Set(select == CARME_IO2_nPSC0 ? 0 : CARME_AGPIO_PIN_22,
	                CARME_AGPIO_PIN_22 );
}

/**
 *****************************************************************************
 * @brief		Generate the chip select pin.
 *
 * @param[in]	cs		Set the chip select pin.\n
 *						@arg Bit_RESET:	CS = low\n
 *						@arg Bit_SET:	CS = high
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_SPI_CS_Out(uint8_t cs) {

	GPIO_WriteBit(GPIOA, GPIO_Pin_4, cs);
}

/**
 *****************************************************************************
 * @brief		Send a half word over the SPI port.
 *
 * @param[in]	data	Half word you want to send.
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_SPI_Send(uint16_t data) {

	/* write data to be transmitted to the SPI data register */
	SPI_I2S_SendData(SPI1, data);

	/* wait until transmit complete */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		;
	/* wait until receive complete */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		;
	/* wait until SPI is not busy anymore */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
}

/**
 *****************************************************************************
 * @brief		Receive a half word from the SPI input buffer.
 *
 * @param[out]	pValue	Pointer to the half word buffer to write data in.
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_SPI_Receive(uint16_t *pValue) {

	/* wait until transmit complete */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		;
	/* wait until receive complete */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		;
	/* wait until SPI is not busy anymore */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;

	*pValue = SPI_I2S_ReceiveData(SPI1);
}

/**
 *****************************************************************************
 * @brief		Set the PWM time base settings.
 *
 * @param[in]	pTIM_TimeBaseStruct	Pointer to the PWM time base init
 *									structure.
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_PWM_Settings(TIM_TimeBaseInitTypeDef *pTIM_TimeBaseStruct) {

	TIM_Cmd(TIM5, DISABLE);
	TIM_DeInit(TIM5);
	TIM_TimeBaseInit(TIM5, pTIM_TimeBaseStruct);
	TIM_Cmd(TIM5, ENABLE);
}

/**
 *****************************************************************************
 * @brief		Set the PWM time base settings.
 *
 * @param[in]	channel	PWM channel which are defined in CARME_IO2_PWM_CHANNEL
 * @param[in]	value	PWM output value
 * @return		 0 = #CARME_NO_ERROR\n
 *				11 = #CARME_ERROR_IO2_PWM_WRONG_CHANNEL
 *****************************************************************************
 */
ERROR_CODES CARME_IO2_PWM_Set(CARME_IO2_PWM_CHANNEL channel, uint16_t value) {

	ERROR_CODES err = CARME_NO_ERROR;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = value;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	switch (channel) {
	case CARME_IO2_PWM0:
		TIM_OC1Init(TIM5, &TIM_OCInitStruct);
		TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case CARME_IO2_PWM1:
		TIM_OC2Init(TIM5, &TIM_OCInitStruct);
		TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case CARME_IO2_PWM2:
		TIM_OC3Init(TIM5, &TIM_OCInitStruct);
		TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	case CARME_IO2_PWM3:
		TIM_OC4Init(TIM5, &TIM_OCInitStruct);
		TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
		break;
	default:
		err = CARME_ERROR_IO2_PWM_WRONG_CHANNEL;
		break;
	}
	TIM_ARRPreloadConfig(TIM5, ENABLE);

	return err;
}

/**
 *****************************************************************************
 * @brief		Set the DC motor direction.
 *
 * @param[in]	dir		Direction of the PWM signal
 *
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_PWM_Phase(CARME_IO2_PWM_PHASE dir) {

	CARME_AGPIO_Set(
	        (dir == CARME_IO2_PWM_NORMAL_DIRECTION) ? CARME_AGPIO_PIN_21 : 0,
	        CARME_AGPIO_PIN_21 );
}

/**
 *****************************************************************************
 * @brief		Get the GPIO In 0..3 value.
 *
 * @param[out]	pStatus	Status of the GPIO input bits [0..3].
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_GPIO_IN_Get(uint8_t *pStatus) {

	uint32_t agpio_state;
	uint8_t value = 0;

	value |= (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8) != Bit_RESET) ? 0x1 : 0;
	value |= (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) != Bit_RESET) ? 0x2 : 0;
	value |= (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_7) != Bit_RESET) ? 0x4 : 0;
	CARME_AGPIO_Get(&agpio_state);
	value |= (agpio_state & CARME_AGPIO_PIN_108) ? 0x8 : 0;
	*pStatus = value;
}

/**
 *****************************************************************************
 * @brief		Set the GPIO Out 0..3 value.
 *
 * @param[out]	Status	Status of the GPIO output bits [0..3].
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_GPIO_OUT_Set(uint8_t Status) {

	uint8_t i;
	BitAction BitVal;

	for (i = 0; i < sizeof(CARME_IO2_GPIO_Out_Port_Pin) / sizeof(CARME_Port_Pin_t);
	        i++) {

		BitVal = (Status & (1 << i)) ? Bit_SET : Bit_RESET;
		GPIO_WriteBit(CARME_IO2_GPIO_Out_Port_Pin[i].GPIOx,
		              CARME_IO2_GPIO_Out_Port_Pin[i].GPIO_Pin, BitVal);
	}
}

/**
 *****************************************************************************
 * @brief		Get the OPTO In 0..1 value.
 *
 * @param[out]	pStatus	Status of the OPTO input bits.
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_OPTO_IN_Get(uint8_t *pStatus) {

	uint8_t i;
	uint32_t BitStatus = 0;

	for (i = 0; i < sizeof(CARME_IO2_OPTO_In_Port_Pin) / sizeof(CARME_Port_Pin_t);
	        i++) {

		if (GPIO_ReadInputDataBit(CARME_IO2_OPTO_In_Port_Pin[i].GPIOx,
			                      CARME_IO2_OPTO_In_Port_Pin[i].GPIO_Pin)
				!= Bit_RESET) {

			BitStatus |= (1 << i);
		}
	}
	*pStatus = BitStatus;
}

/**
 *****************************************************************************
 * @brief		Set the OPTO Out 0..1 value.
 *
 * @param[out]	Status	Status of the OPTO input bits.
 * @return		None
 *****************************************************************************
 */
void CARME_IO2_OPTO_OUT_Set(uint8_t Status) {

	uint8_t i;
	BitAction BitVal;

	for (i = 0; i < sizeof(CARME_IO2_OPTO_Out_Port_Pin) / sizeof(CARME_Port_Pin_t);
	        i++) {

		BitVal = (Status & (1 << i)) ? Bit_SET : Bit_RESET;
		GPIO_WriteBit(CARME_IO2_OPTO_Out_Port_Pin[i].GPIOx,
		              CARME_IO2_OPTO_Out_Port_Pin[i].GPIO_Pin, BitVal);
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
