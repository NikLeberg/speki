/**
 *****************************************************************************
 * @addtogroup	sGUI
 * @{
 * @defgroup	Log Log
 * @brief		Print log messages to the display. This module uses a fifo to
 *				show every time the newest log messages.
 * @{
 *
 * @file		log.c
 * @version		1.0
 * @date		2013-09-09
 * @author		rct1
 *
 * @brief		Simple graphic library message console.\n
 *				Print log messages to the display. This module uses a fifo to
 *				show every time the newest log messages.
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
 * @example		log.c
 *				Explains how to generate a log screen.
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <string.h>					/* memory and string functions			*/
#include "lcd.h"					/* Simple graphic library				*/

/*----- Macros -------------------------------------------------------------*/
/**
 * @brief	Maximal number of lines.
 */
#define MAX_LINES		(LCD_VER_RESOLUTION / FONT_MIN_HEIGHT)

/**
 * @brief	Maximal number of columns.
 */
#define MAX_COLUMNS		(LCD_HOR_RESOLUTION / FONT_MIN_WIDTH)

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	Array of all log messages. The maximum line width and line count
 *			are the array dimension.
 */
static char LOG_Messages[MAX_LINES][MAX_COLUMNS];

/**
 * @brief	Index of the \ref LOG_Messages of the last message.
 */
static uint8_t head = 0;

/**
 * @brief	Number of messages which are stored in \ref LOG_Messages.
 */
static uint8_t count = 0;

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		Add a log message to the screen.
 * @param[in]	*ptr	Pointer to string to display on LCD.
 */
void LCD_Log_AddMsg(char *ptr) {

	strncpy(LOG_Messages[head], ptr, (uint8_t) (MAX_COLUMNS));
	LOG_Messages[head][MAX_COLUMNS - 1] = '\0';

	if (++head >= MAX_LINES) {
		head = 0;
	}
	if (++count > MAX_LINES) {
		count = MAX_LINES;
	}

	LCD_Log_Update();
}

/**
 * @brief		Update the log messages on the display. This is used, if the
 *				screen is overwritten by user.
 */
void LCD_Log_Update(void) {

	uint8_t i;
	uint8_t CurrentLine;
	uint8_t LineCount = LCD_GetLineCount();
	uint8_t MaxLines = (LineCount > count) ? count : LineCount;

	for (i = 1; i <= MaxLines; i++) {
		CurrentLine = ((head - i) >= 0) ? head - i : MAX_LINES + head - i;
		LCD_DisplayStringLine(MaxLines - i, LOG_Messages[CurrentLine]);
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
