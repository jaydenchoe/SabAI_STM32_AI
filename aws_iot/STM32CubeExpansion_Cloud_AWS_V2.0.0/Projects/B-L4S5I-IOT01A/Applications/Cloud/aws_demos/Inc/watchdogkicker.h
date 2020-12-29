/**
  ******************************************************************************
  * @file           : watchdogkicker.h
  * @brief          : Header for watchdogkicker.c file.
  *                   This file contains the defines/config for the watchdog
  *                   kicker task.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WATCHDOGKICKER_H
#define __WATCHDOGKICKER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
BaseType_t xWatchDogKickerTaskInitialize( uint16_t usStackSize,
                                          UBaseType_t uxPriority);

/* Private defines -----------------------------------------------------------*/
#define WATCHDOG_KICK_INTERVAL 2000 /* ticks between watchdog timer refresh  */
#define MAXIMUM_SELFTEST_KICKS 45   /* Max number of watchdog refreshes when */
                                    /* the running image is in the selftest  */
                                    /* commit pending state */


#ifdef __cplusplus
}
#endif

#endif /* __WATCHDOGKICKER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
