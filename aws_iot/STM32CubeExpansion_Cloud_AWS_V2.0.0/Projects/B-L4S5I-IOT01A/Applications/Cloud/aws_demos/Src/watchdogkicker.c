/**
  ******************************************************************************
  * @file    watchdogkicker.c
  * @author  AME AMS IoT Application Team
  * @brief   Management of independant watchdog timer.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "watchdogkicker.h"
#include "aws_iot_ota_agent.h"
#include "aws_iot_ota_pal.h"
#include "stm32l4xx_hal.h"

/* Private typedef -----------------------------------------------------------*/

/* Private defines ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void prvWatchDogKickerTask( void * pvParameters );

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  The WatchDog Kicker Task.
  * @note   The watchdog timer is configured and started by the associated
  *         bootloader
  * @param  In: usStackSize  Task stack size
  * @param  In: uxPriority   Task priority
  * @retval  pdPASS:  Success.
            -pdFAIL:  Failure.
  */
static void prvWatchDogKickerTask( void * pvParameters )
{
  IWDG_HandleTypeDef hiwdg1;
  hiwdg1.Instance = IWDG;

  /* When the running image is in self test mode we kick the watchdog
   * a maximum number of MAXIMUM_SELFTEST_KICKS times
   */
  uint32_t SelfTestKicks = MAXIMUM_SELFTEST_KICKS;
  OTA_PAL_ImageState_t ImageState = eOTA_PAL_ImageState_Unknown;

  for( ; ; )
  {
    /* Read the image state if the image state is unknown or pending commit,
       otherwise we no longer need to read the state as it will not change */
    if((eOTA_PAL_ImageState_PendingCommit == ImageState) || (eOTA_PAL_ImageState_Unknown == ImageState))
    {
      ImageState = prvPAL_GetPlatformImageState();
    }

    if(eOTA_PAL_ImageState_PendingCommit == ImageState)
    {
      /* Image is in self-test / commit-pending state
       * kick the watchdog a max of MAXIMUM_SELFTEST_KICKS times and then
       * let it expire.
       */
      if(SelfTestKicks > 0)
      {
        HAL_IWDG_Refresh( &hiwdg1 );
        SelfTestKicks--;
      }
    }
    else
    {
      /* Image is not in self-test / commit-pending mode
       * kick the watchdog timer */
      HAL_IWDG_Refresh( &hiwdg1 );
      SelfTestKicks = MAXIMUM_SELFTEST_KICKS;
    }
    /* wait for next refresh */
    vTaskDelay(WATCHDOG_KICK_INTERVAL);
  }
}

/* Public functions ---------------------------------------------------------*/
/**
  * @brief  Initialize the WatchDog Kicker Task.
  * @note   The watchdog timer is configured and started by the associated
  *         bootloader
  * @param  In: usStackSize  Task stack size
  * @param  In: uxPriority   Task priority
  * @retval  pdPASS:  Success.
            -pdFAIL:  Failure.
  */
BaseType_t xWatchDogKickerTaskInitialize( uint16_t usStackSize,
                                          UBaseType_t uxPriority)
{
    BaseType_t xReturn = pdFAIL;
    IWDG_HandleTypeDef hiwdg1;
    hiwdg1.Instance = IWDG;

    /* Kick the watchdog once asap */
    HAL_IWDG_Refresh( &hiwdg1 );

    /* Create the periodic kicker task */
    if( xTaskCreate( prvWatchDogKickerTask, "IWDKicker", usStackSize, NULL, uxPriority, NULL ) == pdPASS )
    {
      xReturn = pdPASS;
    }

    return xReturn;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
