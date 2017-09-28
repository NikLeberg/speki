/**
 *****************************************************************************
 * @addtogroup	sGUI
 * @{
 * @defgroup	Geometry Geometry
 * @brief		Functionality to draw simple geometric objects on the lcd.
 * @{
 *
 * @file		geometry.c
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
 * @example		geometry.c
 *				Explains how to draw a circles, rectangular and lines.
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

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		Draw a line on the display starting at (Xstart/Ystart) and
 *				ending at (Xstop/Ystop). This function draws the line with the
 *				Bresenham's line algorithm.
 * @param[in]	Xstart	specifies the X start position.
 * @param[in]	Ystart	specifies the Y start position.
 * @param[in]	Xstop	specifies the X stop position.
 * @param[in]	Ystop	specifies the Y stop position.
 * @param[in]	Color	specifies the color of the line.
 */
void LCD_DrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xstop,
                  uint16_t Ystop, LCDCOLOR Color) {

	int16_t dX = 0, dY = 0;
	int16_t sX, sY, err, e2;

	if (Xstop > Xstart) {
		dX = Xstop - Xstart;
	}
	else {
		dX = Xstart - Xstop;
	}
	if (Ystop > Ystart) {
		dY = Ystop - Ystart;
	}
	else {
		dY = Ystart - Ystop;
	}

	if (Xstart < Xstop) {
		sX = 1;
	}
	else {
		sX = -1;
	}
	if (Ystart < Ystop) {
		sY = 1;
	}
	else {
		sY = -1;
	}
	err = dX - dY;

	while (1) {
		LCD_WritePixel(Xstart, Ystart, Color);
		if (Xstart == Xstop && Ystart == Ystop) {
			break;
		}
		e2 = 2 * err;
		if (e2 > -dY) {
			err = err - dY;
			Xstart = Xstart + sX;
		}
		if (e2 < dX) {
			err = err + dX;
			Ystart = Ystart + sY;
		}
	}
}

/**
 * @brief		Displays a rectangle.
 * @param[in]	Xpos	specifies the X position.
 * @param[in]	Ypos	specifies the Y position.
 * @param[in]	width	display rectangle width.
 * @param[in]	height	display rectangle height.
 * @param[in]	Color	specifies the color of the line.
 */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t width,
                  uint16_t height, LCDCOLOR Color) {

	LCD_DrawLine(Xpos, Ypos, Xpos + width, Ypos, Color);
	LCD_DrawLine(Xpos, Ypos, Xpos, Ypos + height, Color);
	LCD_DrawLine(Xpos + width, Ypos, Xpos + width, Ypos + height, Color);
	LCD_DrawLine(Xpos, Ypos + height, Xpos + width, Ypos + height, Color);
}

/**
 * @brief		Displays a filled rectangle.
 * @param[in]	Xpos	specifies the X position.
 * @param[in]	Ypos	specifies the Y position.
 * @param[in]	width	display rectangle width.
 * @param[in]	height	display rectangle height.
 * @param[in]	Color	specifies the color of the line.
 */
void LCD_DrawRectF(uint16_t Xpos, uint16_t Ypos, uint16_t width,
                   uint16_t height, LCDCOLOR Color) {

	LCD_FillArea(Xpos, Ypos, Xpos + width, Ypos + height, Color);
}

/**
 * @brief		Displays a circle.
 * @param[in]	Xpos	specifies the X position of the center.
 * @param[in]	Ypos	specifies the Y position of the center.
 * @param[in]	Radius	radius of the circle.
 * @param[in]	Color	specifies the color of the line.
 */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius,
                    LCDCOLOR Color) {

	int16_t x, y;

	for (x = -Radius; x <= Radius; x++) {
		for (y = -Radius; y <= Radius; y++) {
			if (abs(x * x + y * y - Radius * Radius) <= Radius) {
		LCD_WritePixel(x + Xpos, y + Ypos, Color);
}
	    }
	}
}

/**
 * @brief		Displays a filled circle.
 * @param[in]	Xpos	specifies the X position of the center.
 * @param[in]	Ypos	specifies the Y position of the center.
 * @param[in]	Radius	radius of the circle.
 * @param[in]	Color	specifies the color of the line.
 */
void LCD_DrawCircleF(uint8_t Xpos, uint16_t Ypos, uint16_t Radius,
                     LCDCOLOR Color) {

	int16_t x, y;
	int16_t radError;

	x = Radius;
	y = 0;
	radError = 1 - x;

	while (x >= y) {

			LCD_DrawLine(x + Xpos, y + Ypos, -x + Xpos, y + Ypos, Color);
			LCD_DrawLine(y + Xpos, x + Ypos, -y + Xpos, x + Ypos, Color);
			LCD_DrawLine(-x + Xpos, -y + Ypos, x + Xpos, -y + Ypos, Color);
			LCD_DrawLine(-y + Xpos, -x + Ypos, y + Xpos, -x + Ypos, Color);

	    y++;
	    if (radError < 0) {
			radError += 2 * y + 1;
	    }
	    else {
			x--;
			radError += 2 * (y - x + 1);
	    }
	}
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
