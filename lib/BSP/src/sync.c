/**
 *****************************************************************************
 * @addtogroup 	FatFs FatFs
 * @{
 * @defgroup	FatFs_Sync FreeRTOS Task Synchronization
 * @brief		FatFs synchronize functions.
 * @{
 *
 * @file		sync.c
 * @version		1.0
 * @date		2013-10-08
 * @author		rct1
 *
 * @brief		FatFs synchronize functions.\n
 *				Add synchronize functions to the FatFs. This will add the
 *				reentrant feature to the module.
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
#include <_ansi.h>					/* support for ANSI environments		*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/
#include <stddef.h>					/* Standard defines						*/
#include <ff.h>						/* Fat file system						*/

#if _FS_REENTRANT

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/
_VOLATILE static uint8_t sync[_VOLUMES];

/*----- Implementation -----------------------------------------------------*/
__attribute__((naked)) static uint32_t DisableGlobalInterrupt(void) {
	__asm volatile
	(
			"	mrs r0, basepri											\n"
			"	mov r1, %0												\n"
			"	msr basepri, r1											\n"
			"	bx lr													\n"
			:: "i" (255) : "r0", "r1"
	);

	/* This return will not be reached but is necessary to prevent compiler
	 * warnings.															*/
	return 0;
}

__attribute__((naked)) static void EnableGlobalInterrupt(
		uint32_t ulNewMaskValue) {
	__asm volatile
	(
			"	msr basepri, r0										\n"
			"	bx lr												\n"
			:::"r0"
	);

	/* Just to avoid compiler warnings. */
	(void) ulNewMaskValue;
}

/**
 *****************************************************************************
 * \brief		Create a sync object.
 *
 * \param[in]	vol		Not used.
 * \param[in]	mutex	A pointer to the mutex.
 *
 * \return		0 if mutex has been created, else !0.
 *****************************************************************************
 */
__attribute__((weak)) int ff_cre_syncobj(BYTE vol, _SYNC_t *mutex) {

	_VOLATILE uint8_t **value = (_VOLATILE uint8_t **)mutex;

	/* Check if the drive number is valid */
	if (vol >= _VOLUMES) {
		return FR_INVALID_DRIVE;
	}

	/* Get current fs object */
	*value = &sync[vol];

	return (*mutex != NULL);
}

/**
 *****************************************************************************
 * \brief		Delete the sync object.
 *
 * \param[in]	mutex	The mutex to delete.
 *
 * \return		1 if mutex has been deleted.
 *****************************************************************************
 */
__attribute__((weak)) int ff_del_syncobj(_SYNC_t mutex) {

	uint8_t *value = (uint8_t *)mutex;
	uint32_t interrupts;

	interrupts = DisableGlobalInterrupt();
	*value = 0;
	EnableGlobalInterrupt(interrupts);

	return !0;
}

/**
 *****************************************************************************
 * \brief		Request the access to the mutex.
 *
 * \param[in]	mutex	The precreated mutex.
 *
 * \return		0 if access is granted, else !0.
 *****************************************************************************
 */
__attribute__((weak)) int ff_req_grant(_SYNC_t mutex) {

	uint8_t *value = (uint8_t *)mutex;
	uint32_t interrupts;
	uint8_t granted = 0;

	interrupts = DisableGlobalInterrupt();
	if (!(*value)) {
		*value = 1;
		granted = 1;
	}
	EnableGlobalInterrupt(interrupts);

	return granted;
}

/**
 *****************************************************************************
 * \brief		Release the access.
 *
 * \param[in]	mutex	The precreated and requested mutex.
 *****************************************************************************
 */
__attribute__((weak)) void ff_rel_grant(_SYNC_t mutex) {

	uint8_t *value = (uint8_t *)mutex;
	uint32_t interrupts;

	interrupts = DisableGlobalInterrupt();
	*value = 0;
	EnableGlobalInterrupt(interrupts);
}

#endif /* _FS_REENTRANT */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 */

/**
 * @}
 */
