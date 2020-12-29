/**
  ******************************************************************************
  * @file    se_interface_kms.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
  *          module access when securely enclaved into Secure Engine
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
/* Place code in a specific section*/
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".SE_IF_Code"
#elif defined(__CC_ARM)
#pragma arm section code = ".SE_IF_Code"
#endif /* __ICCARM__ / __CC_ARM */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "se_low_level.h"               /* This file is part of SE_CoreBin and adapts the Secure Engine
                                           (and its interface) to the STM32 board specificities */
#include "se_interface_common.h"
#include "se_callgate.h"
#include "se_intrinsics.h"
#include "pkcs11.h"
#include "kms_entry.h"

#include "se_interface_kms.h"

extern uint32_t CoreVectorsTableAddr;

/** @addtogroup tKMS User interface to Key Management Services (tKMS)
  * @{
  */

/** @addtogroup KMS_TKMS_SE tKMS Secure Engine access
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_TKMS_SE_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This function is called upon tKMS @ref C_Initialize call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_Initialize function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  pInitArgs
  * @retval Operation status
  */
__root CK_RV SE_KMS_Initialize(CK_VOID_PTR pInitArgs)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_INITIALIZE_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, pInitArgs);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_Finalize call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_Finalize function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  pReserved
  * @retval Operation status
  */
__root CK_RV SE_KMS_Finalize(CK_VOID_PTR pReserved)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_FINALIZE_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, pReserved);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_GetInfo call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GetInfo function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  pInfo
  * @retval Operation status
  */
__root CK_RV SE_KMS_GetInfo(CK_INFO_PTR   pInfo)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_GET_INFO_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, pInfo);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);

  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_GetFunctionList call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GetFunctionList function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  ppFunctionList
  * @retval Operation status
  */
__root CK_RV SE_KMS_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  CK_FUNCTION_LIST_PTR pFunctionList = *ppFunctionList;

  pFunctionList->version.major = 2;
  pFunctionList->version.minor = 40;

  pFunctionList->C_Initialize = SE_KMS_Initialize;
  pFunctionList->C_Finalize = SE_KMS_Finalize;
  pFunctionList->C_GetInfo = SE_KMS_GetInfo;
  pFunctionList->C_GetFunctionList = SE_KMS_GetFunctionList;
  pFunctionList->C_GetSlotList = SE_KMS_GetSlotList;
  pFunctionList->C_GetSlotInfo = NULL;
  pFunctionList->C_GetTokenInfo = SE_KMS_GetTokenInfo;
  pFunctionList->C_GetMechanismList = NULL;
  pFunctionList->C_GetMechanismInfo = NULL;
  pFunctionList->C_InitToken = NULL;
  pFunctionList->C_InitPIN = NULL;
  pFunctionList->C_SetPIN = NULL;
  pFunctionList->C_OpenSession = SE_KMS_OpenSession;
  pFunctionList->C_CloseSession = SE_KMS_CloseSession;
  pFunctionList->C_CloseAllSessions = NULL;
  pFunctionList->C_GetSessionInfo = NULL;
  pFunctionList->C_GetOperationState = NULL;
  pFunctionList->C_SetOperationState = NULL;
  pFunctionList->C_Login = NULL;
  pFunctionList->C_Logout = NULL;
  pFunctionList->C_CreateObject = SE_KMS_CreateObject;
  pFunctionList->C_CopyObject = NULL;
  pFunctionList->C_DestroyObject = SE_KMS_DestroyObject;
  pFunctionList->C_GetObjectSize = NULL;
  pFunctionList->C_GetAttributeValue = SE_KMS_GetAttributeValue;
  pFunctionList->C_SetAttributeValue = SE_KMS_SetAttributeValue;
  pFunctionList->C_FindObjectsInit = SE_KMS_FindObjectsInit ;
  pFunctionList->C_FindObjects = SE_KMS_FindObjects ;
  pFunctionList->C_FindObjectsFinal = SE_KMS_FindObjectsFinal ;
  pFunctionList->C_EncryptInit = SE_KMS_EncryptInit;
  pFunctionList->C_Encrypt = SE_KMS_Encrypt;
  pFunctionList->C_EncryptUpdate = SE_KMS_EncryptUpdate;
  pFunctionList->C_EncryptFinal = SE_KMS_EncryptFinal;
  pFunctionList->C_DecryptInit = SE_KMS_DecryptInit;
  pFunctionList->C_Decrypt = SE_KMS_Decrypt;
  pFunctionList->C_DecryptUpdate = SE_KMS_DecryptUpdate;
  pFunctionList->C_DecryptFinal = SE_KMS_DecryptFinal;
  pFunctionList->C_DigestInit = SE_KMS_DigestInit;
  pFunctionList->C_Digest = SE_KMS_Digest;
  pFunctionList->C_DigestUpdate = SE_KMS_DigestUpdate;
  pFunctionList->C_DigestKey = SE_KMS_DigestKey;
  pFunctionList->C_DigestFinal = SE_KMS_DigestFinal;
  pFunctionList->C_SignInit = SE_KMS_SignInit;
  pFunctionList->C_Sign = SE_KMS_Sign;
  pFunctionList->C_SignUpdate = NULL;
  pFunctionList->C_SignFinal = NULL;
  pFunctionList->C_SignRecoverInit = NULL;
  pFunctionList->C_SignRecover = NULL;
  pFunctionList->C_VerifyInit = SE_KMS_VerifyInit;
  pFunctionList->C_Verify = SE_KMS_Verify;
  pFunctionList->C_VerifyUpdate = NULL;
  pFunctionList->C_VerifyFinal = NULL;
  pFunctionList->C_VerifyFinal = NULL;
  pFunctionList->C_VerifyRecoverInit = NULL;
  pFunctionList->C_VerifyRecover = NULL;
  pFunctionList->C_DigestEncryptUpdate = NULL;
  pFunctionList->C_DecryptDigestUpdate = NULL;
  pFunctionList->C_SignEncryptUpdate = NULL;
  pFunctionList->C_DecryptVerifyUpdate = NULL;
  pFunctionList->C_GenerateKey = NULL;
  pFunctionList->C_GenerateKeyPair = SE_KMS_GenerateKeyPair;
  pFunctionList->C_WrapKey = NULL;
  pFunctionList->C_UnwrapKey = NULL;
  pFunctionList->C_DeriveKey = SE_KMS_DeriveKey;
  pFunctionList->C_SeedRandom = NULL;
  pFunctionList->C_GenerateRandom = SE_KMS_GenerateRandom;
  pFunctionList->C_GetFunctionStatus = NULL;
  pFunctionList->C_CancelFunction = NULL;
  pFunctionList->C_WaitForSlotEvent = NULL;

  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_GetSlotList call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GetSlotList function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  tokenPresent
  * @param  pSlotList
  * @param  pulCount
  * @retval Operation status
  */
__root CK_RV SE_KMS_GetSlotList(CK_BBOOL tokenPresent, CK_SLOT_ID_PTR pSlotList,
                                CK_ULONG_PTR pulCount)
{

  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_GET_SLOT_LIST_FCT_ID,
                    (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, tokenPresent, pSlotList, pulCount);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);

  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_GetSlotInfo call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GetSlotInfo function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  slotID
  * @param  pInfo
  * @retval Operation status
  */
__root CK_RV SE_KMS_GetSlotInfo(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_GET_SLOT_INFO_FCT_ID,
                    (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    slotID,
                    pInfo);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_GetTokenInfo call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GetTokenInfo function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  slotID
  * @param  pInfo
  * @retval Operation status
  */
__root CK_RV SE_KMS_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_GET_TOKEN_INFO_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, slotID,
                    pInfo);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_GetMechanismInfo  call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GetMechanismInfo  function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  slotID
  * @param  type
  * @param  pInfo
  * @retval Operation status
  */
__root CK_RV SE_KMS_GetMechanismInfo(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_GET_MECHANISM_INFO_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, slotID, type, pInfo);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_InitToken call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_InitToken function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  slotID
  * @param  pPin
  * @param  ulPinLen
  * @param  pLabel
  * @retval Operation status
  */
__root CK_RV SE_KMS_InitToken(CK_SLOT_ID slotID,  CK_UTF8CHAR_PTR pPin,
                              CK_ULONG ulPinLen,  CK_UTF8CHAR_PTR pLabel)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_INIT_TOKEN_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, slotID, pPin,
                    ulPinLen, pLabel);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_OpenSession call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_OpenSession function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  slotID
  * @param  flags
  * @param  pApplication
  * @param  Notify
  * @param  phSession
  * @retval Operation status
  */
__root CK_RV SE_KMS_OpenSession(CK_SLOT_ID slotID, CK_FLAGS flags,
                                CK_VOID_PTR pApplication, CK_NOTIFY Notify,
                                CK_SESSION_HANDLE_PTR phSession)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_OPEN_SESSION_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    slotID, flags, pApplication, Notify, phSession);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_CloseSession call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_CloseSession function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @retval Operation status
  */
__root CK_RV SE_KMS_CloseSession(CK_SESSION_HANDLE hSession)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_CLOSE_SESSION_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, hSession);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_CreateObject call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_CreateObject function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pTemplate
  * @param  ulCount
  * @param  phObject
  * @retval Operation status
  */
__root CK_RV SE_KMS_CreateObject(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate,
                                 CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phObject)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_CREATE_OBJECT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pTemplate, ulCount, phObject);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_DestroyObject call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DestroyObject function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  hObject
  * @retval Operation status
  */
__root CK_RV SE_KMS_DestroyObject(CK_SESSION_HANDLE hSession,
                                  CK_OBJECT_HANDLE hObject)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DESTROY_OBJECT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, hObject);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_GetAttributeValue call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GetAttributeValue function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  hObject
  * @param  pTemplate
  * @param  ulCount
  * @retval Operation status
  */
__root CK_RV SE_KMS_GetAttributeValue(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject,
                                      CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_GET_ATTRIBUTE_VALUE_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, hObject, pTemplate, ulCount);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_SetAttributeValue call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_SetAttributeValue function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  hObject
  * @param  pTemplate
  * @param  ulCount
  * @retval Operation status
  */
__root CK_RV SE_KMS_SetAttributeValue(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject,
                                      CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_SET_ATTRIBUTE_VALUE_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, hObject, pTemplate, ulCount);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_FindObjectsInit call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_FindObjectsInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pTemplate
  * @param  ulCount
  * @retval Operation status
  */
__root CK_RV SE_KMS_FindObjectsInit(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate,
                                    CK_ULONG ulCount)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_FIND_OBJECTS_INIT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pTemplate, ulCount);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_FindObjects call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_FindObjects function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  phObject
  * @param  ulMaxObjectCount
  * @param  pulObjectCount
  * @retval Operation status
  */
__root CK_RV SE_KMS_FindObjects(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE_PTR phObject,
                                CK_ULONG ulMaxObjectCount,  CK_ULONG_PTR pulObjectCount)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_FIND_OBJECTS_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, phObject, ulMaxObjectCount, pulObjectCount);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_FindObjectsFinal call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_FindObjectsFinal function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @retval Operation status
  */
__root CK_RV SE_KMS_FindObjectsFinal(CK_SESSION_HANDLE hSession)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_FIND_OBJECTS_FINAL_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_EncryptInit call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_EncryptInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pMechanism
  * @param  hKey
  * @retval Operation status
  */
__root CK_RV SE_KMS_EncryptInit(CK_SESSION_HANDLE hSession,
                                CK_MECHANISM_PTR  pMechanism, CK_OBJECT_HANDLE  hKey)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_ENCRYPT_INIT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pMechanism, hKey);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_Encrypt call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_Encrypt function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pData
  * @param  ulDataLen
  * @param  pEncryptedData
  * @param  pulEncryptedDataLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_Encrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                            CK_ULONG  ulDataLen, CK_BYTE_PTR  pEncryptedData,
                            CK_ULONG_PTR      pulEncryptedDataLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_ENCRYPT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pData, ulDataLen, pEncryptedData,
                    pulEncryptedDataLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_EncryptUpdate call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_EncryptUpdate function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pPart
  * @param  ulPartLen
  * @param  pEncryptedPart
  * @param  pulEncryptedPartLen
  * @retval Operation status
  */

__root CK_RV SE_KMS_EncryptUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart,
                                  CK_ULONG          ulPartLen,
                                  CK_BYTE_PTR       pEncryptedPart,
                                  CK_ULONG_PTR      pulEncryptedPartLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_ENCRYPT_UPDATE_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pPart, ulPartLen, pEncryptedPart,
                    pulEncryptedPartLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_EncryptFinal call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_EncryptFinal function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pLastEncryptedPart
  * @param  pulLastEncryptedPartLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_EncryptFinal(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR       pLastEncryptedPart,
                                 CK_ULONG_PTR      pulLastEncryptedPartLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_ENCRYPT_FINAL_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pLastEncryptedPart, pulLastEncryptedPartLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_DecryptInit call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DecryptInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pMechanism
  * @param  hKey
  * @retval Operation status
  */
__root CK_RV SE_KMS_DecryptInit(CK_SESSION_HANDLE hSession,
                                CK_MECHANISM_PTR  pMechanism,
                                CK_OBJECT_HANDLE  hKey)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DECRYPT_INIT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pMechanism, hKey);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_Decrypt call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_Decrypt function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pEncryptedData
  * @param  ulEncryptedDataLen
  * @param  pData
  * @param  pulDataLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_Decrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedData,
                            CK_ULONG  ulEncryptedDataLen, CK_BYTE_PTR  pData,
                            CK_ULONG_PTR pulDataLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DECRYPT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pEncryptedData, ulEncryptedDataLen,
                    pData, pulDataLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_DecryptUpdate call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DecryptUpdate function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pEncryptedPart
  * @param  ulEncryptedPartLen
  * @param  pPart
  * @param  pulPartLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_DecryptUpdate(CK_SESSION_HANDLE hSession,
                                  CK_BYTE_PTR       pEncryptedPart,
                                  CK_ULONG          ulEncryptedPartLen,
                                  CK_BYTE_PTR       pPart,
                                  CK_ULONG_PTR      pulPartLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DECRYPT_UPDATE_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pEncryptedPart, ulEncryptedPartLen,
                    pPart, pulPartLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_DecryptFinal call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DecryptFinal function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pLastPart
  * @param  pulLastPartLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_DecryptFinal(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR       pLastPart,
                                 CK_ULONG_PTR      pulLastPartLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DECRYPT_FINAL_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pLastPart, pulLastPartLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}



/**
  * @brief  This function is called upon tKMS @ref C_DigestInit call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DigestInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pMechanism
  * @retval Operation status
  */
__root CK_RV SE_KMS_DigestInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DIGEST_INIT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pMechanism);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}




/**
  * @brief  This function is called upon tKMS @ref C_Digest call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_Digest function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pData
  * @param  ulDataLen
  * @param  pDigest
  * @param  pulDigestLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_Digest(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                           CK_ULONG ulDataLen, CK_BYTE_PTR pDigest,
                           CK_ULONG_PTR pulDigestLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DIGEST_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pData, ulDataLen, pDigest, pulDigestLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_DigestUpdate call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DigestUpdate function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pPart
  * @param  ulPartLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_DigestUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DIGEST_UPDATE_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pPart, ulPartLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_DigestKey call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DigestKey function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  hKey
  * @retval Operation status
  */
__root CK_RV SE_KMS_DigestKey(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hKey)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DIGEST_KEY_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, hKey);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_DigestFinal call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DigestFinal function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pDigest
  * @param  pulDigestLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_DigestFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pDigest,
                                CK_ULONG_PTR pulDigestLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DIGEST_FINAL_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pDigest, pulDigestLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}



/**
  * @brief  This function is called upon tKMS @ref C_SignInit call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_SignInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pMechanism
  * @param  hKey
  * @retval Operation status
  */
__root CK_RV SE_KMS_SignInit(CK_SESSION_HANDLE hSession,
                             CK_MECHANISM_PTR  pMechanism,
                             CK_OBJECT_HANDLE  hKey)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_SIGN_INIT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pMechanism, hKey);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_Sign call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_Sign function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pData
  * @param  ulDataLen
  * @param  pSignature
  * @param  pulSignatureLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_Sign(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                         CK_ULONG  ulDataLen, CK_BYTE_PTR  pSignature,
                         CK_ULONG_PTR pulSignatureLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_SIGN_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pData, ulDataLen,
                    pSignature, pulSignatureLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called upon tKMS @ref C_VerifyInit call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_VerifyInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pMechanism
  * @param  hKey
  * @retval Operation status
  */
__root CK_RV SE_KMS_VerifyInit(CK_SESSION_HANDLE hSession,
                               CK_MECHANISM_PTR  pMechanism,
                               CK_OBJECT_HANDLE  hKey)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */
  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_VERIFY_INIT_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pMechanism, hKey);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_Verify call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_Verify function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pData
  * @param  ulDataLen
  * @param  pSignature
  * @param  ulSignatureLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_Verify(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                           CK_ULONG  ulDataLen, CK_BYTE_PTR  pSignature,
                           CK_ULONG  ulSignatureLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_VERIFY_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pData, ulDataLen,
                    pSignature, ulSignatureLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}


/**
  * @brief  This function is called to call KMS service that will authenticate,
  *         verify and decrypt a blob to update NVM static ID keys
  * @param  pData Pointer to encrypted blob header
  * @param  pImportBlobState Blob importation result
  * @retval Operation status
  */
__root CK_RV SE_KMS_ImportBlob(CK_BYTE_PTR pData, CK_ULONG_PTR pImportBlobState)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_IMPORT_BLOB_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    pData, pImportBlobState);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_DeriveKey call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_DeriveKey function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pMechanism
  * @param  hBaseKey
  * @param  pTemplate
  * @param  ulAttributeCount
  * @param  phKey
  * @retval Operation status
  */
__root CK_RV SE_KMS_DeriveKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                              CK_OBJECT_HANDLE hBaseKey, CK_ATTRIBUTE_PTR  pTemplate,
                              CK_ULONG  ulAttributeCount, CK_OBJECT_HANDLE_PTR  phKey)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_DERIVE_KEY_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pMechanism, hBaseKey, pTemplate, ulAttributeCount, phKey);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_GenerateKeyPair call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GenerateKeyPair function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pMechanism
  * @param  pPublicKeyTemplate
  * @param  ulPublicKeyAttributeCount
  * @param  pPrivateKeyTemplate
  * @param  ulPrivateKeyAttributeCount
  * @param  phPublicKey
  * @param  phPrivateKey
  * @retval Operation status
  */
__root CK_RV SE_KMS_GenerateKeyPair(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                                    CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG  ulPublicKeyAttributeCount,
                                    CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount,
                                    CK_OBJECT_HANDLE_PTR phPublicKey, CK_OBJECT_HANDLE_PTR phPrivateKey)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_GENERATE_KEYPAIR_FCT_ID,
                    (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit, hSession,  pMechanism,
                    pPublicKeyTemplate,   ulPublicKeyAttributeCount,  pPrivateKeyTemplate,
                    ulPrivateKeyAttributeCount, phPublicKey,  phPrivateKey);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

/**
  * @brief  This function is called upon tKMS @ref C_GenerateRandom call
  *         to call appropriate KMS service through Secure Engine Callgate
  * @note   Refer to @ref C_GenerateRandom function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession
  * @param  pRandomData
  * @param  ulRandomLen
  * @retval Operation status
  */
__root CK_RV SE_KMS_GenerateRandom(CK_SESSION_HANDLE hSession, CK_BYTE_PTR  pRandomData,
                                   CK_ULONG  ulRandomLen)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_GENERATE_RANDOM_FCT_ID,
                    (SE_StatusTypeDef *)&ck_rv_ret_status, primask_bit,
                    hSession, pRandomData, ulRandomLen);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}

#ifdef KMS_UNITARY_TEST
/**
  * @brief  This function is called to execute KMS unitary tests
  * @param  ulTestID;
  * @param  plNbPassed;
  * @param  plNbFailed;
  * @retval Operation status
  */
__root CK_RV SE_KMS_UnitaryTest(CK_ULONG ulTestID, CK_ULONG_PTR plNbPassed, CK_ULONG_PTR plNbFailed)
{
  CK_RV ck_rv_ret_status = CKR_OK;
  uint32_t primask_bit; /*!< interruption mask saved when disabling ITs then restore when re-enabling ITs */

  /* Set the CallGate function pointer */
  SET_CALLGATE();

  /*Enter Secure Mode*/
  SE_EnterSecureMode(&primask_bit);

  /*Secure Engine Call*/
  (*SE_CallGatePtr)((SE_FunctionIDTypeDef)KMS_UNITARY_TEST_FCT_ID, (SE_StatusTypeDef *)&ck_rv_ret_status,
                    ulTestID, plNbPassed, plNbFailed);

  /*Exit Secure Mode*/
  SE_ExitSecureMode(primask_bit);


  return ck_rv_ret_status;
}
#endif /* KMS_UNITARY_TEST */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_function_attributes =
#elif defined(__CC_ARM)
#pragma arm section code
#endif /* __ICCARM__ / __CC_ARM */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
