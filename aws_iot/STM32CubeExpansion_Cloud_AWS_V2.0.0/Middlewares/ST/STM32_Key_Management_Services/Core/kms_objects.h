/**
  ******************************************************************************
  * @file    kms_objects.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module object manipulation services.
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
#ifndef KMS_OBJECTS_H
#define KMS_OBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "kms.h"


/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_OBJECTS Blob Objects Management
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup KMS_OBJECTS_Exported_Types Exported Types
  * @{
  */
/**
  * @brief Allows to identify the different range of object IDs
  */
typedef enum
{
  KMS_OBJECT_RANGE_EMBEDDED = 0,              /*!< Objects embedded in code at compilation time */
  KMS_OBJECT_RANGE_NVM_STATIC_ID,             /*!< Objects stored in NVM with Static IDs */
  KMS_OBJECT_RANGE_NVM_DYNAMIC_ID,            /*!< Objects stored in NVM with Dynamic IDs */
  KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID,       /*!< Objects stored in external token with Static IDs */
  KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID,      /*!< Objects stored in external token with Dynamic IDs */
  KMS_OBJECT_RANGE_UNKNOWN                    /*!< Unknown objects */
} kms_obj_range_t;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/** @addtogroup KMS_OBJECTS_Exported_Functions Exported Functions
  * @{
  */

kms_obj_keyhead_t *KMS_Objects_GetPointer(CK_OBJECT_HANDLE hKey);
kms_obj_range_t    KMS_Objects_GetRange(CK_OBJECT_HANDLE hKey);
CK_RV              KMS_LockKeyHandle(CK_OBJECT_HANDLE hKey);
CK_RV              KMS_CheckKeyIsNotLocked(CK_OBJECT_HANDLE hKey);

CK_RV              KMS_Objects_SearchAttributes(uint32_t searched_id, kms_obj_keyhead_t *pkms_key_head,
                                                kms_ref_t **pAttribute);
CK_RV              KMS_Objects_TranslateKey(kms_ref_t *pAttribute, uint8_t *vKeyValue);
void               KMS_Objects_TranslateRsaAttributes(kms_ref_t *pAttribute, uint8_t *vKeyValue);
CK_RV              KMS_Objects_ImportBlob(CK_BYTE_PTR pData, CK_ULONG_PTR pImportBlobState);


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

#endif /* KMS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
