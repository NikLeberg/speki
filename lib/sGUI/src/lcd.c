/**
 *****************************************************************************
 * @addtogroup	sGUI
 * @{
 * @defgroup	LCD LCD
 * @brief		General LCD functions.
 * @{
 *
 * @file		lcd.c
 * @version		1.0
 * @date		2021-12-21
 * @author		leuen4
 *
 * @brief		Simple graphic library general LCD functions.
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
#include <string.h>					/* memory and string functions			*/
#include "lcd.h"					/* Simple graphic library				*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
/**
 * @brief	Pointer to callback function provided by user. Gets called after
 * 			every update to the lcd so the user can synchronize his tasks or
 * 			update the lcd data.
 */
static lcd_update_callback_t update_callback;

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		Set the callback to be called after every update of the LCD.
 * 				This functionality can be used to synchronize tasks to the
 * 				update rate (normally 50 Hz check \ref TFT_FPS) and prevent
 * 				visual tears and glitches.
 * @note		Function will be called from ISR, don't do lengthy stuff in it.
 * @param[in]	callback Function to call after each update or NULL to reset.
 */
void LCD_RegisterUpdateCallback(lcd_update_callback_t callback) {
    if (callback) {
        update_callback = callback;
        LCD_UpdateCallbackCfg(1);
    } else {
        update_callback = NULL;
        LCD_UpdateCallbackCfg(0);
    }
}

/**
 * @brief 		Dispatch update callback. Gets called from ISR when the update
 * 				to the LCD has finished.
 */
void LCD_DispatchUpdateCallback(void) {
	if (update_callback) {
		update_callback();
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
