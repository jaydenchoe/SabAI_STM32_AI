/**
  ******************************************************************************
  * @file    se_fwimg.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions used by the bootloader
  *          to manage Secure Firmware Update functionalities.
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
#include "se_def.h"
#include "se_fwimg.h"
#include "se_low_level.h"
#include "se_exception.h"
#include "string.h"
#ifdef ENABLE_IMAGE_STATE_HANDLING
#include "se_user_application.h"
#endif /* ENABLE_IMAGE_STATE_HANDLING */

/** @addtogroup SE
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @defgroup SE_IMG SE Firmware Image
  * @brief Code used to handle the Firmware Images.
  * This contains functions used by the bootloader.
  * @{
  */


/** @defgroup SE_IMG_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Write in Flash protected area
  * @param  pDestination pointer to destination area in Flash
  * @param  pSource pointer to input buffer
  * @param  Length number of bytes to be written
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_IMG_Write(void *pDestination, const void *pSource, uint32_t Length)
{
  SE_ErrorStatus ret;
  uint32_t areabegin = (uint32_t)pDestination;
  uint32_t areaend = areabegin + Length - 1U;

  /* The header of slot #0 is placed in protected area */
  if ((areabegin >= SFU_IMG_SLOT_0_REGION_BEGIN_VALUE) &&
     (areaend < (SFU_IMG_SLOT_0_REGION_BEGIN_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    /* Writing in protected memory */
    ret = SE_LL_FLASH_Write(pDestination, pSource, Length);
  }
  else
  {
    /* Abnormal case: this primitive should not be used to access this address */
    ret = SE_ERROR;
  }
  return ret;
}

/**
  * @brief  Read from Flash protected area
  * @param  pDestination pointer to output buffer
  * @param  pSource pointer to source area in Flash
  * @param  Length number of bytes to be read
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_IMG_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  SE_ErrorStatus ret;
  uint32_t areabegin = (uint32_t)pSource;
  uint32_t areaend = areabegin + Length - 1U;
  /* The header of slot #0 is placed in protected area */
 if ((areabegin >= SFU_IMG_SLOT_0_REGION_BEGIN_VALUE) &&
     (areaend < (SFU_IMG_SLOT_0_REGION_BEGIN_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    /* Accessing protected memory */
    ret = SE_LL_FLASH_Read(pDestination, pSource, Length);
  }
  else
  {
    /* Abnormal case: this primitive should not be used to access this address */
    ret = SE_ERROR;
  }
  return ret;
}

/**
  * @brief  Erase a Flash protected area
  * @param  pDestination pointer to destination area in Flash
  * @param  Length number of bytes to be erased
  * @retval SE_SUCCESS if successful, otherwise SE_ERROR
  */

SE_ErrorStatus SE_IMG_Erase(void *pDestination, uint32_t Length)
{
  SE_ErrorStatus ret;
  uint32_t areabegin = (uint32_t)pDestination;
  uint32_t areaend = areabegin + Length - 1U;
  /* The header of slot #0 is placed in protected area */
  if ((areabegin >= SFU_IMG_SLOT_0_REGION_BEGIN_VALUE) &&
      (areaend < (SFU_IMG_SLOT_0_REGION_BEGIN_VALUE + SFU_IMG_IMAGE_OFFSET)))
  {
    /* Accessing protected memory */
    ret = SE_LL_FLASH_Erase(pDestination, Length);
  }
  else
  {
    /* Abnormal case: this primitive should not be used to access this address */
    ret = SE_ERROR;
  }
  return ret;
}

#ifdef ENABLE_IMAGE_STATE_HANDLING
/**
  * @brief Service called by the Bootloader to Set the Active Firmware State.
  * The Bootloader allowed state transitions are:
  * FWIMG_STATE_NEW to FWIMG_STATE_SELFTEST
  * FWIMG_STATE_SELFTEST to FWIMG_STATE_INVALID
  * FWIMG_STATE_VALID to FWIMG_STATE_INVALID
  * @param p_FwState Active Firmware State structure containing the state that will be set.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SE_ErrorStatus SE_SetActiveFwState(SE_FwStateTypeDef *p_FwState)
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
  if( (e_ret_status != SE_SUCCESS) || (eCurrentState == FWIMG_STATE_UNKNOWN) )
  {
    return SE_ERROR;
  }
  
  /* Bootloader can change state
   * NEW to SELFTEST
   * SELFTEST to INVALID
   * VALID to INVALID 
   */
  if((eCurrentState == FWIMG_STATE_NEW) && (*p_FwState != FWIMG_STATE_SELFTEST))
  {
    return SE_ERROR;
  }
  /* Bootloader can only transition from FWIMG_STATE_SELFTEST to FWIMG_STATE_INVALID
   * A transition from FWIMG_STATE_SELFTEST to FWIMG_STATE_VALID should be done by
   * application after selft test passes.
   */
  if((eCurrentState == FWIMG_STATE_SELFTEST) && (*p_FwState != FWIMG_STATE_INVALID))
  {
    return SE_ERROR;
  }
  if((eCurrentState == FWIMG_STATE_VALID) && (*p_FwState != FWIMG_STATE_INVALID))
  {
    return SE_ERROR;
  }
  
  switch(*p_FwState)
  {
    case FWIMG_STATE_INVALID:
    {
      /* state transition 32*0xFF, 32*0xFF, 32*0x00 -> 32*0x00, 32*0x00, 32*0x00 Selftest -> Invalid
                       or 32*0xFF, 32*0x00, 32*0x00 -> 32*0x00, 32*0x00, 32*0x00 Valid    -> Invalid */
      e_ret_status = SE_LL_FLASH_Write((void*) pfw_image_header->FwImageState[0] , ZerosBuffer, sizeof(ZerosBuffer));
      if(e_ret_status == SE_SUCCESS)
      {
        if(eCurrentState == FWIMG_STATE_SELFTEST)
        {
          e_ret_status = SE_LL_FLASH_Write((void*) pfw_image_header->FwImageState[1] , ZerosBuffer, sizeof(ZerosBuffer));
        }
      }
      break;
    }
    case FWIMG_STATE_SELFTEST:
    {
      /* state transition 32*0xFF, 32*0xFF, 32*0xFF -> 32*0xFF, 32*0xFF, 32*0x00 New -> Selftest */
      e_ret_status = SE_LL_FLASH_Write((void*) pfw_image_header->FwImageState[2] , ZerosBuffer, sizeof(ZerosBuffer));
      break;
    }
    default:
    {
      e_ret_status = SE_ERROR;
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
