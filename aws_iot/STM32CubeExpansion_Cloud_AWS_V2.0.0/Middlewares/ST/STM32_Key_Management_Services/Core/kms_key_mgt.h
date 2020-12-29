/**
  ******************************************************************************
  * @file    kms_key_mgt.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module key handling functionalities.
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
#ifndef KMS_KEY_MGT_H
#define KMS_KEY_MGT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "kms.h"

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_KEY Key handling services
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/** @addtogroup KMS_KEY_Exported_Functions Exported Functions
  * @{
  */
/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup KMS_API KMS APIs (PKCS#11 Standard Compliant)
  * @{
  */

#ifdef KMS_DERIVE_KEY
CK_RV KMS_DeriveKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                    CK_OBJECT_HANDLE hBaseKey, CK_ATTRIBUTE_PTR  pTemplate,
                    CK_ULONG  ulAttributeCount, CK_OBJECT_HANDLE_PTR  phKey);
#endif /* KMS_DERIVE_KEY */
#ifdef KMS_WRAP_KEY
CK_RV KMS_WrapKey(CK_SESSION_HANDLE hSession,     CK_MECHANISM_PTR  pMechanism,
                  CK_OBJECT_HANDLE  hWrappingKey, CK_OBJECT_HANDLE  hKey,
                  CK_BYTE_PTR       pWrappedKey,  CK_ULONG_PTR      pulWrappedKeyLen);
#endif /* KMS_WRAP_KEY */
#ifdef KMS_GET_ATTRIBUTE_VALUE
CK_RV KMS_GetAttributeValue(CK_SESSION_HANDLE hSession,  CK_OBJECT_HANDLE  hObject,
                            CK_ATTRIBUTE_PTR  pTemplate, CK_ULONG          ulCount);
#endif /* KMS_GET_ATTRIBUTE_VALUE */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_KEY_MGT_H */


