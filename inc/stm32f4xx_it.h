#ifndef __STM32F4xx_IT_H__
#define __STM32F4xx_IT_H__
/**
 *****************************************************************************
 * @addtogroup	Project
 * @{
 *
 * @file		stm32f4xx_it.h
 * @version		1.0
 * @date		2014-11-21
 * @author		rct1
 *
 * @brief		Project Interrupt Request Handler
 *
 *****************************************************************************
 * @copyright
 * @{
 * Copyright &copy; 2013, Bern University of Applied Sciences.
 * All rights reserved.
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
 * @}
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <stm32f4xx.h>				/* Processor STM32F407IG				*/

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void EXTI0_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __STM32F4xx_IT_H__ */
