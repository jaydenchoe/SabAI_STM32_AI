/**
  ******************************************************************************
  * @file    kms_platf_objects.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module platform objects management
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
#ifndef KMS_PLATF_OBJECTS_H
#define KMS_PLATF_OBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "kms.h"
#include "kms_nvm_storage.h"

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_PLATF Platform Objects
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/** @addtogroup KMS_PLATF_Exported_Functions Exported Functions
  * @{
  */

void   KMS_PlatfObjects_Init(void);
void   KMS_PlatfObjects_Finalize(void);

void            KMS_PlatfObjects_EmbeddedRange(uint32_t *pMin, uint32_t *pMax);
kms_obj_keyhead_t *KMS_PlatfObjects_EmbeddedObject(uint32_t hKey);
#ifdef KMS_NVM_ENABLED
void            KMS_PlatfObjects_NvmStaticRange(uint32_t *pMin, uint32_t *pMax);
kms_obj_keyhead_t *KMS_PlatfObjects_NvmStaticObject(uint32_t hKey);
#ifdef KMS_NVM_DYNAMIC_ENABLED
void            KMS_PlatfObjects_NvmDynamicRange(uint32_t *pMin, uint32_t *pMax);
kms_obj_keyhead_t *KMS_PlatfObjects_NvmDynamicObject(uint32_t hKey);
#endif /* KMS_NVM_DYNAMIC_ENABLED */
#endif /* KMS_NVM_ENABLED */
#ifdef KMS_EXT_TOKEN_ENABLED
void            KMS_PlatfObjects_ExtTokenStaticRange(uint32_t *pMin, uint32_t *pMax);
void            KMS_PlatfObjects_ExtTokenDynamicRange(uint32_t *pMin, uint32_t *pMax);
#endif /* KMS_EXT_TOKEN_ENABLED */
nvms_error_t    KMS_PlatfObjects_NvmStoreObject(uint32_t ObjectId, uint8_t *pObjectToAdd,  uint32_t ObjectSize);
CK_RV           KMS_PlatfObjects_AllocateNvmDynamicObjectId(CK_OBJECT_HANDLE_PTR pObjId);
void            KMS_PlatfObjects_NvmDynamicObjectList(void);
nvms_error_t    KMS_PlatfObjects_NvmRemoveObject(uint32_t ObjectId);


void            KMS_PlatfObjects_NvmInitialize(void);
void            KMS_PlatfObjects_NvmFinalize(void);

CK_ULONG        KMS_PlatfObjects_GetBlobVerifyKey(void);
CK_ULONG        KMS_PlatfObjects_GetBlobDecryptKey(void);


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

#endif /* KMS_PLATF_OBJECTS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

