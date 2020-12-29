/**
  ******************************************************************************
  * @file    kms_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
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

#ifndef KMS_LOW_LEVEL_H
#define KMS_LOW_LEVEL_H

/* Includes ------------------------------------------------------------------*/
#include "sfu_fwimg_regions.h"

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_LL Low Level access
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @addtogroup KMS_LL_Exported_Constants Exported Constants
  * @{
  */
/**
  * @brief The address in flash where the encrypted blob is downloaded
  * @note  This value is used by @ref KMS_LL_FLASH_GetBlobDownloadAddress.
  *        So , depending on @ref KMS_LL_FLASH_GetBlobDownloadAddress
  *        implementation, this define may be not needed
  */
#define KMS_BLOB_DOWNLOAD_ADDRESS        (SFU_IMG_SLOT_DWL_REGION_BEGIN)



/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/

/** @addtogroup KMS_LL_Exported_Functions Exported Functions
  * @{
  */
CK_RV KMS_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length);
uint32_t KMS_LL_FLASH_GetBlobDownloadAddress(void);
CK_RV KMS_LL_CRC_Init(void);
#ifdef KMS_CHECK_PARAMS
void KMS_LL_IsBufferInSecureEnclave(void * pBuffer, uint32_t ulSize);
#endif /* KMS_CHECK_PARAMS */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* KMS_LOW_LEVEL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
