/**
  ******************************************************************************
  * @file    sfu_kms.h
  * @author  MCD Application Team
  * @brief   This file contains the KMS Blob service (SFU_KMS_ENABLED) API definitions.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SFU_KMS_H
#define SFU_KMS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "se_def.h"
#include "sfu_def.h"

/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup SFU_CORE SBSFU Application
  * @{
  */

/** @addtogroup SFU_IMG
  * @{
  */


/** @addtogroup SFU_IMG_SERVICES
  * @{
  */

/** @defgroup SFU_IMG_Exported_Types Exported Types for FWIMG services
  * @brief Exported Types for FWIMG services
  * @{
  */

/**
  * @brief  SFU_KMS Installation Status Type Definition
  */
typedef enum
{
  SFU_KMS_INIT_OK = 0x0U,                /*!< SFU Firmware Image Handling (FWIMG) Init OK */
  SFU_KMS_INIT_SLOTS_SIZE_ERROR,         /*!< error related to slots size */
  SFU_KMS_INIT_SWAP_SETTINGS_ERROR,      /*!< error related to swap settings */
  SFU_KMS_INIT_FLASH_CONSTRAINTS_ERROR,  /*!< error related to flash constraints */
  SFU_KMS_INIT_CRYPTO_CONSTRAINTS_ERROR, /*!< error related to crypto constraints */
  SFU_KMS_INIT_ERROR                     /*!< Init is FAILED: unspecified error */
} SFU_KMS_InitStatusTypeDef;

/**
  * @brief  SFU_KMS Blob Installation State Type Definition
  */
typedef enum
{
  SFU_KMS_BLOBIMAGE_TO_INSTALL = 0x0U,   /*!< There is a KMS blob to be installed */
  SFU_KMS_NO_BLOBUPDATE,                 /*!< No KMS blob installation pending */
} SFU_KMS_BlobInstallStateTypeDef;


/**
  * @}
  */

/** @addtogroup SFU_IMG_Exported_Functions FW Images Handling Services
  * @{
  */

/** @addtogroup SFU_IMG_Initialization_Functions FW Images Handling Init. Functions
  * @{
  */

/**
  * @}
  */

/** @addtogroup SFU_IMG_Service_Functions FW Images Handling Services Functions
  * @{
  */


/** @addtogroup SFU_KMS_Functions_NewImg New image installation services
  * @{
  */
SFU_KMS_BlobInstallStateTypeDef SFU_KMS_CheckPendingBlobInstallation(void);
uint8_t *SFU_KMS_GetBlobInstallInfo(void);

SFU_ErrorStatus SFU_KMS_BlobToInstall(void);
SFU_ErrorStatus SFU_KMS_EraseBlob(void);

/**
  * @}
  */


/** @addtogroup SFU_KMS_Service_Functions_CurrentImage Active Firmware services (current image)
  * @{
  */
SFU_ErrorStatus SFU_KMS_InvalidateCurrentFirmware(void);
SFU_ErrorStatus SFU_KMS_VerifyActiveImgMetadata(void);
SFU_ErrorStatus SFU_KMS_VerifyActiveImg(void);
SFU_ErrorStatus SFU_KMS_LaunchActiveImg(void);
int32_t SFU_KMS_GetActiveFwVersion(void);
SFU_ErrorStatus SFU_KMS_HasValidActiveFirmware(void);
SFU_ErrorStatus SFU_KMS_Validation(uint8_t *pHeader);
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

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* SFU_KMS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

