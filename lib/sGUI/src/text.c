/**
 *****************************************************************************
 * @addtogroup	sGUI
 * @{
 * @defgroup	Text Text
 * @brief		Draw simple characters or full lines.
 * @{
 *
 * @file		text.c
 * @version		1.0
 * @date		2013-10-04
 * @author		rct1
 *
 * @brief		Simple graphic library draw text functionality.\n
 *				Draw simple characters or full lines.
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
#include <stdlib.h>					/* Common types and functions			*/
#include <stdint.h>					/* Standard integer formats				*/
#include "lcd.h"					/* Main functionality and lld			*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	Color of the text.
 */
static LCDCOLOR TextColor = 0xFFFF;

/**
 * @brief	Color of the background.
 */
static LCDCOLOR BackColor = 0x0000;

/**
 * @brief	Current font.
 */
static FONT_T *CurrentFont = &font_5x8;

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		Draws a character on LCD.
 * @param[in]	Xpos		Specifies the X position.
 * @param[in]	Ypos		Specifies the Y position.
 * @param[in]	BackColor	Specifies the Background color code RGB(5-6-5).
 * @param[in]	TextColor	Specifies the Text color code RGB(5-6-5).
 * @param[in]	Font		Fonttype to use @ref font.h.
 * @param[in]	ptr			Pointer to the character data.
 */
static void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, LCDCOLOR BackColor,
                         LCDCOLOR TextColor, FONT_T *Font, void *ptr) {

	uint32_t x = 0, y = 0;
	LCDCOLOR pixels[FONT_MAX_HEIGHT * FONT_MAX_WIDTH];
	uint32_t *PixelCount;
	uint8_t count = 0;

	for (y = Font->height; y > 0; y--) {

		for (x = 0; x < Font->width; x++) {

			PixelCount = ptr + x * Font->datasize;
			if ((*PixelCount & (1 << (y - 1))) == 0) {
				pixels[count] = BackColor;
			}
			else {
				pixels[count] = TextColor;
			}
			count++;
		}
	}

	LCD_WriteArea(Xpos, Ypos, Xpos + Font->width - 1, Ypos + Font->height - 1,
	              &pixels[0]);
}

/**
 * @brief		Sets the Text color.
 * @param[in]	Color	specifies the Text color code RGB(5-6-5).
 */
void LCD_SetTextColor(LCDCOLOR Color) {
	TextColor = Color;
}

/**
 * @brief		Sets the Background color.
 * @param[in]	Color	specifies the Background color code RGB(5-6-5).
 */
void LCD_SetBackColor(LCDCOLOR Color) {
	BackColor = Color;
}

/**
 * @brief		Sets the current font.
 * @param[in]	Font	specifies the font array.
 */
void LCD_SetFont(FONT_T *Font) {
	CurrentFont = Font;
}

/**
 * @brief		Gets the current font.
 * @return		Pointer to the current font.
 */
FONT_T* LCD_GetFont(void) {
	return CurrentFont;
}

/**
 * @brief		Gets the current count of possible lines.
 * @return		Number of possible lines.
 */
uint8_t LCD_GetLineCount(void) {
	return LCD_VER_RESOLUTION / CurrentFont->height;
}

/**
 * @brief		Clears the selected line.
 * @param[in]	Line	The Line to be cleared.
 *						This parameter can be one of the following values:
 *						\arg Linex: where x can be 0..9
 */
void LCD_ClearLine(uint8_t Line) {

	LCD_DrawRectF(0, Line * CurrentFont->height, LCD_HOR_RESOLUTION - 1,
	              CurrentFont->height - 1, BackColor);
}

/**
 * @brief		Displays one character (16dots width, 24dots height) on the
 *				LCD. Position is set with a x- and a y-coordinate.
 * @param[in]	x		X-Position in pixel
 * @param[in]	y		Y-Position in pixel
 * @param[in]	Ascii	Character ascii code, must be between 0x20 and 0x7E.
 */
void LCD_DisplayCharXY(uint16_t x, uint16_t y, char Ascii) {

	Ascii -= 32;

	LCD_DrawChar(
	        x,
	        y,
	        BackColor,
	        TextColor,
	        CurrentFont,
	        (void *) (CurrentFont->data
	                + Ascii * CurrentFont->width * CurrentFont->datasize));
}

/**
 * @brief		Displays one character (16dots width, 24dots height) on the
 *				LCD. Position is set with a line and a column.
 *
 * @param[in]	Line	The Line where to display the character shape.
 *						This parameter can be one of the following values:
 *						\arg Linex: where x can be 0..9
 * @param[in]	Column	Start column address.
 * @param[in]	Ascii	Character ascii code, must be between 0x20 and 0x7E.
 */
void LCD_DisplayCharLine(uint8_t Line, uint8_t Column, char Ascii) {

	LCD_DisplayCharXY(Column * CurrentFont->width, Line * CurrentFont->height,
	                  Ascii);
}

/**
 * @brief		Displays a string on the LCD. Position is set with the x- and
 *				y-coordinate.
 * @param[in]	x		X-Position in pixel
 * @param[in]	y		Y-Position in pixel
 * @param[in]	*ptr	Pointer to string to display on LCD.
 */
void LCD_DisplayStringXY(uint16_t x, uint16_t y, const char *ptr) {

	while (*ptr != 0) {
		LCD_DisplayCharXY(x, y, *ptr++);
		x += CurrentFont->width;
	}
}

/**
 * @brief		Displays a string on a line on the LCD. Position is set with
 *				a line number.
 * @param[in]	Line	The Line where to display the character shape.
 * @param[in]	*ptr	Pointer to string to display on LCD.
 */
void LCD_DisplayStringLine(uint8_t Line, const char *ptr) {

	uint8_t i = 0;

	while ((*ptr != 0) && (i < (LCD_HOR_RESOLUTION / CurrentFont->width))) {
		if (*ptr == '\n') {
			Line++;
		}
		else if (*ptr == '\r') {
			i = 0;
		}
		else if (*ptr == '\t') {
			LCD_DisplayCharLine(Line, i++, ' ');
			while (i % 4) {
				LCD_DisplayCharLine(Line, i++, ' ');
			}
		}
		else if (*ptr == '\b') {
			LCD_DisplayCharLine(Line, --i, ' ');
			i--;
		}
		else {
			LCD_DisplayCharLine(Line, i++, *ptr);
		}
		ptr++;
	}

	while (i < (LCD_HOR_RESOLUTION / CurrentFont->width)) {
		LCD_DisplayCharLine(Line, i++, ' ');
	}
}

/**
 * @brief		Displays a string on the LCD. Position is set at the
 *				y-coordinate on center of the display.
 * @param[in]	Line	The Line where to display the character shape.
 * @param[in]	*ptr	Pointer to string to display on LCD.
 */
void LCD_DisplayStringCenterLine(uint8_t Line, const char *ptr) {

	uint8_t i;
	uint16_t length = 0;

	for (i = 0;
	        (i < LCD_HOR_RESOLUTION / CurrentFont->width) && (ptr[i] != 0);
	        i++) {

		length += CurrentFont->width;
	}

	LCD_DisplayStringXY((LCD_HOR_RESOLUTION - length) / 2,
	                    Line * CurrentFont->height, ptr);
}

/*----- EOF ----------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 */

/**
 * @}
 */
