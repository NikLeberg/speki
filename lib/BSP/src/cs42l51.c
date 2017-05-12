/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_Sound Sound
 * @brief		CARME-M4 Sound controller
 * @{
 *
 * @file		cs42l51.c
 * @version		1.0
 * @date		2013-03-11
 * @author		rct1
 *
 * @brief		CS42L51 codec support functions.
 *
 * @todo		Activate microphone and line in.
 * @todo		Test it.
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
#include <i2c.h>					/* CARME I2C definitions				*/
#include <i2s.h>					/* CARME I2S definitions				*/
#include <cs42l51.h>				/* cs42l51 audio codec definitions		*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
void CS42L51_CtrlInterface_Init(void);
void CS42L51_AudioInterface_Init(void);

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		Write a byte to a register of the cs42l51 codec.
 *
 * @param[in]	reg		Register address to write
 * @param[in]	data	Data
 * @return		None
 *****************************************************************************
 */
void CS42L51_WriteReg(uint8_t reg, uint8_t data) {

	CARME_I2C_Write(CODEC_I2C, CODEC_ADDRESS, (uint8_t) reg, 0, &data, 1);
}

/**
 *****************************************************************************
 * @brief		Read a byte from a register of the cs42l51 codec.
 *
 * @param[in]	reg		Register address to read
 * @return		Data
 *****************************************************************************
 */
uint8_t CS42L51_ReadReg(uint8_t reg) {

	uint8_t data;

	CARME_I2C_Read(CODEC_I2C, CODEC_ADDRESS, (uint8_t) reg, 0, &data, 1);
	return data;
}

/**
 *****************************************************************************
 * @brief		Codec CS42L51 low-layer and register initialization.
 *
 * @param[in]	Volume	\ref CS42L51_VolumeOutCtrl.
 * @return		1 if no codec is found, else 0.
 *****************************************************************************
 */
uint8_t CS42L51_Init(int8_t Volume) {

	CS42L51_CtrlInterface_Init();
	CS42L51_AudioInterface_Init();

	/* Check if the codec is connected */
	if (CS42L51_ReadReg(CHIP_ID) != 0xD9) {
		return 1;
	}

	/* Keep Codec powered OFF */
	CS42L51_WriteReg(POWER_CONTROL, 0x01);

	/**
	 * Set mic power and speed
	 * - Select single speed mode (4-50 kHz)
	 */
	CS42L51_WriteReg(MIC_POWER_AND_SPEED, 0x20);

	/**
	 * Configure the interface
	 * - DAC and ADC interface is I2S
	 */
	CS42L51_WriteReg(INTERFACE_CONTROL, 0x0C);

	/**
	 * Set mic control
	 */
	CS42L51_WriteReg(MIC_CONTROL, 0x60);

	/**
	 * Set ADC control configuration
	 * - Enable the high-pass filter
	 */
	CS42L51_WriteReg(ADC_CONTROL, 0xA0);

	/**
	 * Set the ADC configuration
	 * - Select the line in as source
	 */
	CS42L51_WriteReg(ADC_CONFIGURE, 0x00);

	/**
	 * Set DAC output configuration
	 * - HP Gain is 0.6047
	 */
	CS42L51_WriteReg(DAC_OUTPUT_CONTROL, 0x60);

	/**
	 * Set DAC configuration
	 * - Data selection is 00 for PCM or 01 for signal processing engine
	 * - Soft ramp
	 */
	CS42L51_WriteReg(DAC_CONTROL, 0x42);

	/* Set ALCAx and PGAx control */
	CS42L51_WriteReg(ALCA_AND_PGAA_CONTROL, 0x00);
	CS42L51_WriteReg(ALCB_AND_PGAB_CONTROL, 0x00);

	/**
	 * Set PCMx mixer volume
	 * - mute the mixer channel
	 */
	CS42L51_WriteReg(PCMA_MIXER_VOLUME_CONTROL, 0x00);
	CS42L51_WriteReg(PCMB_MIXER_VOLUME_CONTROL, 0x00);

	/* Set treble and bass */
	CS42L51_WriteReg(TONE_CONTROL, 0x88);

	/* Set the output volume */
	CS42L51_WriteReg(AOUTA_VOLUME_CONTROL, 0x18);
	CS42L51_WriteReg(AOUTB_VOLUME_CONTROL, 0x18);

	/**
	 * Set PCM channel mixer
	 * AOUTA = Channel L
	 * AOUTB = Channel R
	 * ADCA = Channel L
	 * ADCB = Channel R
	 */
	CS42L51_WriteReg(PCM_CHANNEL_MIXER, 0x00);

	/**
	 * Set limiter threshold
	 * - Maximum threshold = 0 dB
	 * - Cushion threshold = 0 dB
	 */
	CS42L51_WriteReg(LIMITER_THRESHOLD, 0x00);

	/**
	 * Limiter release rate
	 * - Disabled
	 * - Both channel
	 * - slowest release
	 */
	CS42L51_WriteReg(LIMITER_RELEASE, 0x7F);

	/**
	 * Limiter attack rate
	 * - fastest attack
	 */
	CS42L51_WriteReg(LIMITER_ATTACK, 0x00);

	/* Power down up after initial configure */
	CS42L51_WriteReg(POWER_CONTROL, 0x00);

	/**
	 * Enable periodic beep sound
	 * - On time: 5.2 s
	 * - Off time: 5.2 s
	 * - Frequency: 260.87 Hz (C4)
	 * - Volume: +12 dB
	 * - Repeat beep
	 */
//	CS42L51_WriteReg(BEEP_FREQUENCY_AND_TIMING, 0x0F);
//	CS42L51_WriteReg(BEEP_OFF_TIME_AND_VOLUME, 0x66);
//	CS42L51_WriteReg(BEEP_AND_TONE_CONFIGURATION, 0xC0);
	CS42L51_VolumeOutCtrl(Volume);

	return 0;
}

/**
 *****************************************************************************
 * @brief		Codec CS42L51 ctrl line configuration and register
 *				initialization.
 *
 * @return		None
 *****************************************************************************
 */
void CS42L51_CtrlInterface_Init(void) {

	CARME_I2C_Init(CODEC_I2C);
}

/**
 *****************************************************************************
 * @brief		Codec CS42L51 audio line configuration.
 *
 * @return		None
 *****************************************************************************
 */
void CS42L51_AudioInterface_Init(void) {

	I2S_InitTypeDef I2S_InitStruct;

	/* Enable the peripheral clock */
	RCC_APB1PeriphClockCmd(CODEC_I2S_CLK, ENABLE);

	/* GPIO configuration */
	CARME_I2S_GPIO_Init();

	/* Deinitialize and disable the I2S and SPI hardware */
	SPI_Cmd(CODEC_I2S, DISABLE);
	I2S_Cmd(CODEC_I2S, DISABLE);
	I2S_Cmd(CODEC_I2S_EXT, DISABLE);
	SPI_I2S_DeInit(CODEC_I2S);

	/* CODEC_I2S peripheral configuration */
	I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_48k;
	I2S_InitStruct.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_16b;
	I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
	I2S_InitStruct.I2S_Mode = I2S_Mode_MasterTx;
	I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Enable;

	/* Configure the I2S */
	I2S_Init(CODEC_I2S, &I2S_InitStruct);
	/* Initialize the I2S extended channel for RX */
	I2S_FullDuplexConfig(CODEC_I2S_EXT, &I2S_InitStruct);

	I2S_Cmd(CODEC_I2S, ENABLE);
	I2S_Cmd(CODEC_I2S_EXT, ENABLE);
}

/**
 *****************************************************************************
 * @brief		Get the status of the cs42l51 codec.
 *
 * @return		Status register
 *****************************************************************************
 */
uint8_t CS42L51_Status(void) {

	return CS42L51_ReadReg(STATUS_REGISTER);
}

/**
 *****************************************************************************
 * @brief		Codec CS42L51 output volume control.
 *
 * @param[in]	Volume	Binary code		volume setting\n
 *						  0001 1000			  +12.0 dB\n
 *							...				    ...   \n
 *						  0000 0000			    0.0 dB\n
 *						  1111 1111			   -0.5 dB\n
 *						  1111 1110			   -1.0 dB\n
 *							...				    ...   \n
 *						  0011 0100			   -102 dB\n
 *							...				    ...   \n
 *						  0001 1001			   -102 dB\n
 * @return		None
 *****************************************************************************
 */
void CS42L51_VolumeOutCtrl(int8_t Volume) {

//	uint8_t reg;
//
//	Volume &= 0x07;
//	Volume <<= 5;
//	reg = CS42L51_ReadReg(DAC_OUTPUT_CONTROL);
//	reg &= 0x1F;
//	reg |= Volume;
//
//	CS42L51_WriteReg(DAC_OUTPUT_CONTROL, reg);
	if (Volume > 0xE6) {
		/* Set the Master volume */
		CS42L51_WriteReg(AOUTA_VOLUME_CONTROL, Volume - 0xE7);
		CS42L51_WriteReg(AOUTB_VOLUME_CONTROL, Volume - 0xE7);
	}
	else {
		/* Set the Master volume */
		CS42L51_WriteReg(AOUTA_VOLUME_CONTROL, Volume + 0x19);
		CS42L51_WriteReg(AOUTB_VOLUME_CONTROL, Volume + 0x19);
	}
}

/**
 *****************************************************************************
 * @brief		Mute the CS42L51 output.
 *
 * @param[in]	on		if 0 the codec will unmute the output, else it will
 *						mute it.
 * @return		None
 *****************************************************************************
 */
void CS42L51_Mute(uint8_t on) {

	uint8_t reg;

	reg = CS42L51_ReadReg(DAC_OUTPUT_CONTROL);

	if (on) {
		reg |= 0x03;
	}
	else {
		reg &= ~0x03;
	}

	CS42L51_WriteReg(DAC_OUTPUT_CONTROL, reg);
}

#ifdef __cplusplus
 }
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
