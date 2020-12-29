/**
  ******************************************************************************
  * @file    kms_aes.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module AES functionalities.
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
#ifndef KMS_AES_H
#define KMS_AES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "kms.h"

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_AES AES services
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/** @addtogroup KMS_AES_Exported_Functions Exported Functions
  * @{
  */

void     KMS_InitBufferAlloc(void);
void     KMS_FinalizeBufferAlloc(void);
void    *KMS_AllocKey(uint32_t size);
void     KMS_FreeKey(const void *pBufferToRelease);


/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup KMS_API KMS APIs (PKCS#11 Standard Compliant)
  * @{
  */

CK_RV          KMS_EncryptInit(CK_SESSION_HANDLE hSession,
                               CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey);

CK_RV          KMS_Encrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                           CK_ULONG ulDataLen, CK_BYTE_PTR pEncryptedData,
                           CK_ULONG_PTR pulEncryptedDataLen);

CK_RV          KMS_EncryptUpdate(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR pPart, CK_ULONG ulPartLen,
                                 CK_BYTE_PTR pEncryptedPart,
                                 CK_ULONG_PTR pulEncryptedPartLen);

CK_RV          KMS_EncryptFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pLastEncryptedPart,
                                CK_ULONG_PTR pulLastEncryptedPartLen);

CK_RV          KMS_DecryptInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                               CK_OBJECT_HANDLE hKey);

CK_RV          KMS_Decrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedData,
                           CK_ULONG ulEncryptedDataLen,
                           CK_BYTE_PTR pData, CK_ULONG_PTR pulDataLen);

CK_RV          KMS_DecryptUpdate(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR pEncryptedPart,
                                 CK_ULONG ulEncryptedPartLen,
                                 CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen);


CK_RV          KMS_DecryptFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pLastPart, CK_ULONG_PTR pulLastPartLen);


/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_AES_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

