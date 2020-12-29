/**
  ******************************************************************************
  * @file    kms_init.c
  * @author  MCD Application Team
  * @brief   Secure Engine CRYPTO module.
  *          This file provides the initiatisation function of the Key
  *            Management Services functionalities.
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

/* Includes ------------------------------------------------------------------*/
#include "kms.h"

#include "kms_aes.h"                    /* Key storage initialisation */
#include "kms_nvm_storage.h"            /* KMS storage services */
#include "kms_platf_objects.h"          /* KMS platform objects services */
#include "kms_low_level.h"              /* CRC configuration for Init */



/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_INIT Initialization And Session handling
  * @{
  */


/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @addtogroup KMS_INIT_Private_Variables Private Variables
  * @{
  */
static CK_BBOOL  initialized = CK_FALSE;   /*!< P11 Initialization Status.
                                                CK_TRUE after successful call to C_Initialize
                                                CK_FALSE after successful call to C_Finalize */
static CK_ULONG session_nb = 0;   /*!< Current number of session in use */
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
/** @addtogroup KMS_INIT_Exported_Variables Exported Variables
  * @{
  */
/**
  * @brief KMS session management variable
  */
kms_session_desc_t    KMS_SessionList[KMS_NB_SESSIONS_MAX];
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_INIT_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This function is called upon @ref C_Initialize call
  * @note   Refer to @ref C_Initialize function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  pInitArgs either has the value NULL_PTR or points to a
  *         CK_C_INITIALIZE_ARGS structure containing information on how the
  *         library should deal with multi-threaded access
  * @retval Operation status
  */
CK_RV  KMS_Initialize(CK_VOID_PTR pInitArgs)
{
  /* Check paremeters */
  if (pInitArgs != NULL)
  {
    CK_C_INITIALIZE_ARGS_PTR  ptr = (CK_C_INITIALIZE_ARGS_PTR)pInitArgs;
    /* pReserved must be NULL */
    if (ptr->pReserved != NULL)
    {
      return CKR_ARGUMENTS_BAD;
    }
    if (
    /* if there is at least one pointer set */
    ((ptr->CreateMutex != NULL) || (ptr->DestroyMutex != NULL) || (ptr->LockMutex != NULL) || (ptr->UnlockMutex != NULL))
      &&
    /* and at least one pointer not set */
    ((ptr->CreateMutex == NULL) || (ptr->DestroyMutex == NULL) || (ptr->LockMutex == NULL) || (ptr->UnlockMutex == NULL))
    )
    {
      return CKR_ARGUMENTS_BAD;
    }
    if ((ptr->flags == 0)
      &&
    ((ptr->CreateMutex == NULL) && (ptr->DestroyMutex == NULL) && (ptr->LockMutex == NULL) && (ptr->UnlockMutex == NULL))
    )
    {
      /* = the application won’t be accessing the Cryptoki library from multiple threads simultaneously */
      /* => Supported case */
    }
    else
    {
      //return CKR_CANT_LOCK;  // Remove for now: to be handled properly later on
    }
  }
  /* Initialize KMS */
  session_nb = 0;

  /* Check if PKCS11 module has already been initialized */
  if (initialized == CK_TRUE)
  {
    return CKR_CRYPTOKI_ALREADY_INITIALIZED;
  }

  /* Initialize SessionList */
  for (uint32_t i = 1 ; i <= KMS_NB_SESSIONS_MAX; i++)
  {
    KMS_GETSESSSION(i).state = KMS_SESSION_NOT_USED;
    KMS_GETSESSSION(i).pKeyAllocBuffer = NULL;
  }

  /* Initialize the buffer alloc */
  KMS_InitBufferAlloc();

  /* This is to enable the CryptoLib */
  (void)KMS_LL_CRC_Init();

  /* Call the Platform Init function */
  KMS_PlatfObjects_Init();

  /* Marking module as initialized */
  initialized = CK_TRUE;

  return CKR_OK;
}

/**
  * @brief  This function is called upon @ref C_Finalize call
  * @note   Refer to @ref C_Finalize function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  pReserved reserved for future versions
  * @retval Operation status
  */
CK_RV KMS_Finalize(CK_VOID_PTR pReserved)
{
  /* To fullfill the PKCS11 spec the input parameter is expected to be NULL */
  if (pReserved != NULL_PTR)
  {
    return CKR_ARGUMENTS_BAD;
  }

  /* Check if PKCS11 module has already been initialized */
  if (initialized != CK_TRUE)
  {
    return CKR_CRYPTOKI_NOT_INITIALIZED;
  }

  /* Release the buffer alloc */
  KMS_FinalizeBufferAlloc();

  /* Call the Platform Finalize function */
  KMS_PlatfObjects_Finalize();

  /* Marking module as not initialized */
  initialized = CK_FALSE;

  return CKR_OK;
}


/**
  * @brief  This function is called upon @ref C_GetTokenInfo call
  * @note   Refer to @ref C_GetTokenInfo function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  slotID token slot ID
  * @param  pInfo token information
  * @retval Operation status
  */
CK_RV KMS_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo)
{
  const CK_TOKEN_INFO token_desc_template =
  {
    "KMS on STM32",                       /* label[32]; */

    "ST Microelectronics",                /* manufacturerID[32]; */
#ifdef KMS_EXT_TOKEN_ENABLED
    "KMS + Ext.Token",                    /* model[16]; */
#else
    "KMS Foundations",                    /* model[16]; */
#endif  /* KMS_EXT_TOKEN_ENABLED */
    "",                                   /* serialNumber[16]; */
    0,                                    /* flags; CKF_RNG, CKF_WRITE_PROTECTED, ... */

    KMS_NB_SESSIONS_MAX,                  /* ulMaxSessionCount */
    0,                                    /* ulSessionCount */
    KMS_NB_SESSIONS_MAX,                  /* ulMaxRwSessionCount */
    0,                                    /* ulRwSessionCount */

    0,                                    /* ulMaxPinLen */
    0,                                    /* ulMinPinLen */
    0,                                    /* ulTotalPublicMemory*/
    0,                                    /* ulFreePublicMemory */
    0,                                    /* ulTotalPrivateMemory */
    0,                                    /* ulFreePrivateMemory */
    {0, 0},                               /* hardwareVersion */
    {0, 1},                               /* firmwareVersion */
    {0}                                   /* utcTime[16] */
  };

  (void)(slotID);

  /* Setup the structure with the default values */
  (void)memcpy(pInfo, &token_desc_template, sizeof(token_desc_template));

  /* Pass the Flag */
  pInfo->flags = CKF_WRITE_PROTECTED;

  return CKR_OK;
}

/**
  * @brief  This function is called upon @ref C_OpenSession call
  * @note   Refer to @ref C_OpenSession function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  slotID slot ID
  * @param  flags session type
  * @param  pApplication application-defined pointer to be passed to the notification callback
  * @param  Notify notification callback function
  * @param  phSession handle for the newly opened session
  * @retval Operation status
  */
CK_RV KMS_OpenSession(CK_SLOT_ID slotID, CK_FLAGS flags,
                      CK_VOID_PTR pApplication, CK_NOTIFY Notify,
                      CK_SESSION_HANDLE_PTR phSession)
{
  uint32_t session_index;

  /* As defined in PKCS11 spec: For legacy reasons, the CKF_SERIAL_SESSION bit MUST
     always be set; if a call to C_OpenSession does not have this bit set, the call
     should return unsuccessfully with the error code CKR_SESSION_PARALLEL_NOT_SUPPORTED.
  */
  if ((flags & CKF_SERIAL_SESSION) == 0UL)
  {
    return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
  }

  /* We reach the max number of opened sessions */
  if (session_nb >=  KMS_NB_SESSIONS_MAX)
  {
    return CKR_SESSION_COUNT;
  }

  /* Find a slot for a Session */
  session_index = 1;
  do
  {
    if (KMS_GETSESSSION(session_index).state == KMS_SESSION_NOT_USED)
    {
      break;
    }
    session_index++;
  } while (session_index <= KMS_NB_SESSIONS_MAX); /* Session index are going from 1 to KMS_NB_SESSIONS_MAX */

  /* Slot is found */
  if ((session_index <= KMS_NB_SESSIONS_MAX) &&
      (KMS_GETSESSSION(session_index).state == KMS_SESSION_NOT_USED))
  {

    *phSession = session_index;
    KMS_GETSESSSION(session_index).slotID = slotID;
    KMS_GETSESSSION(session_index).state = KMS_SESSION_IDLE;      /* Initialized */
    KMS_GETSESSSION(session_index).flags = flags;
    KMS_GETSESSSION(session_index).pApplication = pApplication;
    KMS_GETSESSSION(session_index).Notify = Notify;
    /* A session can have only one crypto mechanism on going at a time. */
    KMS_GETSESSSION(session_index).hKey = KMS_HANDLE_KEY_NOT_KNOWN;
    KMS_GETSESSSION(session_index).Mechanism = CKM_VENDOR_DEFINED;
    KMS_GETSESSSION(session_index).pKeyAllocBuffer = NULL;
#ifdef KMS_EXT_TOKEN_ENABLED
    KMS_GETSESSSION(session_index).hSession_ExtToken = 0xFFFF;

#endif /* KMS_EXT_TOKEN_ENABLED      */

    /* Increment the session counter */
    session_nb++;

    return CKR_OK;
  }

  return CKR_SESSION_COUNT;
}

#ifdef KMS_EXT_TOKEN_ENABLED

/**
  * @brief  Notification callback for external token
  * @param  hSession handle of the session performing the callback
  * @param  event type of notification callback
  * @param  pApplication application-defined value (same value as
  *         @ref KMS_OpenSession pApplication parameter)
  * @retval Operation status
  */
CK_RV  KMS_CallbackFunction_ForExtToken(CK_SESSION_HANDLE hSession,
                                        CK_NOTIFICATION event,
                                        CK_VOID_PTR pApplication)
{
  uint32_t session_index;

  /* This Callback is in the KMS Rooter to handle all exchanges with applications */
  /* Search corresponding session ID in KMS domain */
  /* Find a slot for a Session */
  session_index = 1;
  do
  {
    if (KMS_GETSESSSION(session_index).hSession_ExtToken == hSession)
    {
      break;
    }
    session_index++;
  } while (session_index <= KMS_NB_SESSIONS_MAX);

  if (session_index <= KMS_NB_SESSIONS_MAX)
  {

    /* Check that the pointer to the callback is valid: in non secure & != NULL */
    if (KMS_GETSESSSION(hSession).Notify != NULL)
    {
      /* Call the Notify Callback() */
      (KMS_GETSESSSION(hSession).Notify)(session_index, event, KMS_GETSESSSION(hSession).pApplication);
    }

  }
  return CKR_OK;
}


/**
  * @brief  Used to link External Token session handle to internal session handle
  * @param  hSession internal session handle
  * @param  hSession_ExtToken external token session handle
  * @retval Operation status
  */
CK_RV  KMS_OpenSession_RegisterExtToken(CK_SESSION_HANDLE hSession,
                                        CK_SESSION_HANDLE hSession_ExtToken)
{
  /* Check that the session correspond to a waiting session */
  if ((KMS_GETSESSSION(hSession).state == KMS_SESSION_IDLE) &&
      (KMS_GETSESSSION(hSession).hSession_ExtToken == 0xFFFF) &&
      (hSession <= KMS_NB_SESSIONS_MAX))
  {
    /* Record the  corresponding SessionNumber of the External Token */
    KMS_GETSESSSION(hSession).hSession_ExtToken = hSession_ExtToken;
  }

  return CKR_OK;
}

#endif /* KMS_EXT_TOKEN_ENABLED       */


/**
  * @brief  This function is called upon @ref C_CloseSession call
  * @note   Refer to @ref C_CloseSession function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @retval Operation status
  */
CK_RV KMS_CloseSession(CK_SESSION_HANDLE hSession)
{
  if (KMS_GETSESSSION(hSession).state != KMS_SESSION_NOT_USED)
  {
    KMS_GETSESSSION(hSession).slotID = 0;
    KMS_GETSESSSION(hSession).state = KMS_SESSION_NOT_USED;   /* Free session */
    KMS_GETSESSSION(hSession).flags = 0;
    KMS_GETSESSSION(hSession).pApplication = NULL;
    KMS_GETSESSSION(hSession).Notify = NULL;
    KMS_GETSESSSION(hSession).pKeyAllocBuffer = NULL;

#ifdef KMS_EXT_TOKEN_ENABLED
    KMS_GETSESSSION(hSession).hSession_ExtToken = 0xFFFF;
#endif /* KMS_EXT_TOKEN_ENABLED      */

    /* Decrement the session counter */
    session_nb--;
  }

  return CKR_OK;
}

/**
  * @brief  Allows to check that the given handle corresponds to an opened session
  * @note   Can be called after @ref KMS_OpenSession and before @ref KMS_CloseSession
  * @param  hSession session handle
  * @retval Operation status
  */
CK_RV     KMS_Check_Session_Handle(CK_SESSION_HANDLE hSession)
{

  if ((hSession < 1UL) ||
      (hSession > KMS_NB_SESSIONS_MAX) ||
      (KMS_GETSESSSION(hSession).state == KMS_SESSION_NOT_USED))
  {
    return   CKR_SESSION_HANDLE_INVALID;
  }
  else
  {
    return   CKR_OK;
  }
}



/**
  * @}
  */

/**
  * @brief  This function is called upon @ref C_GetMechanismInfo call
  * @note   Refer to @ref C_GetMechanismInfo function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  slotID token slot ID
  * @param  type type of mechanism
  * @param  pInfo mechanism information
  * @retval Operation status
  */
CK_RV     KMS_GetMechanismInfo(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo)
{
  CK_RV ret_status = CKR_MECHANISM_INVALID;

  switch (type)
  {
#if defined(KMS_AES_CBC)
    case CKM_AES_CBC:
#endif
#if defined(KMS_AES_CCM)
    case CKM_AES_CCM:
#endif
#if defined(KMS_AES_GCM)
    case CKM_AES_GCM:
#endif
#if defined(KMS_AES_ECB)
    case CKM_AES_ECB:
#endif
#if defined(KMS_AES_CBC) || defined(KMS_AES_CCM) || defined(KMS_AES_GCM) || defined(KMS_AES_ECB)
    {
      pInfo->flags = CKF_ENCRYPT | CKF_DECRYPT;
      pInfo->ulMaxKeySize = 256;
      pInfo->ulMinKeySize = 128;
      ret_status = CKR_OK;
      break;
    }
#endif

#if defined(KMS_DERIVE_KEY) && defined(KMS_AES_ECB)
    case CKM_AES_ECB_ENCRYPT_DATA:
    {
      pInfo->flags = CKF_DERIVE;
      pInfo->ulMaxKeySize = 256;
      pInfo->ulMinKeySize = 128;
      ret_status = CKR_OK;
      break;
    }
#endif

#if defined(KMS_AES_CMAC)
    case CKM_AES_CMAC:
    {
      pInfo->flags = CKF_SIGN | CKF_VERIFY;
      pInfo->ulMaxKeySize = 256;
      pInfo->ulMinKeySize = 128;
      ret_status = CKR_OK;
      break;
    }
#endif

#if defined(KMS_ECDSA)
#if defined(KMS_ECDSA_SHA1)
    case CKM_ECDSA_SHA1:
#endif
#if defined(KMS_ECDSA_SHA256)
    case CKM_ECDSA_SHA256:
#endif
    {
      pInfo->flags = CKF_SIGN | CKF_VERIFY;
      pInfo->ulMaxKeySize = 521;
      pInfo->ulMinKeySize = 192;
      ret_status = CKR_OK;
      break;
    }
#endif

#if defined(KMS_DIGEST)
    case CKM_SHA256:
    case CKM_SHA_1:
    {
      pInfo->flags= CKF_DIGEST;
      pInfo->ulMaxKeySize = 0;
      pInfo->ulMinKeySize = 0;
      ret_status = CKR_OK;
      break;
    }
#endif

#if defined(KMS_RSA)
    case CKM_SHA1_RSA_PKCS:
    case CKM_SHA256_RSA_PKCS:
    {
      pInfo->flags = CKF_SIGN | CKF_VERIFY;
      pInfo->ulMaxKeySize = 2048;
      pInfo->ulMinKeySize = 1024;
      ret_status = CKR_OK;
      break;
    }
#endif

    default:
    {
      pInfo->flags = 0;
      pInfo->ulMaxKeySize = 0;
      pInfo->ulMinKeySize = 0;
      break;
    }
  }

  return ret_status;
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
