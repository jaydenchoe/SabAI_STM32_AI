/**
  ******************************************************************************
  * @file    se_user_application.c
  * @author  MCD Application Team
  * @brief   Secure Engine USER APPLICATION module.
  *          This file is a placeholder for the code dedicated to the user application.
  *          These services are used by the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#include "se_user_application.h"
#include "se_low_level.h"
#include "string.h"

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @defgroup  SE_APPLI SE Code for Application
  * @brief This file is a placeholder for the code dedicated to the user application.
  * It contains the code written by the end user.
  * The code used by the application to handle the Firmware images is located in se_fwimg.c.
  * @{
  */

/** @defgroup SE_APPLI_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief Service called by the User Application to retrieve the Active Firmware Info.
  * @param p_FwInfo Active Firmware Info structure that will be filled.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_APPLI_GetActiveFwInfo(SE_APP_ActiveFwInfo_t *p_FwInfo)
{
  SE_ErrorStatus e_ret_status;
  uint8_t buffer[SE_FW_HEADER_METADATA_LEN];  /* to read FW metadata from FLASH */
  SE_FwRawHeaderTypeDef *pfw_image_header;  /* FW metadata */

  /* Check the pointer allocation */
  if (NULL == p_FwInfo)
  {
    return SE_ERROR;
  }

  /*
   * The Firmware Information is available in the header of the slot #0.
   */
  e_ret_status = SE_LL_FLASH_Read(buffer, SFU_IMG_SLOT_0_REGION_BEGIN, sizeof(buffer));
  if (e_ret_status != SE_ERROR)
  {
    pfw_image_header = (SE_FwRawHeaderTypeDef *)(uint32_t)buffer;

    /*
     * We do not check the header validity.
     * We just copy the information.
     */
    p_FwInfo->ActiveFwVersion = pfw_image_header->FwVersion;
    p_FwInfo->ActiveFwSize = pfw_image_header->FwSize;
  }

  return e_ret_status;
}

#ifdef ENABLE_IMAGE_STATE_HANDLING
/**
  * @brief Service called by the User Application to retrieve the Active Firmware State.
  * @param p_FwState Active Firmware State structure that will be filled.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_APPLI_GetActiveFwState(SE_FwStateTypeDef *p_FwState)
{
  SE_ErrorStatus e_ret_status = SE_ERROR;
  uint8_t buffer[3*32];  /* to read FW State from FLASH */
  SE_FwRawHeaderTypeDef *pfw_image_header = (SE_FwRawHeaderTypeDef*) SFU_IMG_SLOT_0_REGION_BEGIN;  /* FW metadata */
  const uint8_t OnesBuffer[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,\
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,\
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,\
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,};

  const uint8_t ZerosBuffer[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint32_t TempState = 0x00; /* checking the image state in the FLASH (header of slot #0) */
  /* Check the pointer allocation */
  if (NULL == p_FwState)
  {
    return SE_ERROR;
  }

  /*
   * The Firmware State is available in the header of the slot #0.
   */
  e_ret_status = SE_LL_FLASH_Read(buffer, pfw_image_header->FwImageState, sizeof(buffer));

  /*
     * We do not check the header validity.
     * We just copy the information.
  */

  /* In the header (SE_FwRawHeaderTypeDef.FwImageState), the image states are encoded as follows:
   * FWIMG_STATE_INVALID : 3*32 0x00
   * FWIMG_STATE_VALID   : 1*32 0xFF, 2*32 0x00
   * FWIMG_STATE_SELFTEST: 2*32 0xFF, 1*32 0x00
   * FWIMG_STATE_NEW     : 3*32 0xFF
   * FWIMG_STATE_UNKNOWN : any other values
   */
  if (e_ret_status == SE_SUCCESS)
  {

    /* Check FwImageState[0] */
    if(memcmp(&buffer[0], ZerosBuffer, sizeof(ZerosBuffer))==0)
    {
      TempState &= ~0x1;
    }
    else
    {
      if(memcmp(&buffer[0], OnesBuffer, sizeof(OnesBuffer))==0)
      {
        TempState |= 0x1;
      }
      else
      {
        *p_FwState = FWIMG_STATE_UNKNOWN;
        return SE_ERROR;
      }
    }

    /* Check FwImageState[1] */
    if(memcmp(&buffer[32], ZerosBuffer, sizeof(ZerosBuffer))==0)
    {
      TempState &= ~0x2;
    }
    else
    {
      if(memcmp(&buffer[32], OnesBuffer, sizeof(OnesBuffer))==0)
      {
        TempState |= 0x2;
      }
      else
      {
        *p_FwState = FWIMG_STATE_UNKNOWN;
        return SE_ERROR;
      }
    }

    /* Check FwImageState[2] */
    if(memcmp(&buffer[64], ZerosBuffer, sizeof(ZerosBuffer))==0)
    {
      TempState &= ~0x4;
    }
    else
    {
      if(memcmp(&buffer[64], OnesBuffer, sizeof(OnesBuffer))==0)
      {
        TempState |= 0x4;
      }
      else
      {
        *p_FwState = FWIMG_STATE_UNKNOWN;
        return SE_ERROR;
      }
    }

    switch (TempState)
    {
      case 0: // b000 invalid
      {
        *p_FwState = FWIMG_STATE_INVALID;
        e_ret_status = SE_SUCCESS;
        break;
      }
    case 1: // b001 valid
      {
        *p_FwState = FWIMG_STATE_VALID;
        e_ret_status = SE_SUCCESS;
        break;
      }
    case 3: // b011 self test
      {
        *p_FwState = FWIMG_STATE_SELFTEST;
        e_ret_status = SE_SUCCESS;
        break;
      }
    case 7: // b111 new
      {
        *p_FwState = FWIMG_STATE_NEW;
        e_ret_status = SE_SUCCESS;
        break;
      }
    default:
      {
        *p_FwState = FWIMG_STATE_UNKNOWN;
        e_ret_status = SE_SUCCESS;
        break;
      }
    }
  }
  return e_ret_status;
}

/**
  * @brief Service called by the User Application to Set the Active Firmware State.
  * The application can only change an image from SELFTEST state to VALID or INVALID
  * depending on the result of the selftest.
  * @param p_FwState Active Firmware State structure containing the state that will be set.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_APPLI_SetActiveFwState(SE_FwStateTypeDef *p_FwState)
{
  SE_ErrorStatus e_ret_status = SE_ERROR;
  SE_FwRawHeaderTypeDef *pfw_image_header = (SE_FwRawHeaderTypeDef*) SFU_IMG_SLOT_0_REGION_BEGIN;  /* FW metadata */

  const uint8_t ZerosBuffer[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


  SE_FwStateTypeDef eCurrentState = FWIMG_STATE_UNKNOWN;
  SE_FwStateTypeDef eTempCurrentState[3] = {FWIMG_STATE_UNKNOWN, FWIMG_STATE_UNKNOWN, FWIMG_STATE_UNKNOWN};

  /* Check the pointer allocation */
  if (NULL == p_FwState)
  {
    return SE_ERROR;
  }

  /*
   * The Firmware State is available in the header of the slot #0.
   * In the header (SE_FwRawHeaderTypeDef.FwImageState), these states are encoded as follows:
   * FWIMG_STATE_INVALID : 3*32 0x00
   * FWIMG_STATE_VALID   : 1*32 0xFF, 2*32 0x00
   * FWIMG_STATE_SELFTEST: 2*32 0xFF, 1*32 0x00
   * FWIMG_STATE_NEW     : 3*32 0xFF
   * FWIMG_STATE_UNKNOWN : any other values
   */

  /* Read current state multiple times to thwart glitch attacks */
  e_ret_status = SE_APPLI_GetActiveFwState(&eTempCurrentState[0]);
  if(SE_SUCCESS == e_ret_status)
  {
    e_ret_status = SE_APPLI_GetActiveFwState(&eTempCurrentState[1]);
  }
  if(SE_SUCCESS == e_ret_status)
  {
    e_ret_status = SE_APPLI_GetActiveFwState(&eTempCurrentState[2]);
  }
  if(SE_SUCCESS == e_ret_status)
  {
    if((eTempCurrentState[0]==eTempCurrentState[1]) && (eTempCurrentState[0]==eTempCurrentState[2]))
    {
      eCurrentState = eTempCurrentState[0];
    }
  }

  if( (e_ret_status == SE_ERROR) || (eCurrentState == FWIMG_STATE_UNKNOWN) )
  {
    return SE_ERROR;
  }

  /* Application can only change SELFTEST to VALID or INVALID */
  if(eCurrentState != FWIMG_STATE_SELFTEST)
  {
    return SE_ERROR;
  }

  switch(*p_FwState)
  {
    case FWIMG_STATE_VALID:
    {
      /* state transition 32*0xFF, 32*0xFF, 32*0x00 -> 32*0xFF, 32*0x00, 32*0x00 */
      e_ret_status = SE_LL_FLASH_Write((void*) pfw_image_header->FwImageState[1] , ZerosBuffer, sizeof(ZerosBuffer));
      break;
    }
    case FWIMG_STATE_INVALID:
    {
      /* state transition 32*0xFF, 32*0xFF, 32*0x00 -> 32*0x00, 32*0x00, 32*0x00 */
      e_ret_status = SE_LL_FLASH_Write((void*) pfw_image_header->FwImageState[0] , ZerosBuffer, sizeof(ZerosBuffer));
      if(e_ret_status == SE_SUCCESS)
      {
        e_ret_status = SE_LL_FLASH_Write((void*) pfw_image_header->FwImageState[1] , ZerosBuffer, sizeof(ZerosBuffer));
      }
      break;
    }
    default:
    {
      e_ret_status = SE_ERROR;
      break;
    }
  }
  return e_ret_status;
}

#endif /* ENABLE_IMAGE_STATE_HANDLING */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
