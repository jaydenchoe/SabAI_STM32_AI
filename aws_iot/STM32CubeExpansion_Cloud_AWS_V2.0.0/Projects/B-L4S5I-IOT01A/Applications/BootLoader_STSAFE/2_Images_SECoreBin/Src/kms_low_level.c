/**
  ******************************************************************************
  * @file    kms_low_level.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
  *          module Low Level access to Flash, CRC...
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "kms.h"
#include "kms_low_level.h"
#include "se_low_level.h"
#if defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_sbsfu.h"
#endif /* __ICCARM__ || __GNUC__ */ 
 

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_LL Low Level access
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Read data from flash and store into buffer
  * @note   Used to access encrypted blob in flash.
  * @param  pDestination buffer to store red data
  * @param  pSource flash aread to read from
  * @param  Length amount of data to read from flash
  * @retval Operation status
  */
CK_RV KMS_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  SE_ErrorStatus seStatus;
  seStatus = SE_LL_FLASH_Read(pDestination, pSource, Length);
  return (seStatus == SE_SUCCESS) ? CKR_OK : CKR_GENERAL_ERROR;
}

/**
  * @brief  Returns encrypted blob storage address in flash
  * @retval Encrypted blob address
  */
uint32_t KMS_LL_FLASH_GetBlobDownloadAddress(void)
{
  return (uint32_t)KMS_BLOB_DOWNLOAD_ADDRESS;
}

/**
  * @brief  Initialize CRC peripheral
  * @retval Operation status
  */
CK_RV KMS_LL_CRC_Init(void)
{
  /* CRC Peripheral clock enable */
  __HAL_RCC_CRC_CLK_ENABLE();
  /* CRC Peripheral Reset */
  __HAL_RCC_CRC_FORCE_RESET();
  __HAL_RCC_CRC_RELEASE_RESET();

  return CKR_OK;
}

#ifdef KMS_CHECK_PARAMS

/* Remove compilation optimization */
#if defined(__ICCARM__)
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma O0
#else
__attribute__((optimize("O0")))
#endif /* __ICCARM__ */

/**
  * @brief  Check if given buffer in inside secure enclave (RAM or Flash, NVM_Storgae)
  *              If it is in Enclave area, then generate a RESET.
  * @param  pBuffer Buffer address
  * @param  ulSize  Buffer size
  * @retval void
  */
void KMS_LL_IsBufferInSecureEnclave(void * pBuffer, uint32_t ulSize)
{
  uint32_t addr_start = (uint32_t)pBuffer;
  uint32_t addr_end = addr_start + ulSize - 1U;

  /* If pBuffer is NULL, simply return FALSE */
  if (pBuffer == NULL)
  {
    return;
  }
  /* Check if address range is within secure enclave  */
  /*     Checking RAM Need to add test to protect on NVM_STORAGE */
   if ((ulSize != 0U) && ((((addr_start >= SE_REGION_SRAM1_START) && (addr_start <= SE_REGION_SRAM1_END))
                        || ((addr_end   >= SE_REGION_SRAM1_START) && (addr_end   <= SE_REGION_SRAM1_END))
                        || ((addr_start >= SE_CODE_REGION_ROM_START) && (addr_start <= SE_CODE_REGION_ROM_END))
                        || ((addr_end   >= SE_CODE_REGION_ROM_START) && (addr_end   <= SE_CODE_REGION_ROM_END))
                        || ((addr_start >= KMS_DATASTORAGE_START) && (addr_start <= KMS_DATASTORAGE_END))
                        || ((addr_end   >= KMS_DATASTORAGE_START) && (addr_end   <= KMS_DATASTORAGE_END)))))
     
     
  {
    /* Could be an attack ==> Reset */
    NVIC_SystemReset();
  }

   /* Double Check to avoid basic fault injection */
   if ((ulSize != 0U) && ((((addr_start >= SE_REGION_SRAM1_START) && (addr_start <= SE_REGION_SRAM1_END))
                        || ((addr_end   >= SE_REGION_SRAM1_START) && (addr_end   <= SE_REGION_SRAM1_END))
                        || ((addr_start >= SE_CODE_REGION_ROM_START) && (addr_start <= SE_CODE_REGION_ROM_END))
                        || ((addr_end   >= SE_CODE_REGION_ROM_START) && (addr_end   <= SE_CODE_REGION_ROM_END))
                        || ((addr_start >= KMS_DATASTORAGE_START) && (addr_start <= KMS_DATASTORAGE_END))
                        || ((addr_end   >= KMS_DATASTORAGE_START) && (addr_end   <= KMS_DATASTORAGE_END)))))
     
     
  {
    /* Could be an attack ==> Reset */
    NVIC_SystemReset();
  }
  
  return;
}
#endif /* KMS_CHECK_PARAMS */

