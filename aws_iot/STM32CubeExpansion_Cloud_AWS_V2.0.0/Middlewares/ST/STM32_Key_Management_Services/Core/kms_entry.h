/**
  ******************************************************************************
  * @file    kms_entry.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for the entry point of Key Management
  *          Services module functionalities.
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
#ifndef KMS_ENTRY_H
#define KMS_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include "se_callgate.h"
#include "kms.h"

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_ENTRY Entry Point
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup KMS_ENTRY_Exported_Types Exported Types
  * @{
  */

#define KMS_INITIALIZE_FCT_ID             (SE_MW_ADDON_KMS_MSB+0x00UL)            /*!< C_Initialize function ID */
#define KMS_FINALIZE_FCT_ID               (KMS_INITIALIZE_FCT_ID+0x01UL)          /*!< C_Finalize function ID */
#define KMS_GET_INFO_FCT_ID               (KMS_FINALIZE_FCT_ID+0x01UL)            /*!< C_GetInfo function ID */
#define KMS_GET_SLOT_LIST_FCT_ID          (KMS_GET_INFO_FCT_ID+0x01UL)            /*!< C_GetSlotList function ID */
#define KMS_GET_SLOT_INFO_FCT_ID          (KMS_GET_SLOT_LIST_FCT_ID+0x01UL)       /*!< C_GetSlotInfo function ID */
#define KMS_GET_TOKEN_INFO_FCT_ID         (KMS_GET_SLOT_INFO_FCT_ID+0x01UL)       /*!< C_GetTokenInfo function ID */
#define KMS_GET_MECHANISM_INFO_FCT_ID     (KMS_GET_TOKEN_INFO_FCT_ID+0x01UL)      /*!< C_GetMechanismInfo function ID */
#define KMS_INIT_TOKEN_FCT_ID             (KMS_GET_MECHANISM_INFO_FCT_ID+0x01UL)  /*!< C_InitToken function ID */
#define KMS_OPEN_SESSION_FCT_ID           (KMS_INIT_TOKEN_FCT_ID+0x01UL)          /*!< C_OpenSession function ID */
#define KMS_CLOSE_SESSION_FCT_ID          (KMS_OPEN_SESSION_FCT_ID+0x01UL)        /*!< C_CloseSession function ID */
#define KMS_CREATE_OBJECT_FCT_ID          (KMS_CLOSE_SESSION_FCT_ID+0x01UL)       /*!< C_CreateObject function ID */
#define KMS_DESTROY_OBJECT_FCT_ID         (KMS_CREATE_OBJECT_FCT_ID+0x01UL)       /*!< C_DestroyObject function ID */
#define KMS_FIND_OBJECTS_INIT_FCT_ID      (KMS_DESTROY_OBJECT_FCT_ID+0x01UL)      /*!< C_FindObjectsInit function ID */
#define KMS_FIND_OBJECTS_FCT_ID           (KMS_FIND_OBJECTS_INIT_FCT_ID+0x01UL)   /*!< C_FindObjects function ID */
#define KMS_FIND_OBJECTS_FINAL_FCT_ID     (KMS_FIND_OBJECTS_FCT_ID+0x01UL)        /*!< C_FindObjectsFinal function ID */
#define KMS_ENCRYPT_INIT_FCT_ID           (KMS_FIND_OBJECTS_FINAL_FCT_ID+0x01UL)  /*!< C_EncryptInit function ID */
#define KMS_ENCRYPT_FCT_ID                (KMS_ENCRYPT_INIT_FCT_ID+0x01UL)        /*!< C_Encrypt function ID */
#define KMS_ENCRYPT_UPDATE_FCT_ID         (KMS_ENCRYPT_FCT_ID+0x01UL)             /*!< C_EncryptUpdate function ID */
#define KMS_ENCRYPT_FINAL_FCT_ID          (KMS_ENCRYPT_UPDATE_FCT_ID+0x01UL)      /*!< C_EncryptFinal function ID */
#define KMS_DECRYPT_INIT_FCT_ID           (KMS_ENCRYPT_FINAL_FCT_ID+0x01UL)       /*!< C_DecryptInit function ID */
#define KMS_DECRYPT_FCT_ID                (KMS_DECRYPT_INIT_FCT_ID+0x01UL)        /*!< C_Decrypt function ID */
#define KMS_DECRYPT_UPDATE_FCT_ID         (KMS_DECRYPT_FCT_ID+0x01UL)             /*!< C_DecryptUpdate function ID */
#define KMS_DECRYPT_FINAL_FCT_ID          (KMS_DECRYPT_UPDATE_FCT_ID+0x01UL)      /*!< C_DecryptFinal function ID */
#define KMS_DIGEST_INIT_FCT_ID            (KMS_DECRYPT_FINAL_FCT_ID+0x01UL)       /*!< C_DigestInit function ID */
#define KMS_DIGEST_FCT_ID                 (KMS_DIGEST_INIT_FCT_ID+0x01UL)         /*!< C_Digest function ID */
#define KMS_DIGEST_UPDATE_FCT_ID          (KMS_DIGEST_FCT_ID+0x01UL)              /*!< C_DigestUpdate function ID */
#define KMS_DIGEST_KEY_FCT_ID             (KMS_DIGEST_UPDATE_FCT_ID+0x01UL)       /*!< C_DigestKey function ID */
#define KMS_DIGEST_FINAL_FCT_ID           (KMS_DIGEST_KEY_FCT_ID+0x01UL)          /*!< C_DigestFinal function ID */
#define KMS_SIGN_INIT_FCT_ID              (KMS_DIGEST_FINAL_FCT_ID+0x01UL)        /*!< C_SignInit function ID */
#define KMS_SIGN_FCT_ID                   (KMS_SIGN_INIT_FCT_ID+0x01UL)           /*!< C_Sign function ID */
#define KMS_VERIFY_INIT_FCT_ID            (KMS_SIGN_FCT_ID+0x01UL)                /*!< C_VerifyInit function ID */
#define KMS_VERIFY_FCT_ID                 (KMS_VERIFY_INIT_FCT_ID+0x01UL)         /*!< C_Verify function ID */
#define KMS_IMPORT_BLOB_FCT_ID            (KMS_VERIFY_FCT_ID+0x01UL)              /*!< Import Blob function ID */
#define KMS_DERIVE_KEY_FCT_ID             (KMS_IMPORT_BLOB_FCT_ID+0x01UL)         /*!< C_DeriveKey function ID */
#define KMS_WRAP_KEY_FCT_ID               (KMS_DERIVE_KEY_FCT_ID+0x01UL)          /*!< C_WrapKey function ID */
#define KMS_GET_ATTRIBUTE_VALUE_FCT_ID    (KMS_WRAP_KEY_FCT_ID+0x01UL)            /*!< C_GetAttributeValue function ID*/
#define KMS_SET_ATTRIBUTE_VALUE_FCT_ID    (KMS_GET_ATTRIBUTE_VALUE_FCT_ID+0x01UL) /*!< C_SetAttributeValue function ID*/
#define KMS_GENERATE_KEYPAIR_FCT_ID       (KMS_SET_ATTRIBUTE_VALUE_FCT_ID+0x01UL) /*!< C_GenerateKeyPair function ID */
#define KMS_GENERATE_RANDOM_FCT_ID        (KMS_GENERATE_KEYPAIR_FCT_ID+0x01UL)    /*!< C_GenerateRandom function ID */
#ifdef KMS_UNITARY_TEST
#define KMS_UNITARY_TEST_FCT_ID           (KMS_GENERATE_RANDOM_FCT_ID+0x01UL)     /*!< Unitary test function ID */
#define KMS_LAST_ID_CHECK                 (KMS_UNITARY_TEST_FCT_ID)               /*!< Last function ID */
#else /* KMS_UNITARY_TEST */
#define KMS_LAST_ID_CHECK                 (KMS_GENERATE_RANDOM_FCT_ID)               /*!< Last function ID */
#endif /* KMS_UNITARY_TEST */

/**
  * @brief KMS entry function ID type definition
  */
typedef uint32_t KMS_FunctionIDTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @addtogroup KMS_ENTRY_Exported_Constants Exported Constants
  * @{
  */
#define   KMS_FIRST_ID  KMS_INITIALIZE_FCT_ID   /*!< KMS entry first function ID */
#define   KMS_LAST_ID   KMS_LAST_ID_CHECK       /*!< KMS entry last function ID */

/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/

/** @addtogroup KMS_ENTRY_Exported_Functions Exported Functions
  * @{
  */
CK_RV  KMS_Entry(KMS_FunctionIDTypeDef eID, va_list arguments);

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

#endif /* KMS_ENTRY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

