/**
 *****************************************************************************
 * @addtogroup	sGUI
 * @{
 * @defgroup	BMP BMP
 * @brief		Library to print a bmp from a file to the LCD display.
 *				This library depends on the FatFs from ChaN.
 * @{
 *
 * @file		bmp.c
 * @version		1.0
 * @date		2013-10-04
 * @author		rct1
 *
 * @brief		Simple graphic library bitmap support.\n
 *				Library to print a bmp from a file to the LCD display.
 *				This library depends on the FatFs from ChaN.
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
 * @example		bitmap.c
 *				Explains how to draw a bitmap to the LCD with the CARME-M4
 *				kit.
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stdlib.h>					/* Common types, and functions			*/
#include <stdint.h>					/* Standard integer formats				*/
#include <string.h>					/* memory and string functions			*/
#include <ff.h>						/* Fat file system						*/
#include "lcd.h"					/* Simple graphic library				*/

/*----- Macros -------------------------------------------------------------*/
#ifndef _FATFS						/* This module only works with FatFs	*/
	#error Add the FatFs Library or delete the Bitmap module.
#endif /* _FATFS */

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
BMP_STATUS LCD_BMP_ReadHeader(BMP *bmp, FIL *f);
static int LCD_BMP_Read_uint32_t(uint32_t *x, FIL *f);
static int LCD_BMP_Read_uint16_t(uint16_t *x, FIL *f);

/*----- Data ---------------------------------------------------------------*/
/* Error description strings */
static const char* BMP_ERROR_STRING[] = {
	"",
	"General error",
	"Could not allocate enough memory to complete the operation",
	"File input/output error",
	"File not found",
	"File is not a supported BMP variant (must be uncompressed 16, 24 or 32 BPP)",
	"File is not a valid BMP image",
	"An argument is invalid or out of range",
	"The requested action is not compatible with the BMP's type"
};

/*----- Implementation -----------------------------------------------------*/
/**
 * @brief		Reads the specified BMP image file and print it to the LCD.\n
 *				It does only read bitmaps with 16, 24 or 32 bits per pixel
 *				and uncompressed.\n
 *				To use this, the fatfs must be mounted.
 * @param[in]	filename	Name of the file (incl. ending and directory) on
 *				the sdcard.
 * @param[in]	Xpos		Start X position of the bitmap.
 * @param[in]	Ypos		Start Y position of the bitmap.
 * @return		BMP_STATUS
 */
BMP_STATUS LCD_BMP_DrawBitmap(const char *filename, uint16_t Xpos,
                              uint16_t Ypos) {

	BMP bmp;
	FIL f;
	UINT bytesread;
	uint8_t buffer[4];
	uint16_t x, y;
	uint16_t pixel = 0;

	if (filename == NULL) {
		return BMP_INVALID_ARGUMENT;
	}

	/* Open file */
	if (f_open(&f, filename, FA_READ) != FR_OK) {
		return BMP_FILE_NOT_FOUND;
	}

	/* Read header */
	if (LCD_BMP_ReadHeader(&bmp, &f) != BMP_OK
	        || bmp.Header.Magic != 0x4D42) {
		f_close(&f);
		return BMP_FILE_INVALID;
	}

	/* Verify that the bitmap variant is supported */
	if ((bmp.Header.BitsPerPixel != 32 && bmp.Header.BitsPerPixel != 24
	        && bmp.Header.BitsPerPixel != 16)
	        || bmp.Header.CompressionType != 0
	        || bmp.Header.HeaderSize != 40) {
		f_close(&f);
		return BMP_FILE_NOT_SUPPORTED;
	}

	/* Read image data */
	for (y = 0; y < bmp.Header.Height; y++) {
		for (x = 0; x < bmp.Header.Width; x++) {
			/* Read one pixel in format BGR */
			if (f_read(&f, &buffer[0], bmp.Header.BitsPerPixel / 8,
			           &bytesread) == FR_OK) {
				if (bmp.Header.BitsPerPixel == 16) {
					/* BMP is still in the RGB 5-6-5 format */
					pixel = (buffer[0] << 8) | buffer[1];
				}
				else {
					/* Format pixel into the RGB 5-6-5 format */
					pixel = ((buffer[0] >> 3) & 0x001F)
					        | ((buffer[1] << 3) & 0x07E0)
					        | ((buffer[2] << 8) & 0xF800);
				}
				LCD_WritePixel(Xpos + x, bmp.Header.Height + Ypos - y, pixel);
			}
			else {
				f_close(&f);
				return BMP_FILE_INVALID;
			}
		}
	}

	f_close(&f);

	return BMP_OK;
}

/**
 * @brief		Reads the specified BMP image file.\n
 *				It does only read bitmaps with 16, 24 or 32 bits per pixel
 *				and uncompressed.
 *				To use this, the fatfs must be mounted.
 * @param[out]	bmp			Pointer to the bitmap data buffer.
 * @param[in]	filename	File to read into the buffer.
 */
BMP_STATUS LCD_BMP_ReadFile(BMP *bmp, const char *filename) {

	FIL f;
	UINT bytesread;

	if (filename == NULL) {
		return BMP_INVALID_ARGUMENT;
	}

	if (bmp == NULL) {
		return BMP_OUT_OF_MEMORY;
	}

	/* Open file */
	if (f_open(&f, filename, FA_READ) != FR_OK) {
		return BMP_FILE_NOT_FOUND;
	}

	/* Read header */
	if (LCD_BMP_ReadHeader(bmp, &f) != BMP_OK
	        || bmp->Header.Magic != 0x4D42) {
		f_close(&f);
		return BMP_FILE_INVALID;
	}

	/* Verify that the bitmap variant is supported */
	if ((bmp->Header.BitsPerPixel != 32 && bmp->Header.BitsPerPixel != 24
	        && bmp->Header.BitsPerPixel != 16)
	        || bmp->Header.CompressionType != 0
	        || bmp->Header.HeaderSize != 40) {
		f_close(&f);
		return BMP_FILE_NOT_SUPPORTED;
	}

	/* Allocate memory for image data */
	if (bmp->Data == NULL) {
		f_close(&f);
		return BMP_OUT_OF_MEMORY;
	}

	/* Read image data */
	if ((f_read(&f, bmp->Data, bmp->Header.ImageDataSize, &bytesread) != FR_OK)
	        || (bytesread != bmp->Header.ImageDataSize)) {
		f_close(&f);
		return BMP_FILE_INVALID;
	}

	f_close(&f);

	return BMP_OK;
}

/**
 * @brief		Returns the image's width.
 * @param[in]	bmp		Pointer to the bitmap data buffer.
 * @return		The bitmap width.
 */
uint32_t LCD_BMP_GetWidth(BMP *bmp) {

	if (bmp == NULL) {
		return -1;
	}

	return (bmp->Header.Width);
}

/**
 * @brief		Returns the image's height.
 * @param[in]	bmp		Pointer to the bitmap data buffer.
 * @return		The bitmap height.
 */
uint32_t LCD_BMP_GetHeight(BMP *bmp) {

	if (bmp == NULL) {
		return -1;
	}

	return (bmp->Header.Height);
}

/**
 * @brief		Returns the image's color depth (bits per pixel).
 * @param[in]	bmp		Pointer to the bitmap data buffer.
 * @return		Bits per pixel.
 */
uint16_t LCD_BMP_GetDepth(BMP *bmp) {

	if (bmp == NULL) {
		return -1;
	}

	return (bmp->Header.BitsPerPixel);
}

/**
 * @brief		Populates the arguments with the specified pixel's RGB values.
 * @param[in]	bmp		Pointer to the bitmap data buffer.
 * @param[in]	x		X-Position of the pixel.
 * @param[in]	y		Y-Position of the pixel.
 * @param[out]	r		Red
 * @param[out]	g		Green
 * @param[out]	b		Blue
 * @return		BMP_OK if no error.
 */
BMP_STATUS LCD_BMP_GetPixelRGB(BMP *bmp, uint32_t x, uint32_t y, uint8_t *r,
                               uint8_t *g, uint8_t *b) {

	uint8_t *pixel;
	uint32_t bytes_per_row;
	uint8_t bytes_per_pixel;

	if (bmp == NULL || x < 0 || x >= bmp->Header.Width || y < 0
	        || y >= bmp->Header.Height) {
		return BMP_INVALID_ARGUMENT;
	}
	else {
		bytes_per_pixel = bmp->Header.BitsPerPixel >> 3;

		/* Row's size is rounded up to the next multiple of 4 bytes */
		bytes_per_row = bmp->Header.ImageDataSize / bmp->Header.Height;

		/* Calculate the location of the relevant pixel (rows are flipped) */
		pixel = bmp->Data
		        + ((bmp->Header.Height - y - 1) * bytes_per_row
		                + x * bytes_per_pixel);

		/* Note: colors are stored in BGR order */
		if (r) {
			*r = *(pixel + 2);
		}
		if (g) {
			*g = *(pixel + 1);
		}
		if (b) {
			*b = *(pixel + 0);
		}
	}

	return BMP_OK;
}

/**
 * @brief		Returns a description of the last error code.
 * @param[in]	errorcode	Error Code number.
 * @return		Pointer to a const string with the description.
 */
const char* LCD_BMP_GetErrorDescription(uint8_t errorcode) {

	if (errorcode > 0 && errorcode < BMP_ERROR_NUM) {
		return BMP_ERROR_STRING[errorcode];
	}
	else {
		return NULL;
	}
}

/**
 * @brief		Reads the BMP file's header into the data structure.
 * @param[out]	bmp		Pointer to the bitmap data buffer.
 * @param[in]	f		Pointer to the file structure.
 * @return		BMP_OK if no error
 */
BMP_STATUS LCD_BMP_ReadHeader(BMP *bmp, FIL *f) {

	if (bmp == NULL || f == NULL) {
		return BMP_INVALID_ARGUMENT;
	}

	/* The header's fields are read one by one, and converted from the format's
	 little endian to the system's native representation. */
	if (!LCD_BMP_Read_uint16_t(&(bmp->Header.Magic), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.FileSize), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint16_t(&(bmp->Header.Reserved1), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint16_t(&(bmp->Header.Reserved2), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.DataOffset), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.HeaderSize), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.Width), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.Height), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint16_t(&(bmp->Header.Planes), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint16_t(&(bmp->Header.BitsPerPixel), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.CompressionType), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.ImageDataSize), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.HPixelsPerMeter), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.VPixelsPerMeter), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.ColorsUsed), f))
		return BMP_IO_ERROR;
	if (!LCD_BMP_Read_uint32_t(&(bmp->Header.ColorsRequired), f))
		return BMP_IO_ERROR;

	return BMP_OK;
}

/**
 * @brief		Reads a little-endian unsigned int from the file.
 * @param[out]	x		Data buffer
 * @param[in]	f		Pointer to the file structure.
 * @return		Returns non-zero on success.
 */
static int LCD_BMP_Read_uint32_t(uint32_t *x, FIL *f) {

	UINT bytesread;
	uint8_t little[4]; /* BMPs use 32 bit ints */

	if (x == NULL || f == NULL) {
		return 0;
	}

	if ((f_read(f, little, 4, &bytesread) != FR_OK) || (bytesread != 4)) {
		return 0;
	}

	*x = (little[3] << 24 | little[2] << 16 | little[1] << 8 | little[0]);

	return 1;
}

/**
 * @brief		Reads a little-endian unsigned short int from the file.
 * @param[out]	x		Data buffer
 * @param[in]	f		Pointer to the file structure.
 * @return		Returns non-zero on success.
 */
static int LCD_BMP_Read_uint16_t(uint16_t *x, FIL *f) {

	UINT bytesread;
	uint8_t little[2]; /* BMPs use 16 bit shorts */

	if (x == NULL || f == NULL) {
		return 0;
	}

	if ((f_read(f, little, 2, &bytesread) != FR_OK) || (bytesread != 2)) {
		return 0;
	}

	*x = (little[1] << 8 | little[0]);

	return 1;
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
