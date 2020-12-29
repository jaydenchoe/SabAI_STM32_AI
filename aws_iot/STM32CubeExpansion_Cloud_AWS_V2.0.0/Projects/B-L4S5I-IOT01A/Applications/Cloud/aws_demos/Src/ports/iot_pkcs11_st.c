/*
 * Amazon FreeRTOS PKCS #11 V2.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_pkcs11_st.c
 * @brief PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS #11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "iot_pkcs11_config.h"
#include "iot_pkcs11.h"
#include "task.h"
#include "semphr.h"

/* SE Interface PKCS11 */
#include "se_interface_kms.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

//#define PKCS11_DEBUG_PRINT( X )            vLoggingPrintf X
#define PKCS11_DEBUG_PRINT( X )
#define PKCS11_WARNING_PRINT( X )    /* vLoggingPrintf X */

/*-----------------------------------------------------------*/
/* The global PKCS #11 mutex.*/
static SemaphoreHandle_t xP11Mutex;
/*-----------------------------------------------------------*/

/**
 * @brief EnterCriticalSection.
 */
void prvEnterSecureSection(void)
{
  /* Mutex - if it does not exist, create it.*/
  taskENTER_CRITICAL();

  if(xP11Mutex == NULL)
  {
    xP11Mutex = xSemaphoreCreateMutex();
  }

  taskEXIT_CRITICAL();

  /* Mutex - Take semaphore.*/
  xSemaphoreTake( xP11Mutex, portMAX_DELAY );

  taskENTER_CRITICAL();
}

/**
 * @brief ExitCriticalSection.
 */
void prvExitSecureSection(void)
{
  /* Mutex - release Semaphore   */
  xSemaphoreGive( xP11Mutex );

  taskEXIT_CRITICAL();

}

/*-----------------------------------------------------------*/
/*
 * PKCS#11 module implementation.
 */

/**
 * @brief PKCS#11 interface functions implemented by this Cryptoki module.
 */
static CK_FUNCTION_LIST prvP11FunctionList =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize,
    C_Finalize,
    NULL, /*C_GetInfo */
    C_GetFunctionList,
    C_GetSlotList,
    NULL, /*C_GetSlotInfo*/
    C_GetTokenInfo,
    NULL, /*C_GetMechanismList*/
    C_GetMechanismInfo,
    C_InitToken,
    NULL, /*C_InitPIN*/
    NULL, /*C_SetPIN*/
    C_OpenSession,
    C_CloseSession,
    NULL,    /*C_CloseAllSessions*/
    NULL,    /*C_GetSessionInfo*/
    NULL,    /*C_GetOperationState*/
    NULL,    /*C_SetOperationState*/
    C_Login, /*C_Login*/
    NULL,    /*C_Logout*/
    C_CreateObject,
    NULL,    /*C_CopyObject*/
    C_DestroyObject,
    NULL,    /*C_GetObjectSize*/
    C_GetAttributeValue,
    NULL,    /*C_SetAttributeValue*/
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    NULL, /*C_EncryptInit*/
    NULL, /*C_Encrypt*/
    NULL, /*C_EncryptUpdate*/
    NULL, /*C_EncryptFinal*/
    NULL, /*C_DecryptInit*/
    NULL, /*C_Decrypt*/
    NULL, /*C_DecryptUpdate*/
    NULL, /*C_DecryptFinal*/
    C_DigestInit,
    NULL, /*C_Digest*/
    C_DigestUpdate,
    NULL, /* C_DigestKey*/
    C_DigestFinal,
    C_SignInit,
    C_Sign,
    NULL, /*C_SignUpdate*/
    NULL, /*C_SignFinal*/
    NULL, /*C_SignRecoverInit*/
    NULL, /*C_SignRecover*/
    C_VerifyInit,
    C_Verify,
    NULL, /*C_VerifyUpdate*/
    NULL, /*C_VerifyFinal*/
    NULL, /*C_VerifyRecoverInit*/
    NULL, /*C_VerifyRecover*/
    NULL, /*C_DigestEncryptUpdate*/
    NULL, /*C_DecryptDigestUpdate*/
    NULL, /*C_SignEncryptUpdate*/
    NULL, /*C_DecryptVerifyUpdate*/
    NULL, /*C_GenerateKey*/
    C_GenerateKeyPair,
    NULL, /*C_WrapKey*/
    NULL, /*C_UnwrapKey*/
    NULL, /*C_DeriveKey*/
    NULL, /*C_SeedRandom*/
    C_GenerateRandom,
    NULL, /*C_GetFunctionStatus*/
    NULL, /*C_CancelFunction*/
    NULL  /*C_WaitForSlotEvent*/
};

/*-------------------------------------------------------------*/

#if !defined( pkcs11configC_INITIALIZE_ALT )

/**
 * @brief Initialize the PKCS #11 module for use.
 *
 * @note C_Initialize is not thread-safe.
 *
 * C_Initialize should be called (and allowed to return) before
 * any additional PKCS #11 operations are invoked.
 *
 * In this implementation, all arguments are ignored.
 * Thread protection for the rest of PKCS #11 functions
 * default to FreeRTOS primitives.
 *
 * @param[in] pvInitArgs    This parameter is ignored.
 *
 * @return CKR_OK if successful.
 * CKR_CRYPTOKI_ALREADY_INITIALIZED if C_Initialize was previously called.
 * All other errors indicate that the PKCS #11 module is not ready to be used.
 * See <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
    CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
    { /*lint !e9072 It's OK to have different parameter name. */

          CK_RV xResult = CKR_OK;

          PKCS11_DEBUG_PRINT( ( "[PKCS11] C_Initialize \r\n" ) );

          /*Enter Secure Section*/
          prvEnterSecureSection();

          /*Secure Engine Call*/
          xResult = SE_KMS_Initialize(pvInitArgs);

          /* Add here the handling of CKR_CANT_LOCK return value.
             SE_KMS implementation can't lock since it is outside of FreeRTOS space.
             But the wrapper could take care of it handling a semaphore -> Therefore returning CKR_OK.
          */

          /*Exit Secure Section*/
          prvExitSecureSection();

        return xResult;
    }
#endif /* if !defined( pkcs11configC_INITIALIZE_ALT ) */

/**
 * @brief Un-initialize the Cryptoki module.
 */
    CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pvReserved )
    {
      CK_RV xResult = CKR_OK;

      PKCS11_DEBUG_PRINT( ( "[PKCS11] C_Finalize \r\n" ) );

      /*Enter Secure Section*/
      prvEnterSecureSection();

      /*Secure Engine Call*/
      xResult = SE_KMS_Finalize(pvReserved);

      /*Exit Secure Section*/
      prvExitSecureSection();

      return xResult;
    }

/**
 * @brief Obtain a pointer to the PKCS #11 module's list
 * of function pointers.
 *
 * All other PKCS #11 functions should be invoked using the returned
 * function list.
 *
 * \warn Do not overwrite the function list.
 *
 * \param[in] ppxFunctionList       Pointer to the location where
 *                                  pointer to function list will be placed.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{ /*lint !e9072 It's OK to have different parameter name. */

  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_GetFunctionList \r\n" ) );

  if( NULL == ppxFunctionList )
  {
    xResult = CKR_ARGUMENTS_BAD;
  }
  else
  {
    *ppxFunctionList = &prvP11FunctionList;
  }

  return xResult;

}

/**
 * @brief Query the list of slots. A single default slot is implemented.
 *
 * This port does not implement the concept of separate slots/tokens.
 *
 *  \param[in]  xTokenPresent   This parameter is unused by this port.
 *  \param[in]  pxSlotList      Pointer to an array of slot IDs.
 *                              At this time, only 1 slot is implemented.
 *  \param[in,out]  pulCount    Length of the slot list pxSlotList. Updated
 *                              to contain the actual number of slots written
 *                              to the list.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetSlotList )( CK_BBOOL xTokenPresent,
                                           CK_SLOT_ID_PTR pxSlotList,
                                           CK_ULONG_PTR pulCount )
{ /*lint !e9072 It's OK to have different parameter name. */
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_GetSlotList \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_GetSlotList(xTokenPresent,pxSlotList,pulCount);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}


/**
 * @brief This function is not implemented for this port.
 *
 * C_GetTokenInfo() is only implemented for compatibility with other ports.
 * All inputs to this function are ignored, and calling this
 * function on this port does provide any information about
 * the PKCS #11 token.
 *
 * @return CKR_OK.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetTokenInfo )(
                                            CK_SLOT_ID slotID,
                                            CK_TOKEN_INFO_PTR pInfo )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_GetTokenInfo \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_GetTokenInfo(slotID,pInfo);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}


/**
 * @brief This function is not implemented for this port.
 *
 * C_InitToken() is only implemented for compatibility with other ports.
 * All inputs to this function are ignored, and calling this
 * function on this port does not add any security.
 *
 * @return CKR_OK.
 */
CK_DEFINE_FUNCTION( CK_RV, C_InitToken )(
    CK_SLOT_ID slotID,
    CK_UTF8CHAR_PTR pPin,
    CK_ULONG ulPinLen,
    CK_UTF8CHAR_PTR pLabel )
{
    PKCS11_DEBUG_PRINT( ( "[PKCS11] C_InitToken \r\n" ) );

    /* Avoid compiler warnings about unused variables. */
    ( void ) slotID;
    ( void ) pPin;
    ( void ) ulPinLen;
    ( void ) pLabel;

    return CKR_OK;
}

/**
 * @brief This function is not implemented for this port.
 *
 * C_GetMechanismInfo() is only implemented for compatibility with other ports.
 * All inputs to this function are ignored, and calling this
 * function on this port does not add any security.
 *
 * @return CKR_OK.
 */
#define LOCAL_GETMECHANISMINFO
CK_DEFINE_FUNCTION( CK_RV, C_GetMechanismInfo)(
  CK_SLOT_ID            slotID,
  CK_MECHANISM_TYPE     type,
  CK_MECHANISM_INFO_PTR pInfo)
{
    CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_GetMechanismInfo \r\n" ) );

#ifdef LOCAL_GETMECHANISMINFO
  /* Avoid compiler warnings about unused variables. */
  ( void ) slotID;

   switch (type)
  {
    case CKM_ECDSA:
    {
      xResult = CKR_OK;
      pInfo->flags= CKF_SIGN | CKF_VERIFY;
      pInfo->ulMaxKeySize = 256;
      pInfo->ulMinKeySize = 256;
      break;
    }

    case CKM_EC_KEY_PAIR_GEN:
    {
      xResult = CKR_OK;
      pInfo->flags= CKF_GENERATE_KEY_PAIR;
      pInfo->ulMaxKeySize = 256;
      pInfo->ulMinKeySize = 256;
      break;
    }

    case CKM_SHA256:
    {
      xResult = CKR_OK;
      pInfo->flags= CKF_DIGEST;
      break;
    }

    default:
      xResult = CKR_MECHANISM_INVALID;
      break;
  }
#else

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_GetMechanismInfo(slotID,type,pInfo);

  /*Exit Secure Section*/
  prvExitSecureSection();
#endif
  return xResult;
}

/**
 * @brief Start a session for a cryptographic command sequence.
 *
 * \note PKCS #11 module must have been previously initialized with a call to
 * C_Initialize() before calling C_OpenSession().
 *
 *
 *  \param[in]  xSlotID         This parameter is unused in this port.
 *  \param[in]  xFlags          Session flags - CKF_SERIAL_SESSION is a
 *                              mandatory flag.
 *  \param[in]  pvApplication   This parameter is unused in this port.
 *  \param[in]  xNotify         This parameter is unused in this port.
 *  \param[in]  pxSession       Pointer to the location that the created
 *                              session's handle will be placed.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_OpenSession )( CK_SLOT_ID xSlotID,
                                            CK_FLAGS xFlags,
                                            CK_VOID_PTR pvApplication,
                                            CK_NOTIFY xNotify,
                                            CK_SESSION_HANDLE_PTR pxSession )
{ /*lint !e9072 It's OK to have different parameter name. */
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_OpenSession \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_OpenSession(xSlotID,xFlags,pvApplication,xNotify,pxSession);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Terminate a session and release resources.
 *
 * @param[in]   xSession        The session handle to
 *                              be terminated.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CloseSession )( CK_SESSION_HANDLE xSession )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_CloseSession \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_CloseSession(xSession);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}


/**
 * @brief This function is not implemented for this port.
 *
 * C_Login() is only implemented for compatibility with other ports.
 * All inputs to this function are ignored, and calling this
 * function on this port does not add any security.
 *
 * @return CKR_OK.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Login )( CK_SESSION_HANDLE hSession,
                                      CK_USER_TYPE userType,
                                      CK_UTF8CHAR_PTR pPin,
                                      CK_ULONG ulPinLen )
{
   PKCS11_DEBUG_PRINT( ( "[PKCS11] C_Login \r\n" ) );

   /* Avoid warnings about unused parameters. */
    ( void ) hSession;
    ( void ) userType;
    ( void ) pPin;
    ( void ) ulPinLen;

    /* THIS FUNCTION IS NOT IMPLEMENTED.
     * Defined for compatibility with other PKCS #11 ports. */
    return CKR_OK;
}


/**
 * @brief Create a PKCS #11 public key object
 * by importing it into device storage.
 *
 * @param[in] xSession                   Handle of a valid PKCS #11 session.
 * @param[in] pxTemplate                 List of attributes of the object to
 *                                       be created.
 * @param[in] ulCount                    Number of attributes in pxTemplate.
 * @param[out] pxObject                  Pointer to the location where the created
 *                                       object's handle will be placed.
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE xSession,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulCount,
                                             CK_OBJECT_HANDLE_PTR pxObject )
{ /*lint !e9072 It's OK to have different parameter name. */
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_CreateObject \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_CreateObject(xSession,pxTemplate,ulCount,pxObject);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Destroy an object.
 *
 * @param[in] xSession                   Handle of a valid PKCS #11 session.
 * @param[in] xObject                    Handle of the object to be destroyed.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_DestroyObject \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_DestroyObject(xSession,xObject);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Query the value of the specified cryptographic object attribute.
 * @param[in] xSession                   Handle of a valid PKCS #11 session.
 * @param[in] xObject                    PKCS #11 object handle to be queried.
 * @param[in,out] pxTemplate             Attribute template.
 *                                       pxTemplate.pValue should be set to the attribute
 *                                       to be queried. pxTemplate.ulValueLen should be
 *                                       set to the length of the buffer allocated at
 *                                       pxTemplate.pValue, and will be updated to contain
 *                                       the actual length of the data copied.
 *                                       pxTemplate.pValue should be set to point to
 *                                       a buffer to receive the attribute value data.
 *                                       If parameter length is unknown,
 *                                       pxTemplate.pValue may be set to NULL, and
 *                                       this function will set the required buffer length
 *                                       in pxTemplate.ulValueLen.
 * @param[in] ulCount                    The number of attributes in the template.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetAttributeValue )( CK_SESSION_HANDLE xSession,
                                                  CK_OBJECT_HANDLE xObject,
                                                  CK_ATTRIBUTE_PTR pxTemplate,
                                                  CK_ULONG ulCount )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_GetAttributeValue \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_GetAttributeValue(xSession,xObject,pxTemplate,ulCount);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Initializes a search for an object by its label.
 *
 * \sa C_FindObjects() and C_FindObjectsFinal() which must be called
 * after C_FindObjectsInit().
 *
 * \note FindObjects parameters are shared by a session.  Calling
 * C_FindObjectsInit(), C_FindObjects(), and C_FindObjectsFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pxTemplate                    Pointer to a template which specifies
 *                                          the object attributes to match.
 *                                          In this port, the only searchable attribute
 *                                          is object label.  All other attributes will
 *                                          be ignored.
 * @param[in] ulCount                       The number of attributes in pxTemplate.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsInit )( CK_SESSION_HANDLE xSession,
                                                CK_ATTRIBUTE_PTR pxTemplate,
                                                CK_ULONG ulCount )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_FindObjectsInit (Session = %d, ulCount=%d) \r\n", xSession, ulCount ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_FindObjectsInit(xSession,pxTemplate,ulCount);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Find an object.
 *
 * \sa C_FindObjectsInit() which must be called before calling C_FindObjects()
 * and C_FindObjectsFinal(), which must be called after.
 *
 * \note FindObjects parameters are shared by a session.  Calling
 * C_FindObjectsInit(), C_FindObjects(), and C_FindObjectsFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[out] pxObject                     Points to the handle of the object to
 *                                          be found.
 * @param[in] ulMaxObjectCount              The size of the pxObject object handle
 *                                          array. In this port, this value should
 *                                          always be set to 1, as searching for
 *                                          multiple objects is not supported.
 * @param[out] pulObjectCount               The actual number of objects that are
 *                                          found. In this port, if an object is found
 *                                          this value will be 1, otherwise if the
 *                                          object is not found, it will be set to 0.
 *
 * \note In the event that an object does not exist, CKR_OK will be returned, but
 * pulObjectCount will be set to 0.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjects )( CK_SESSION_HANDLE xSession,
                                            CK_OBJECT_HANDLE_PTR pxObject,
                                            CK_ULONG ulMaxObjectCount,
                                            CK_ULONG_PTR pulObjectCount )
{ /*lint !e9072 It's OK to have different parameter name. */
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_FindObjects (Session = %d) \r\n", xSession ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_FindObjects(xSession,pxObject,ulMaxObjectCount,pulObjectCount);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Completes an object search operation.
 *
 * \sa C_FindObjectsInit(), C_FindObjects() which must be called before
 * calling C_FindObjectsFinal().
 *
 * \note FindObjects parameters are shared by a session.  Calling
 * C_FindObjectsInit(), C_FindObjects(), and C_FindObjectsFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsFinal )( CK_SESSION_HANDLE xSession )
{ /*lint !e9072 It's OK to have different parameter name. */
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_FindObjectsFinal \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_FindObjectsFinal(xSession);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Begins a digest (hash) operation.
 *
 * \sa C_DigestUpdate(), C_DigestFinal()
 *
 * \note Digest parameters are shared by a session.  Calling
 * C_DigestInit(), C_DigestUpdate(), and C_DigestFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pMechanism                    Digesting mechanism.  This port only supports
 *                                          the mechanism CKM_SHA256.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DigestInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pMechanism )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_DigestInit \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_DigestInit(xSession,pMechanism);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}


/**
 * @brief Continues a multi-part digest (hash) operation.
 *
 * \sa C_DigestInit(), C_DigestFinal()
 *
 * \note Digest parameters are shared by a session.  Calling
 * C_DigestInit(), C_DigestUpdate(), and C_DigestFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pPart                         Pointer to the data to be added to the digest.
 * @param[in] ulPartLen                     Length of the data located at pPart.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DigestUpdate )( CK_SESSION_HANDLE xSession,
                                             CK_BYTE_PTR pPart,
                                             CK_ULONG ulPartLen )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_DigestUpdate \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_DigestUpdate(xSession,pPart,ulPartLen);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Complete a multi-part digest (hash) operation.
 *
 * \sa C_DigestInit(), C_DigestUpdate()
 *
 * \note Digest parameters are shared by a session.  Calling
 * C_DigestInit(), C_DigestUpdate(), and C_DigestFinal() with the
 * same session across different tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[out] pDigest                      Pointer to the location that receives
 *                                          the message digest.  Memory must be allocated
 *                                          by the caller.                                          Caller is responsible for allocating memory.
 *                                          Providing NULL for this input will cause
 *                                          pulDigestLen to be updated for length of
 *                                          buffer required.
 * @param[in,out] pulDigestLen              Points to the location that holds the length
 *                                          of the message digest.  If pDigest is NULL,
 *                                          this value is updated to contain the length
 *                                          of the buffer needed to hold the digest. Else
 *                                          it is updated to contain the actual length of
 *                                          the digest placed in pDigest.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DigestFinal )( CK_SESSION_HANDLE xSession,
                                            CK_BYTE_PTR pDigest,
                                            CK_ULONG_PTR pulDigestLen )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_DigestFinal \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_DigestFinal(xSession,pDigest,pulDigestLen);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Begin creating a digital signature.
 *
 * \sa C_Sign() completes signatures initiated by C_SignInit().
 *
 * \note C_Sign() parameters are shared by a session.  Calling
 * C_SignInit() & C_Sign() with the same session across different
 * tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pxMechanism                   Mechanism used to sign.
 *                                          This port supports the following mechanisms:
 *                                          - CKM_RSA_PKCS for RSA signatures
 *                                          - CKM_ECDSA for elliptic curve signatures
 *                                          Note that neither of these mechanisms perform
 *                                          hash operations.
 * @param[in] xKey                          The handle of the private key to be used for
 *                                          signature. Key must be compatible with the
 *                                          mechanism chosen by pxMechanism.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_SignInit )( CK_SESSION_HANDLE xSession,
                                         CK_MECHANISM_PTR pxMechanism,
                                         CK_OBJECT_HANDLE xKey )
{
  CK_RV xResult = CKR_OK;


  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_SignInit (Session = %d, KeyHandle =%d) \r\n", xSession, xKey ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_SignInit(xSession,pxMechanism,xKey);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Performs a digital signature operation.
 *
 * \sa C_SignInit() initiates signatures signature creation.
 *
 * \note C_Sign() parameters are shared by a session.  Calling
 * C_SignInit() & C_Sign() with the same session across different
 * tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pucData                       Data to be signed.
 *                                          Note: Some applications may require this data to
 *                                          be hashed before passing to C_Sign().
 * @param[in] ulDataLen                     Length of pucData, in bytes.
 * @param[out] pucSignature                 Buffer where signature will be placed.
 *                                          Caller is responsible for allocating memory.
 *                                          Providing NULL for this input will cause
 *                                          pulSignatureLen to be updated for length of
 *                                          buffer required.
 * @param[in,out] pulSignatureLen           Length of pucSignature buffer.
 *                                          If pucSignature is non-NULL, pulSignatureLen is
 *                                          updated to contain the actual signature length.
 *                                          If pucSignature is NULL, pulSignatureLen is
 *                                          updated to the buffer length required for signature
 *                                          data.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Sign )( CK_SESSION_HANDLE xSession,
                                     CK_BYTE_PTR pucData,
                                     CK_ULONG ulDataLen,
                                     CK_BYTE_PTR pucSignature,
                                     CK_ULONG_PTR pulSignatureLen )
{ /*lint !e9072 It's OK to have different parameter name. */
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_Sign \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_Sign(xSession,pucData,ulDataLen,pucSignature,pulSignatureLen);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Begin a digital signature verification.
 *
 * \sa C_Verify() completes verifications initiated by C_VerifyInit().
 *
 * \note C_Verify() parameters are shared by a session.  Calling
 * C_VerifyInit() & C_Verify() with the same session across different
 * tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pxMechanism                   Mechanism used to verify signature.
 * @param[in] xKey                          The handle of the public key to be used for
 *                                          verification. Key must be compatible with the
 *                                          mechanism chosen by pxMechanism.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_VerifyInit )( CK_SESSION_HANDLE xSession,
                                           CK_MECHANISM_PTR pxMechanism,
                                           CK_OBJECT_HANDLE xKey )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_VerifyInit \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_VerifyInit(xSession,pxMechanism,xKey);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Verifies a digital signature.
 *
 * \note C_VerifyInit() must have been called previously.
 *
 * \note C_Verify() parameters are shared by a session.  Calling
 * C_VerifyInit() & C_Verify() with the same session across different
 * tasks may lead to unexpected results.
 *
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pucData                       Data who's signature is to be verified.
 *                                          Note: In this implementation, this is generally
 *                                          expected to be the hash of the data.
 * @param[in] ulDataLen                     Length of pucData.
 * @param[in] pucSignature                  The signature to be verified.
 * @param[in] ulSignatureLength             Length of pucSignature in bytes.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Verify )( CK_SESSION_HANDLE xSession,
                                       CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataLen,
                                       CK_BYTE_PTR pucSignature,
                                       CK_ULONG ulSignatureLen )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_Verify \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_Verify(xSession,pucData,ulDataLen,pucSignature,ulSignatureLen);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}



/**
 * @brief Generate a new public-private key pair.
 *
 * This port only supports generating elliptic curve P-256
 * key pairs.
 *
 * @param[in] xSession                      Handle of a valid PKCS #11 session.
 * @param[in] pxMechanism                   Pointer to a mechanism. At this time,
 *                                          CKM_EC_KEY_PAIR_GEN is the only supported mechanism.
 * @param[in] pxPublicKeyTemplate           Pointer to a list of attributes that the generated
 *                                          public key should possess.
 * @param[in] ulPublicKeyAttributeCount     Number of attributes in pxPublicKeyTemplate.
 * @param[in] pxPrivateKeyTemplate          Pointer to a list of attributes that the generated
 *                                          private key should possess.
 * @param[in] ulPrivateKeyAttributeCount    Number of attributes in pxPrivateKeyTemplate.
 * @param[out] pxPublicKey                  Pointer to the handle of the public key to be created.
 * @param[out] pxPrivateKey                 Pointer to the handle of the private key to be created.
 *
 * \note Not all attributes specified by the PKCS #11 standard are supported.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateKeyPair )( CK_SESSION_HANDLE xSession,
                                                CK_MECHANISM_PTR pxMechanism,
                                                CK_ATTRIBUTE_PTR pxPublicKeyTemplate,
                                                CK_ULONG ulPublicKeyAttributeCount,
                                                CK_ATTRIBUTE_PTR pxPrivateKeyTemplate,
                                                CK_ULONG ulPrivateKeyAttributeCount,
                                                CK_OBJECT_HANDLE_PTR pxPublicKey,
                                                CK_OBJECT_HANDLE_PTR pxPrivateKey )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_GenerateKeyPair \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_GenerateKeyPair(xSession,pxMechanism,pxPublicKeyTemplate,ulPublicKeyAttributeCount,pxPrivateKeyTemplate,ulPrivateKeyAttributeCount,pxPublicKey,pxPrivateKey);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}

/**
 * @brief Generate cryptographically random bytes.
 *
 * @param xSession[in]          Handle of a valid PKCS #11 session.
 * @param pucRandomData[out]    Pointer to location that random data will be placed.
 *                              It is the responsiblity of the application to allocate
 *                              this memory.
 * @param ulRandomLength[in]    Length of data (in bytes) to be generated.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GenerateRandom )( CK_SESSION_HANDLE xSession,
                                               CK_BYTE_PTR pucRandomData,
                                               CK_ULONG ulRandomLen )
{
  CK_RV xResult = CKR_OK;

  PKCS11_DEBUG_PRINT( ( "[PKCS11] C_GenerateRandom \r\n" ) );

  /*Enter Secure Section*/
  prvEnterSecureSection();

  /*Secure Engine Call*/
  xResult = SE_KMS_GenerateRandom(xSession,pucRandomData,ulRandomLen);

  /*Exit Secure Section*/
  prvExitSecureSection();

  return xResult;
}
