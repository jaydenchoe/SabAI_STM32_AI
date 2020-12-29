/**
  ******************************************************************************
  * @file    kms.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module functionalities.
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
#ifndef KMS_H
#define KMS_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "pkcs11.h"
#include "kms_config.h"
#include "kms_blob_headers.h"


#ifdef __cplusplus
extern "C" {
#endif

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
  * @brief Blob import finalized
  */
#define KMS_IMPORT_BLOB_FINALIZED                (CKR_VENDOR_DEFINED+0UL)
/**
  * @brief Blob import failed: authentication error
  */
#define KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR   (CKR_VENDOR_DEFINED+1UL)
/**
  * @brief Blob import failed: verification error
  */
#define KMS_IMPORT_BLOB_CRYPTO_BLOB_VERIF_ERROR  (CKR_VENDOR_DEFINED+2UL)
/**
  * @brief Blob import failed: form error
  */
#define KMS_IMPORT_BLOB_CRYPTO_BLOB_FORM_ERROR   (CKR_VENDOR_DEFINED+3UL)
/**
  * @brief Blob import failed: object  ID error
  */
#define KMS_IMPORT_BLOB_OBJECTID_ERROR           (CKR_VENDOR_DEFINED+4UL)
/**
  * @brief Blob import failed: NVM error
  */
#define KMS_IMPORT_BLOB_NVM_ERROR                (CKR_VENDOR_DEFINED+5UL)
/**
  * @brief Blob import finalized with NVM warning
  */
#define KMS_IMPORT_BLOB_NVM_WARNING              (CKR_VENDOR_DEFINED+6UL)
/**
  * @brief Blob import failed: session in use
  */
#define KMS_IMPORT_BLOB_SESSION_INUSE            (CKR_VENDOR_DEFINED+7UL)
/**
  * @brief Blob import intermediate status: no error
  */
#define KMS_IMPORT_BLOB_NO_ERROR                 (CKR_VENDOR_DEFINED+8UL)

/**
  * @brief KMS blob import returned state type definition
  */
typedef uint32_t kms_import_blob_state_t;
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup KMS_INIT Initialization And Session handling
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Constants Exported Constants
  * @{
  */
#define KMS_HANDLE_KEY_NOT_KNOWN        0x00U     /*!< Unknow KMS key handle value */


#define KMS_SESSION_NOT_USED            0xFFFFFFFFUL  /*!< KMS Session not in use */
#define KMS_SESSION_IDLE                0x00000000UL  /*!< KMS Session opened no action ongoing */
#define KMS_SESSION_DIGEST              0x00000001UL  /*!< KMS Session digest ongoing */
#define KMS_SESSION_ENCRYPT             0x00000002UL  /*!< KMS Session encryption ongoing */
#define KMS_SESSION_DECRYPT             0x00000003UL  /*!< KMS Session decryption ongoing */
#define KMS_SESSION_SIGN                0x00000004UL  /*!< KMS Session signature ongoing */
#define KMS_SESSION_VERIFY              0x00000005UL  /*!< KMS Session verification ongoing */

/**
  * @}
  */



/* Exported types ------------------------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Types Exported Types
  * @{
  */
/**
  * @brief KMS session management structure definition
  */
typedef struct
{
  CK_SLOT_ID    slotID;                 /*!< Session Slot ID */
  CK_ULONG      state;                  /*!< Session state */
  CK_FLAGS      flags;                  /*!< Session flags */
  CK_ULONG      ulDeviceError;          /*!< Session device error */
  CK_VOID_PTR   pApplication;           /*!< Session application-defined pointer */
  CK_NOTIFY     Notify;                 /*!< Session notification callback */
  CK_MECHANISM_TYPE   Mechanism;        /*!< Session mechanism */
  CK_OBJECT_HANDLE hKey;                /*!< Session key handle */
  CK_VOID_PTR   pKeyAllocBuffer;        /*!< Session key allocated buffer */
#ifdef KMS_EXT_TOKEN_ENABLED
  CK_SESSION_HANDLE hSession_ExtToken;  /*!< Session external token associated session handle */
#endif /* KMS_EXT_TOKEN_ENABLED */
} kms_session_desc_t;


/**
  * @brief Item reference in a serial blob
  */
typedef struct
{
  uint32_t id;        /*!< Item ID */
  uint32_t size;      /*!< Item Size */
  uint32_t data[1];   /*!< Item data */
} kms_ref_t;

/**
  * @}
  */


/* Exported variables --------------------------------------------------------*/

/** @addtogroup KMS_INIT_Exported_Variables Exported Variables
  * @{
  */
extern kms_session_desc_t    KMS_SessionList[KMS_NB_SESSIONS_MAX];
/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Macros Exported Macros
  * @{
  */
/**
  * @brief KMS Session description assessor from external handle
  * @note  KMS session externally accessed with session handle starting from 1
  *        but internally, sessions are stored into a table indexed from 0
  */
#define KMS_GETSESSSION(HANDLE)  (KMS_SessionList[(HANDLE)-1UL])
/**
  * @}
  */

/**
  * @brief Check given buffer and its length compared to required length following Section 5.2 recommendation
  * @verbatim
5.2 Conventions for functions returning output in a variable-length buffer
A number of the functions defined in Cryptoki return output produced by some cryptographic mechanism.  The amount of output returned by these functions is returned in a variable-length application-supplied buffer.  An example of a function of this sort is C_Encrypt, which takes some plaintext as an argument, and outputs a buffer full of ciphertext.

These functions have some common calling conventions, which we describe here.  Two of the arguments to the function are a pointer to the output buffer (say pBuf) and a pointer to a location which will hold the length of the output produced (say pulBufLen).  There are two ways for an application to call such a function:

1.     If pBuf is NULL_PTR, then all that the function does is return (in *pulBufLen) a number of bytes which would suffice to hold the cryptographic output produced from the input to the function.  This number may somewhat exceed the precise number of bytes needed, but should not exceed it by a large amount.  CKR_OK is returned by the function.

2.     If pBuf is not NULL_PTR, then *pulBufLen MUST contain the size in bytes of the buffer pointed to by pBuf.  If that buffer is large enough to hold the cryptographic output produced from the input to the function, then that cryptographic output is placed there, and CKR_OK is returned by the function.  If the buffer is not large enough, then CKR_BUFFER_TOO_SMALL is returned.  In either case, *pulBufLen is set to hold the exact number of bytes needed to hold the cryptographic output produced from the input to the function.
  * @endverbatim
  */
#define KMS_CHECK_BUFFER_SECTION5_2(DATA_PTR, LEN_PTR, NEED_LENGTH)                             \
  do                                                                                            \
  {                                                                                             \
    if(DATA_PTR == NULL)                                                                        \
    {                                                                                           \
      *LEN_PTR = NEED_LENGTH;                                                                   \
      return CKR_OK;                                                                            \
    }                                                                                           \
    if(*LEN_PTR < NEED_LENGTH)                                                                  \
    {                                                                                           \
      *LEN_PTR = NEED_LENGTH;                                                                   \
      return CKR_BUFFER_TOO_SMALL;                                                              \
    }                                                                                           \
  } while(0);
/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Functions Exported Functions
  * @{
  */

/* kms_init */
#ifdef KMS_EXT_TOKEN_ENABLED
CK_RV  KMS_CallbackFunction_ForExtToken(CK_SESSION_HANDLE hSession,
                                        CK_NOTIFICATION event,
                                        CK_VOID_PTR pApplication);
CK_RV  KMS_OpenSession_RegisterExtToken(CK_SESSION_HANDLE hSession,
                                        CK_SESSION_HANDLE hSession_ExtToken);
#endif /* KMS_EXT_TOKEN_ENABLED */

CK_RV  KMS_Check_Session_Handle(CK_SESSION_HANDLE hSession) ;

#if defined(KMS_UNITARY_TEST)
CK_RV  KMS_UnitaryTest_GetAttribute(CK_ULONG_PTR plNbPassed, CK_ULONG_PTR plNbFailed);
CK_RV  KMS_UnitaryTest(CK_ULONG ulTestID, CK_ULONG_PTR plNbPassed, CK_ULONG_PTR plNbFailed);
#endif /* KMS_UNITARY_TEST */


/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup KMS_API KMS APIs (PKCS#11 Standard Compliant)
  * @{
  */
CK_RV  KMS_Initialize(CK_VOID_PTR pInitArgs) ;
CK_RV  KMS_Finalize(CK_VOID_PTR pReserved);
CK_RV  KMS_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo);
CK_RV  KMS_OpenSession(CK_SLOT_ID slotID, CK_FLAGS flags,
                       CK_VOID_PTR pApplication, CK_NOTIFY Notify,
                       CK_SESSION_HANDLE_PTR phSession);
CK_RV  KMS_CloseSession(CK_SESSION_HANDLE hSession);
CK_RV  KMS_GetMechanismInfo(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo);



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
