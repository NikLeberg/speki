/**
 *****************************************************************************
 * @addtogroup	CARME
 * @{
 * @defgroup	CARME_LCD LCD
 * @brief		CARME-M4 Graphic controller
 * @{
 *
 * @file		ssd1963.c
 * @version		1.0
 * @date		2012-10-10
 * @author		aom1
 *
 * @brief		Driver for the SSD1963 Graphic-Controller.
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
#include <stdint.h>					/* Standard integer formats				*/
#include "ssd1963.h"				/* SSD1963 Graphic-Controller			*/
#include "ssd1963_lld.h"			/* SSD1963 Graphic-Controller driver	*/
#include "ssd1963_cmd.h"			/* SSD1963 Graphic-Controller commands	*/
#include "lcd_conf.h"				/* LCD configuration					*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
static uint16_t deviceDescriptorBlock[3];

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief		Inserts a delay time.
 *
 * @param[in]	nCount	specifies the delay time length.
 * @return		None
 *****************************************************************************
 */
static void SSD1963_Delay(uint32_t nCount) {

	volatile uint32_t index = 0;

	for (index = (100000 * nCount); index != 0; index--)
		;
}

/**
 *****************************************************************************
 * @brief		Init the SSD1963 LCD-Controller and clear the display.
 *
 * @return		None
 *****************************************************************************
 */
void SSD1963_Init(void) {

	SSD1963_LLD_Init();

	SSD1963_GetDeviceDescriptorBlock(deviceDescriptorBlock);

	/* Software reset */
	SSD1963_WriteCommand(CMD_SOFT_RESET);
	SSD1963_Delay(10);

	/**
	 * Set pll parameters
	 * Target system clock	: 110MHz
	 * Crystal frequency	: 10MHz
	 * VCO range			: 250MHz - 800MHz
	 * Multiplier			: 33 (N = 0x20)
	 * Divider				: 3 (M = 0x02)
	 */
	SSD1963_WriteCommand(CMD_SET_PLL_MN);		/* Set PLL with OSC = 8 MHz	*/
	SSD1963_WriteData(0x002C);	/* Multiplier N = 44,
								 * VCO (>250MHz) = OSC*(N+1), VCO = 360MHz	*/
	SSD1963_WriteData(0x0002);	/* Divider M = 3, PLL = 360/(M+1) = 120MHz	*/
	SSD1963_WriteData(0xA5);	/* Validate M and N values					*/

	SSD1963_WriteCommand(CMD_PLL_START);		/* Start PLL command		*/
	SSD1963_WriteData(0x01);					/* Enable PLL				*/
	SSD1963_Delay(10);							/* Wait to stabilize		*/

	SSD1963_WriteCommand(CMD_PLL_START);		/* Start PLL command again	*/
	SSD1963_WriteData(0x03);					/* Now, use PLL output as
												 * system clock				*/
	SSD1963_Delay(10);

	/* Set panel mode, varies from individual manufacturer */
	SSD1963_WriteCommand(CMD_SET_PANEL_MODE);
	SSD1963_WriteData(0x0C);					/* 18-bit 3.5" TFT Panel	*/
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(WR_HIGH_BYTE((TFT_WIDTH-1)));		/* Set horizontal	*/
	SSD1963_WriteData(WR_LOW_BYTE((TFT_WIDTH-1)));		/* panel size		*/
	SSD1963_WriteData(WR_HIGH_BYTE((TFT_HEIGHT-1)));	/* Set vertical		*/
	SSD1963_WriteData(WR_LOW_BYTE((TFT_HEIGHT-1)));		/* panel size		*/
	SSD1963_WriteData(0); /* RGB sequence	*/

	/* Pixel data interface (host) */
	SSD1963_WriteCommand(CMD_SET_DATA_INTERFACE);
	SSD1963_WriteData(SSD1963_PDI_16BIT565);

	/**
	 * Set LSHIFT freq, i.e. the DCLK with PLL freq 120 MHz set previously
	 * Typical DCLK for TFT-320x240 is 6.5MHz in 18 bit format
	 * 6.5 MHz = 120 MHz*(LCDC_FPR+1)/2^20
	 */
	SSD1963_WriteCommand(CMD_SET_PCLK);
	SSD1963_WriteData((LCD_FPR >> 16) & 0xFF);
	SSD1963_WriteData((LCD_FPR >> 8) & 0xFF);
	SSD1963_WriteData(LCD_FPR & 0xFF);

	/* Set horizontal period */
	SSD1963_WriteCommand(CMD_SET_HOR_PERIOD);
	SSD1963_WriteData(WR_HIGH_BYTE(TFT_HSYNC_PERIOD));
	SSD1963_WriteData(WR_LOW_BYTE(TFT_HSYNC_PERIOD));
	SSD1963_WriteData(WR_HIGH_BYTE((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
	SSD1963_WriteData(WR_LOW_BYTE((TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH)));
	SSD1963_WriteData(TFT_HSYNC_PULSE);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);

	/* Set vertical period */
	SSD1963_WriteCommand(CMD_SET_VER_PERIOD);
	SSD1963_WriteData(WR_HIGH_BYTE(TFT_VSYNC_PERIOD));
	SSD1963_WriteData(WR_LOW_BYTE(TFT_VSYNC_PERIOD));
	SSD1963_WriteData(WR_HIGH_BYTE((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
	SSD1963_WriteData(WR_LOW_BYTE((TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH)));
	SSD1963_WriteData(TFT_VSYNC_PULSE);
	SSD1963_WriteData(0x00);
	SSD1963_WriteData(0x00);

	/* Setup GPIOs */
	SSD1963_WriteCommand(CMD_SET_GPIO_CONF);	/* Set all GPIOs to
												 * output, controlled by
												 * host						*/
	SSD1963_WriteData(0x0F);	/* Set GPIO0 as output						*/
	SSD1963_WriteData(0x01);	/* GPIO[3:0] used as normal GPIOs			*/

	/* Clear display to black */
	SSD1963_FillArea(SCRN_LEFT, SCRN_TOP, SCRN_RIGHT, SCRN_BOTTOM, 0x00);
	SSD1963_DisplayOn();		/* Turn on display							*/
}

/**
 *****************************************************************************
 * @brief		Puts pixel.
 *
 * @param[in]	x		column from the left.
 * @param[in]	y		row from the top.
 * @param[in]	color	16-Bit color value in the RGB 5-6-5 format.
 * @return		None
 *****************************************************************************
 */
void SSD1963_WritePixel(uint16_t x, uint16_t y, uint16_t color) {

	SSD1963_SetArea(x, y, x + 1, y + 1);
	SSD1963_WriteCommand(CMD_WR_MEMSTART);
	SSD1963_WriteData(color);
}

/**
 *****************************************************************************
 * @brief		Reads pixel.
 *
 * @param[in]	x		column from the left.
 * @param[in]	y		row from the top.
 * @return		16-Bit color value in the RGB 5-6-5 format.
 *****************************************************************************
 */
uint16_t SSD1963_ReadPixel(uint16_t x, uint16_t y) {

	SSD1963_SetArea(x, y, x + 1, y + 1);
	SSD1963_WriteCommand(CMD_RD_MEMSTART);
	return SSD1963_ReadData();
}

/**
 *****************************************************************************
 * @brief		defines start/end columns and start/end rows for memory
 *				access from host to SSD1963.
 *
 * @param[in]	x1		start column from the left.
 * @param[in]	y1		start row from the top.
 * @param[in]	x2		end column from the left.
 * @param[in]	y2		end row from the top.
 * @return		None
 *****************************************************************************
 */
void SSD1963_SetArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

	SSD1963_WriteCommand(CMD_SET_COLUMN);
	SSD1963_WriteData((x1 >> 8) & 0x0FF);
	SSD1963_WriteData((x1) & 0xFF);
	SSD1963_WriteData((x2 >> 8) & 0xFF);
	SSD1963_WriteData((x2) & 0xFF);
	SSD1963_WriteCommand(CMD_SET_PAGE);
	SSD1963_WriteData((y1 >> 8) & 0x0FF);
	SSD1963_WriteData((y1) & 0x0FF);
	SSD1963_WriteData((y2 >> 8) & 0x0FF);
	SSD1963_WriteData((y2) & 0x0FF);
}

/**
 *****************************************************************************
 * @brief		Fill area of specified color.
 *
 * @param[in]	x1		start column from the left.
 * @param[in]	y1		start row from the top.
 * @param[in]	x2		end column from the left.
 * @param[in]	y2		end row from the top.
 * @param[in]	color	16-Bit color value in the RGB 5-6-5 format.
 * @return		None
 *****************************************************************************
 */
void SSD1963_FillArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                      uint16_t color) {

	uint32_t i;

	SSD1963_SetArea(x1, y1, x2, y2);
	SSD1963_WriteCommand(CMD_WR_MEMSTART);
	for (i = 0; i < ((x2 - x1 + 1) * (y2 - y1 + 1)); i++) {
		SSD1963_WriteData(color);
	}
}

/**
 *****************************************************************************
 * @brief		Write a array of data to the display.
 *
 * @param[in]	x1		start column from the left.
 * @param[in]	y1		start row from the top.
 * @param[in]	x2		end column from the left.
 * @param[in]	y2		end row from the top.
 * @param[in]	pData	Pointer to the array of pixels in format of 16-Bit
 *						color value in the RGB 5-6-5 format.
 * @return		None
 *****************************************************************************
 */
void SSD1963_WriteArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                       uint16_t *pData) {

	uint32_t i;

	SSD1963_SetArea(x1, y1, x2, y2);
	SSD1963_WriteCommand(CMD_WR_MEMSTART);
	for (i = 0; i < ((x2 - x1 + 1) * (y2 - y1 + 1)); i++) {
		SSD1963_WriteData(*pData++);
	}
}

/**
 *****************************************************************************
 * @brief		This function enable/disable tearing effect.
 *
 * @param[in]	state	1:	to enable\n
 *						0:	to disable
 * @param[in]	mode	0:	the tearing effect output line consists of
 *							V-blanking information only.\n
 *						1:	the tearing effect output line consists of both
 *							V-blanking and H-blanking info.
 * @return		None
 *****************************************************************************
 */
void SSD1963_SetTearingCfg(uint8_t state, uint8_t mode) {

	if (state == 1) {
		SSD1963_WriteCommand(CMD_SET_TEAR_ON);
		SSD1963_WriteData(mode & 0x01);
	}
	else {
        SSD1963_WriteCommand(CMD_SET_TEAR_OFF);
    }
}

/**
 *****************************************************************************
 * @brief		Get SSD1963 Device Descriptor Block.
 *
 * @param[out]	ddb		Device descriptor block information, stored in 3
 *						uint16_t variables.
 * @return		None
 *****************************************************************************
 */
void SSD1963_GetDeviceDescriptorBlock(uint16_t *ddb) {

	SSD1963_WriteCommand(CMD_RD_DDB_START);
	ddb[0] = SSD1963_ReadData();
	ddb[1] = SSD1963_ReadData();
	ddb[2] = SSD1963_ReadData();
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
