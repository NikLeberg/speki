/**
 *****************************************************************************
 * @addtogroup  Project
 * @{
 *
 * @file        stm32f4xx_it.h
 * @version     1.0
 * @date        2014-11-21
 * @author      rct1
 *
 * @brief       Project Interrupt Request Handler
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
#include <stm32f4xx.h>              /* Processor STM32F407IG                */
#include <carme.h>                  /* CARME Module                         */
#include <carme_io1.h>              /* CARME IO1 Module                     */
#include <can.h>                    /* CARME CAN Module                     */
#include "stm32f4xx_it.h"

/*----- Macros -------------------------------------------------------------*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/
extern void Default_Handler(void);

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * @brief       This function handles NMI exception.
 *
 * @return      None
 *****************************************************************************
 */
void NMI_Handler(void)
{
}

/**
 *****************************************************************************
 * @brief       This function handles Hard Fault exception.
 *
 * @return      None
 *****************************************************************************
 */
void HardFault_Handler(void)
{
        /* Go to infinite loop when Hard Fault exception occurs */
        Default_Handler();
}

/**
 *****************************************************************************
 * @brief       This function handles Memory Manage exception.
 *
 * @return      None
 *****************************************************************************
 */
void MemManage_Handler(void)
{
        /* Go to infinite loop when Memory Manage exception occurs */
        while (1) {
        }
}

/**
 *****************************************************************************
 * @brief       This function handles Bus Fault exception.
 *
 * @return      None
 *****************************************************************************
 */
void BusFault_Handler(void)
{
        /* Go to infinite loop when Bus Fault exception occurs */
        Default_Handler();
}

/**
 *****************************************************************************
 * @brief       This function handles Usage Fault exception.
 *
 * @return      None
 *****************************************************************************
 */
void UsageFault_Handler(void)
{
        /* Go to infinite loop when Usage Fault exception occurs */
        Default_Handler();
}

/**
 *****************************************************************************
 * @brief       This function handles Debug Monitor exception.
 *
 * @return      None
 *****************************************************************************
 */
void DebugMon_Handler(void)
{
}

/**
 *****************************************************************************
 * @brief       This function handles the EXTI Lines 9:5.
 *
 * @return      None
 *****************************************************************************
 */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON3_PIN)) != RESET) {
        /* CARME IO1 Button3 Interrupt Handler */
        EXTI_ClearITPendingBit(CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON3_PIN));
    }
}

/**
 *****************************************************************************
 * @brief       This function handles the EXTI Lines 9:5.
 *
 * @return      None
 *****************************************************************************
 */
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON0_PIN)) != RESET) {
        /* CARME IO1 Button0 Interrupt Handler */
        EXTI_ClearITPendingBit(CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON0_PIN));
    }

    if (EXTI_GetITStatus(CARME_GPIO_TO_EXTILINE(CARME_CAN_nCAN_INT_PIN)) != RESET) {
        CARME_CAN_Interrupt_Handler();
        EXTI_ClearITPendingBit(CARME_GPIO_TO_EXTILINE(CARME_CAN_nCAN_INT_PIN));
    }
}

/**
 *****************************************************************************
 * @brief       This function handles the EXTI Lines 15:10.
 *
 * @return      None
 *****************************************************************************
 */
void EXTI15_10_IRQHandler(void)
{

    if (EXTI_GetITStatus(CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON1_PIN)) != RESET) {
        /* CARME IO1 Button1 Interrupt Handler */
        EXTI_ClearITPendingBit(CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON1_PIN));
    }

    if (EXTI_GetITStatus(CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON2_PIN)) != RESET) {
        /* CARME IO1 Button2 Interrupt Handler */
        EXTI_ClearITPendingBit(CARME_GPIO_TO_EXTILINE(CARME_IO1_BUTTON2_PIN));
    }
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
