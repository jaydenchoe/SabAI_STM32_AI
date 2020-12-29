/**
  ******************************************************************************
  * @file    sfu_kms.c
  * @author  MCD Application Team
  * @brief   This file provides set of functions to manage the KMS Images.
  *          This file contains the kms services the bootloader (sfu_boot) can
  *             use to deal with the KMS Blob images handling.
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

#define SFU_KMS_C


#ifdef KMS_ENABLED

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#include "sfu_low_level_flash.h"
#include "sfu_low_level_security.h"
#include "sfu_low_level.h"
#include "sfu_fwimg_services.h"
#include "sfu_fwimg_internal.h"
#include "sfu_trace.h"
#include "sfu_kms.h"
#include "sfu_fwimg_internal.h" /* required to re-use fw_image_header_to_test (and SWAP_HDR) in the KMS blob 
                                   installation procedure */
#include "sfu_fwimg_regions.h"  /* required because KMS re-uses the SBSFU SWAP area */


/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup SFU_CORE SBSFU Application
  * @{
  */

/** @addtogroup SFU_KMS SFU KMS Blob
  * @{
  */

/* Private variables ---------------------------------------------------------*/
/** @addtogroup SFU_KMS_Private_Defines KMS Blob Private Defines
  * @{
  */

/**
  * Magic pattern identifying the beginning of a KMS blob.
  */
uint8_t     kms_magic[4] = {"KMSB"};

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup SFU_KMS_Private_Macros SFU KMS Blob Private Macros
  * @{
  */

/**
  * The KMS blob header has the same structure as the SBSFU Firmware Header.
  * Therefore, authenticating a KMS blob header can be done by re-using the SBSFU FWIMG service authenticating a FW
  * Header.
  * The authentication of the KMS blob itself is not in the scope of sfu_kms, this is handled by the KMS middleware.
  */
#define SFU_KMS_AUTHENTICATE_BLOB_HEADER SFU_IMG_GetFWInfoMAC

/**
  * @}
  */

/* Functions Definition ------------------------------------------------------*/

/** @defgroup SFU_KMS_SERVICES SFU Firmware Image Services
  * @brief FW handling services the bootloader can call.
  * @{
  */

/** @defgroup SFU_KMS_Exported_Functions FW Images Handling Services
  * @brief Services the bootloader can call to handle the FW images.
  * @{
  */

/** @defgroup SFU_KMS_Initialization_Functions FW Images Handling Init. Functions
  * @brief FWIMG initialization functions.
  * @{
  */

/**
  * @}
  */

/** @defgroup SFU_KMS_Service_Functions FW Images Handling Services Functions
  * @brief FWIMG services functions.
  * @{
  */

/** @defgroup SFU_KMS_Functions_NewBlob New KMSBlob image installation services
  * @brief New image installation functions.
  * @{
  */

/**
  * @brief  Check that there is a Blob to Install ==> KMSB MAGIC present in SWAP ?
  * @retval SFU_SUCCESS if Image can be installed, a SFU_ERROR  otherwise.
  */
SFU_ErrorStatus SFU_KMS_BlobToInstall(void)
{
  SFU_ErrorStatus  e_ret_status = SFU_ERROR;
  uint8_t *pbuffer = (uint8_t *) SFU_IMG_SLOT_DWL_REGION_BEGIN;
  /*
   * The KMS blob header uses a structure dedicated Header.
   * In this function we focus only on the KMS blob header, not the entire KMS blob.
   */

  /*  Loading the KMS blob header contained in the SWAP area to verify if it can be a valid KMS blob */
  e_ret_status = SFU_LL_FLASH_Read(&kmsblob_image_header_to_test, pbuffer, sizeof(KMS_BlobRawHeaderTypeDef));
  if (e_ret_status == SFU_SUCCESS)
  {
    /* Is there any KMSBlob present in the SWAP ? */
    if (memcmp(&kmsblob_image_header_to_test, kms_magic, sizeof(kms_magic)) == 0)
    {
      /*
       * Check the authenticity of the KMS Blob stored in the SWAP slot.
       * This check is restricted to the authentication of the KMS blob header.
       * Authenticating the KMS blob itself is the role of the KMS module (KMS middleware).
       */
      /* OAR: Move authentication resp. to KMS code */
      /* e_ret_status = SFU_KMS_AUTHENTICATE_BLOB_HEADER(&fw_image_header_to_test, 2); */
      e_ret_status = SFU_SUCCESS;
    }
    else
    {
      e_ret_status = SFU_ERROR;
    }
  }

  return e_ret_status;
}

/**
  * @brief  Load the address of the Blob to Install ==> KMSB MAGIC present in SWAP ?
  * @retval none
  */
uint8_t *SFU_KMS_GetBlobInstallInfo(void)
{
  return ((uint8_t *) &kmsblob_image_header_to_test);
}

/**
  * @brief  Erase the Blob
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_KMS_EraseBlob(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_FLASH_StatusTypeDef flash_if_status;

  /*  Erase in flash  */
  e_ret_status = SFU_LL_FLASH_Erase_Size(&flash_if_status, SFU_IMG_SLOT_DWL_REGION_BEGIN, SFU_IMG_SLOT_DWL_REGION_SIZE);

  return e_ret_status;
}


/**
  * @brief Checks if there is a pending Blob installation.
  *        3 situations can occur:
  *        A. First installation request for a blob: a blob is ready to be installed but the installation has never
  *           been triggered so far.
  *        B. Blob installation to be retried: a blob installation has been interrupted (switch off) and we can restart
  *           its installation.
  *        C. No blob installation pending.
    * @note This function populates the FWIMG module variable: fw_image_header_to_test.
  *         If a KMS blob is present then it is copied in fw_image_header_to_test.
  * @param  None.
  * @retval SFU_KMS_BlobInstallStateTypeDef Pending Installation status (installation requested or not)
  */
SFU_KMS_BlobInstallStateTypeDef SFU_KMS_CheckPendingBlobInstallation(void)
{
  SFU_KMS_BlobInstallStateTypeDef e_ret_state = SFU_KMS_NO_BLOBUPDATE;

  /*
   * While the SWAP AREA contains a KMSB Magic, it means the KMSBlob installation is not finalized.
   *
   */
  if (SFU_SUCCESS == SFU_KMS_BlobToInstall())
  {
    /*
     * A KMS blob is available for installation:
     * fw_image_header_to_test has been populated with this blob.
     */
    e_ret_state = SFU_KMS_BLOBIMAGE_TO_INSTALL;
  }
  else
  {
    /* No pending update */
    e_ret_state = SFU_KMS_NO_BLOBUPDATE;
  }

  return e_ret_state;
}



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

#endif /* KMS_ENABLED */


#undef SFU_KMS_C

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
