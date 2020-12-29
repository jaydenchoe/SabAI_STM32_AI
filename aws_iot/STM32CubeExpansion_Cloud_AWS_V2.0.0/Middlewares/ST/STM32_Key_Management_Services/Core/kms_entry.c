/**
  ******************************************************************************
  * @file    kms_entry.c
  * @author  MCD Application Team
  * @brief   This file contains implementation for the entry point of Key Management
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

/* Includes ------------------------------------------------------------------*/

#include "kms_entry.h"

#include "kms_aes.h"                    /* KMS AES services */
#include "kms_digest.h"                 /* KMS digest services */
#include "kms_dyn_obj.h"                /* KMS dynamic objects services */
#include "kms_sign_verify.h"            /* KMS signature & verification services */
#include "kms_key_mgt.h"                /* KMS key management services */
#include "kms_objects.h"                /* KMS objects services */
#include "kms_low_level.h"
#ifdef KMS_EXT_TOKEN_ENABLED
#include "kms_ext_token.h"              /* KMS external token services */
#endif /* KMS_EXT_TOKEN_ENABLED */


/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_ENTRY Entry Point
  * @{
  */


/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/**
  * @brief Return error if session handle is not a valid one
  */
#define CHECK_SESSION_HANDLE(hsession)                                        \
    do                                                                        \
    {                                                                         \
      if (KMS_Check_Session_Handle(hsession) == CKR_SESSION_HANDLE_INVALID)   \
        return CKR_SESSION_HANDLE_INVALID;                                    \
    } while(0);
/**
  * @brief Return error if object handle is not a valid one
  */
#define CHECK_OBJECT_HANDLE(objHdle)                                          \
    do                                                                        \
    {                                                                         \
      if (KMS_Objects_GetRange(objHdle) == KMS_OBJECT_RANGE_UNKNOWN)          \
        return CKR_OBJECT_HANDLE_INVALID;                                     \
    } while(0);
/* Private variables ---------------------------------------------------------*/

/** @addtogroup KMS_ENTRY_Private_Variables Private Variables
  * @{
  */

#ifdef KMS_EXT_TOKEN_ENABLED      /* Rooter */

/**
  * @brief External token functions list
  */
static CK_FUNCTION_LIST ExtToken_FunctionList;
/**
  * @brief Pointer to @ref ExtToken_FunctionList
  */
static CK_FUNCTION_LIST_PTR pExtToken_FunctionList = &ExtToken_FunctionList;

#endif /* KMS_EXT_TOKEN_ENABLED */

/**
  * @}
  */

#ifdef KMS_UNITARY_TEST
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup KMS_ENTRY_Private_Functions Private Functions
  * @{
  */
CK_RV KMS_UnitaryTest(CK_ULONG ulTestID, CK_ULONG_PTR plNbPassed, CK_ULONG_PTR plNbFailed);
/* Private function ----------------------------------------------------------*/
extern uint32_t KMS_EXT_TOKEN_CalledFctId;
CK_RV KMS_UnitaryTest(CK_ULONG ulTestID, CK_ULONG_PTR plNbPassed, CK_ULONG_PTR plNbFailed)
{
  CK_RV e_ret_status = CKR_OK;

  /* ulTestID is not used today, but for futur use when we will have to run different internal tests. */
  if (ulTestID == 0UL)
  {
    /* Call the Test for GetAttribute() function. */
    e_ret_status = KMS_UnitaryTest_GetAttribute(plNbPassed, plNbFailed);

  }

#ifdef KMS_EXT_TOKEN_ENABLED
  /* Used to test the Ext_Token inn KMS */
  /* The Index of the executed function in the FunctionList is returned in plNbPassed */
  if (ulTestID == 1UL)
  {

    *plNbPassed = KMS_EXT_TOKEN_CalledFctId;

    KMS_EXT_TOKEN_CalledFctId = 0xFFFF;
  }
#endif /* KMS_EXT_TOKEN_ENABLED */

  return e_ret_status;
}

/**
  * @}
  */
#endif /* KMS_UNITARY_TEST */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_ENTRY_Exported_Functions Exported Functions
  * @{
  */



/**
  * @brief  Entry point function
  * @param  eID KMS function ID
  * @param  arguments function ID dependent arguments
  * @retval Operation status
  */
CK_RV KMS_Entry(KMS_FunctionIDTypeDef eID, va_list arguments)
{

  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
#ifdef KMS_EXT_TOKEN_ENABLED
  CK_RV e_ret_ext_status = CKR_FUNCTION_FAILED;
#endif /* KMS_EXT_TOKEN_ENABLED */

  /* Check that the Function ID match */
  if ((eID < KMS_FIRST_ID) && (eID >= KMS_LAST_ID_CHECK))
  {
    return CKR_FUNCTION_FAILED;
  }

  /**
    * @brief Provides import and storage of a single client certificate and
    * associated private key.
    */
  switch (eID)
  {
    case KMS_INITIALIZE_FCT_ID:
    {
      CK_VOID_PTR *pInitArgs;
      pInitArgs = va_arg(arguments, void *);
      /*    CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pInitArgs ) */


#ifdef KMS_EXT_TOKEN_ENABLED
      {
        CK_C_Initialize pC_Initialize;

        /* check if a Token is plugged in */

        /* Read the FunctionList supported by the Token */
        KMS_EXT_TOKEN_GetFunctionList(&pExtToken_FunctionList);

        /* If the Token support Initialize() function, call it */
        pC_Initialize = pExtToken_FunctionList->C_Initialize;

        if (pC_Initialize != NULL)
        {
          /* Call the C_Initialize function in the library */
          e_ret_ext_status = (*pC_Initialize)(pInitArgs);

          /* We call also the Initialize of the KMS */
          e_ret_status = KMS_Initialize(pInitArgs);

          /* Handle error on Ext_Token but not on KMS, to ensure error is */
          /* returned whether issue happens on KMS or Ext_Token           */
          if (e_ret_status == CKR_OK)
          {
            e_ret_status = e_ret_ext_status;
          }
        }
        else
        {
          /* We call also the Initialize of the KMS */
          e_ret_status = KMS_Initialize(pInitArgs);
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = KMS_Initialize(pInitArgs);
#endif /* KMS_EXT_TOKEN_ENABLED */

      break;
    }

    case KMS_FINALIZE_FCT_ID:
    {
      CK_VOID_PTR pReserved;
      pReserved = va_arg(arguments, CK_VOID_PTR);
      /*  CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pReserved ) */

#ifdef KMS_EXT_TOKEN_ENABLED
      {
        CK_C_Finalize pC_Finalize;

        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_Finalize != NULL))
        {
          /* If the Token support Finalize() function, call it */
          pC_Finalize = pExtToken_FunctionList -> C_Finalize;

          /* Call the C_Finalize function in the library */
          e_ret_ext_status = (*pC_Finalize)(pReserved);

          /* We call also the Finalize the KMS */
          e_ret_status = KMS_Finalize(pReserved);

          /* Handle error on Ext_Token but not on KMS, to ensure error is */
          /* returned whether issue happens on KMS or Ext_Token           */
          if (e_ret_status == CKR_OK)
          {
            e_ret_status = e_ret_ext_status;
          }
        }
        else
        {
          /* We call also the Finalize the KMS */
          e_ret_status = KMS_Finalize(pReserved);
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = KMS_Finalize(pReserved);
#endif /* KMS_EXT_TOKEN_ENABLED */


      break;
    }

    case KMS_GET_INFO_FCT_ID:
    {
      CK_INFO_PTR pInfo;

      /*    CK_DEFINEDECLARE_FUNCTION(CK_RV, C_GetInfo)(CK_INFO_PTR pInfo); */
      pInfo = va_arg(arguments, CK_INFO_PTR);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset(); */
      /* Is call by the function */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pInfo, 32);
#endif
          
      pInfo->cryptokiVersion.major = 2;
      pInfo->cryptokiVersion.minor = 40;
      (void)memcpy(&pInfo->manufacturerID[0], "ST Microelectronics             ", 32);
      pInfo->flags = 0;                         /* The spec say: MUST be Zero */
#ifdef KMS_EXT_TOKEN_ENABLED
      (void)memcpy(&pInfo->libraryDescription[0], "KMS-EXT-TOKEN                     ", 32);
#else /* KMS_EXT_TOKEN_ENABLED */
      (void)memcpy(&pInfo->libraryDescription[0], "KMS                               ", 32);
#endif /* KMS_EXT_TOKEN_ENABLED */

      pInfo->libraryVersion.minor = 0;
      pInfo->libraryVersion.major = 0;

      e_ret_status = CKR_OK;
            
      break;
    }

    case KMS_GET_SLOT_LIST_FCT_ID:
    {
      CK_SLOT_ID_PTR pSlotList;
      CK_ULONG_PTR pulCount;

      /* CK_DEFINEDECLARE_FUNCTION( CK_RV, C_GetSlotList)(
                                    CK_BBOOL tokenPresent,
                                    CK_SLOT_ID_PTR pSlotList,
                                    CK_ULONG_PTR pulCount);
        */
      /* tokenPresent parameter dropped has not used.                                                               */
      /* 'int' used instead of 'CK_BBOOL' cause of va_arg promoting parameter smaller than an integer to an integer */
      (void)va_arg(arguments, int);
      pSlotList = va_arg(arguments, CK_SLOT_ID_PTR);
      pulCount = va_arg(arguments, CK_ULONG_PTR);

      /* C_GetSlotList is used to obtain a list of slots in the system. tokenPresent
          indicates whether the list obtained includes only those slots with a token
          present (CK_TRUE), or all slots (CK_FALSE); pulCount points to the location
          that receives the number of slots.

        There are two ways for an application to call C_GetSlotList:
          1. If pSlotList is NULL_PTR, then all that C_GetSlotList does is return
            (in *pulCount) the number of slots, without actually returning a list of
            slots. The contents of the buffer pointed to by pulCount on entry to
            C_GetSlotList has no meaning in this case, and the call returns the value CKR_OK.
          2. If pSlotList is not NULL_PTR, then *pulCount MUST contain the size (in
            terms of CK_SLOT_ID elements) of the buffer pointed to by pSlotList.
            If that buffer is large enough to hold the list of slots, then the list
            is returned in it, and CKR_OK is returned. If not, then the call to
            C_GetSlotList returns the value CKR_BUFFER_TOO_SMALL. In either case,
            the value *pulCount is set to hold the number of slots.
        */

      if (pSlotList == NULL_PTR)
      {
        if (pulCount != NULL_PTR)
        {
          *pulCount = 1;
          e_ret_status = CKR_OK;
        }
        else
        {
          e_ret_status = CKR_HOST_MEMORY;
        }
      }
      else  /* pSlotList is not NULL */
      {
        if (*pulCount >= 1UL)
        {

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset(); */
      /* Is call by the function */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pSlotList, sizeof(CK_SLOT_ID_PTR));
#endif

          pSlotList[0] = 0;
          e_ret_status = CKR_OK;
          
          *pulCount = 1;
        }
        else
        {
          e_ret_status = CKR_BUFFER_TOO_SMALL;
        }
      }

      break;
    }

    case KMS_GET_SLOT_INFO_FCT_ID:
    {
      CK_SLOT_ID slotID;
      CK_SLOT_INFO_PTR pInfo;

      /*    CK_DEFINEDECLARE_FUNCTION(CK_RV, C_GetSlotInfo)(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo); */
      slotID = va_arg(arguments, CK_SLOT_ID);
      pInfo = va_arg(arguments, CK_SLOT_INFO_PTR);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pInfo, 32);
#endif

      
      if (slotID == 0UL)
      {
#ifdef KMS_EXT_TOKEN_ENABLED
        (void)memcpy(&pInfo->slotDescription[0],
                     "KMS FOR STM32 -WITH EXT-TOKEN                                   ",
                     64);
#else /* KMS_EXT_TOKEN_ENABLED */
        (void)memcpy(&pInfo->slotDescription[0],
                     "KMS FOR STM32                                                   ",
                     64);
#endif /* KMS_EXT_TOKEN_ENABLED */

        (void)memcpy(&pInfo->manufacturerID[0], "ST Microelectronics             ", 32);
        pInfo->flags = CKF_TOKEN_PRESENT;
        pInfo->hardwareVersion.minor = 0;
        pInfo->hardwareVersion.major = 0;
        pInfo->firmwareVersion.minor = 0;
        pInfo->firmwareVersion.major = 0;

        e_ret_status = CKR_OK;

      }
      else
      {
        e_ret_status = CKR_SLOT_ID_INVALID;
      }

      break;
    }

    case KMS_GET_TOKEN_INFO_FCT_ID:
    {
      CK_SLOT_ID        slotID;  /* ID of the token's slot */
      CK_TOKEN_INFO_PTR pInfo;    /* receives the token information */

      slotID      = va_arg(arguments, CK_SLOT_ID);
      pInfo       = va_arg(arguments, CK_TOKEN_INFO_PTR);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pInfo, 32);
#endif

      e_ret_status = KMS_GetTokenInfo(slotID, pInfo);

      break;
    }

    case KMS_GET_MECHANISM_INFO_FCT_ID:
    {

      CK_SLOT_ID             slotID;  /* ID of the token's slot */
      CK_MECHANISM_TYPE      type;    /* type of mechanism */
      CK_MECHANISM_INFO_PTR  pInfo;   /* receives the mechanism  information */

      slotID      = va_arg(arguments, CK_SLOT_ID);
      type        = va_arg(arguments, CK_MECHANISM_TYPE);
      pInfo       = va_arg(arguments, CK_MECHANISM_INFO_PTR);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */

      KMS_LL_IsBufferInSecureEnclave((void *)pInfo, 32);
#endif

#ifdef KMS_EXT_TOKEN_ENABLED
      {
        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_GetMechanismInfo != NULL))
        {
          e_ret_status = pExtToken_FunctionList->C_GetMechanismInfo(slotID, type, pInfo);
        }
      }
#endif /* KMS_EXT_TOKEN_ENABLED */

      if (e_ret_status != CKR_OK)
      {
        e_ret_status = KMS_GetMechanismInfo(slotID, type, pInfo);
      }

      break;
    }

    case KMS_OPEN_SESSION_FCT_ID:
    {
      CK_SLOT_ID slotID;
      CK_FLAGS flags;
      CK_VOID_PTR pApplication;
      CK_NOTIFY Notify;
      CK_SESSION_HANDLE_PTR phSession;
      /*    CK_DEFINE_FUNCTION( CK_RV, C_OpenSession )( CK_SLOT_ID slotID,
                                                        CK_FLAGS flags,
                                                        CK_VOID_PTR pApplication,
                                                        CK_NOTIFY Notify,
                                                        CK_SESSION_HANDLE_PTR phSession ) */

      slotID = va_arg(arguments, uint32_t);
      flags = va_arg(arguments, uint32_t);
      pApplication = va_arg(arguments, void *);
      Notify = va_arg(arguments, CK_NOTIFY);
      phSession = va_arg(arguments, CK_SESSION_HANDLE_PTR);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pApplication, sizeof(void *));
      KMS_LL_IsBufferInSecureEnclave((void *)phSession, sizeof(void *));
#endif
      
      
      /* Call the C_OpenSession function of the middleware */
      e_ret_status = KMS_OpenSession(slotID,  flags, pApplication, Notify, phSession);


#ifdef KMS_EXT_TOKEN_ENABLED
      {
        CK_C_OpenSession pC_OpenSession;
        CK_SESSION_HANDLE hSession_ExtToken;

        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_OpenSession != NULL))
        {
          /* If the Token support OpenSession() function, call it */
          pC_OpenSession = pExtToken_FunctionList->C_OpenSession;

          /* Call the C_OpenSession function of the External Token. */
          /* if a Callback for Notify is to be handled, it should pass through KMS. */
          /* So we have to register the Notify to call KMS notif. */
          /* Then this notif function would forward to the application */
          if (Notify != NULL)
          {
            e_ret_status = (*pC_OpenSession)(slotID,
                                             flags,
                                             pApplication,
                                             KMS_CallbackFunction_ForExtToken,
                                             &hSession_ExtToken);
            if (e_ret_status == CKR_OK)
            {
              KMS_OpenSession_RegisterExtToken(*phSession, hSession_ExtToken);
            }
          }
          else
          {
            e_ret_status = (*pC_OpenSession)(slotID,  flags, NULL, NULL, &hSession_ExtToken);
            if (e_ret_status == CKR_OK)
            {
              KMS_OpenSession_RegisterExtToken(*phSession, hSession_ExtToken);
            }
          }


        }
      }
#endif /* KMS_EXT_TOKEN_ENABLED */

      break;
    }

    case KMS_CLOSE_SESSION_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;
      /*    CK_DEFINE_FUNCTION( CK_RV, C_CloseSession )( CK_SESSION_HANDLE_PTR phSession ) */

      hSession = va_arg(arguments, CK_SESSION_HANDLE);
      CHECK_SESSION_HANDLE(hSession);
#ifdef KMS_EXT_TOKEN_ENABLED
      {
        CK_C_CloseSession pC_CloseSession;

        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_CloseSession != NULL))
        {
          /* If the Token support CloseSession() function, call it */
          pC_CloseSession = pExtToken_FunctionList -> C_CloseSession;

          /* Call the C_CloseSession function in the library */
          e_ret_ext_status = (*pC_CloseSession)(hSession);

          /* We call also the CloseSession the KMS */
          e_ret_status = KMS_CloseSession(hSession);

          /* Handle error on Ext_Token but not on KMS, to ensure error is */
          /* returned whether issue happens on KMS or Ext_Token           */
          if (e_ret_status == CKR_OK)
          {
            e_ret_status = e_ret_ext_status;
          }
        }
        else
        {
          /* We call also the CloseSession the KMS */
          e_ret_status = KMS_CloseSession(hSession);
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */

      e_ret_status = KMS_CloseSession(hSession);

#endif /* KMS_EXT_TOKEN_ENABLED */

      break;
    }

    case KMS_CREATE_OBJECT_FCT_ID:
    {
#ifdef KMS_EXT_TOKEN_ENABLED
      CK_SESSION_HANDLE hSession;
      CK_ATTRIBUTE_PTR pTemplate;
      CK_ULONG ulCount;
      CK_OBJECT_HANDLE_PTR phObject;
      /* CK_DEFINE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE hSession,
                                          CK_ATTRIBUTE_PTR pTemplate,
                                          CK_ULONG ulCount,
                                          CK_OBJECT_HANDLE_PTR phObject ) */
      hSession = va_arg(arguments, CK_SESSION_HANDLE);
      pTemplate = va_arg(arguments, CK_ATTRIBUTE_PTR);
      ulCount = va_arg(arguments, CK_ULONG);
      phObject = va_arg(arguments, CK_OBJECT_HANDLE_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pTemplate, sizeof(CK_ATTRIBUTE)*ulCount);
      KMS_LL_IsBufferInSecureEnclave((void *)phObject, sizeof(CK_OBJECT_HANDLE));
#endif
      
      {
        CK_C_CreateObject pC_CreateObject;

        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_CreateObject != NULL))
        {
          /* If the Token support CloseSession() function, call it */
          pC_CreateObject = pExtToken_FunctionList -> C_CreateObject;

          /* Call the C_CloseSession function in the library */
          e_ret_status = (*pC_CreateObject)(hSession, pTemplate, ulCount, phObject);
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = CKR_FUNCTION_FAILED;
#endif /* KMS_EXT_TOKEN_ENABLED */

      break;
    }

    case KMS_DESTROY_OBJECT_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;
      CK_OBJECT_HANDLE hObject;

      /* CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE hSession,
                                         CK_OBJECT_HANDLE hObject )         */
      hSession = va_arg(arguments, CK_SESSION_HANDLE);
      hObject = va_arg(arguments, CK_OBJECT_HANDLE);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hObject);

#ifdef KMS_EXT_TOKEN_ENABLED
      {
        kms_obj_range_t  ObjectRange = KMS_Objects_GetRange(hObject);

        /* The object is in EXT TOKEN Range */
        if ((ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID) ||
            (ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID))
        {
          CK_C_DestroyObject pC_DestroyObject;

          if ((pExtToken_FunctionList != NULL) &&
              (pExtToken_FunctionList->C_DestroyObject != NULL))
          {
            /* If the Token support SignInit() function, call it */
            pC_DestroyObject = pExtToken_FunctionList -> C_DestroyObject;

            /* Call the C_SignInit function in the library */
            e_ret_status = (*pC_DestroyObject)(hSession, hObject);
          }
        }
        else
        {
          e_ret_status = KMS_DestroyObject(hSession, hObject);
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = KMS_DestroyObject(hSession, hObject);
#endif /* KMS_EXT_TOKEN_ENABLED */

      break;
    }

    case KMS_FIND_OBJECTS_INIT_FCT_ID:
    {
#ifdef KMS_EXT_TOKEN_ENABLED
      {

        /* CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsInit )( CK_SESSION_HANDLE hSession,
                                            CK_ATTRIBUTE_PTR pTemplate,
                                            CK_ULONG ulCount ) */
        CK_SESSION_HANDLE hSession;
        CK_ATTRIBUTE_PTR pTemplate;
        CK_ULONG ulCount;

        CK_C_FindObjectsInit pC_FindObjectsInit;

        hSession        = va_arg(arguments, CK_SESSION_HANDLE);
        pTemplate      = va_arg(arguments, CK_ATTRIBUTE_PTR);
        ulCount         = va_arg(arguments, CK_ULONG);
        CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pTemplate, sizeof(CK_ATTRIBUTE)*ulCount);
#endif
        
        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_FindObjectsInit != NULL))
        {
          /* If the Token support FindObjectsInit() function, call it */
          pC_FindObjectsInit = pExtToken_FunctionList -> C_FindObjectsInit;

          /* Call the C_FindObjectsInit function in the library */
          e_ret_status = (*pC_FindObjectsInit)(hSession, pTemplate, ulCount);

        }
        else
        {
          /*  KMS_FindObjectsInit(); not yet supported */
          e_ret_status = CKR_FUNCTION_FAILED;
        }
      }
      break;

#else /* KMS_EXT_TOKEN_ENABLED */

      /*  KMS_FindObjectsInit(); not yet supported */
      e_ret_status = CKR_FUNCTION_FAILED;
      break;
#endif /* KMS_EXT_TOKEN_ENABLED */

    }

    case KMS_FIND_OBJECTS_FCT_ID:
    {

#ifdef KMS_EXT_TOKEN_ENABLED
      {

        /* CK_DEFINE_FUNCTION( CK_RV, C_FindObjects )( CK_SESSION_HANDLE hSession,
                                          CK_OBJECT_HANDLE_PTR phObject,
                                          CK_ULONG ulMaxObjectCount,
                                          CK_ULONG_PTR pulObjectCount ) */
        CK_SESSION_HANDLE hSession;
        CK_OBJECT_HANDLE_PTR phObject;
        CK_ULONG ulMaxObjectCount;
        CK_ULONG_PTR pulObjectCount;

        CK_C_FindObjects pC_FindObjects;

        hSession          = va_arg(arguments, CK_SESSION_HANDLE);
        phObject          = va_arg(arguments, CK_OBJECT_HANDLE_PTR);
        ulMaxObjectCount  = va_arg(arguments, CK_ULONG);
        pulObjectCount    = va_arg(arguments, CK_ULONG_PTR);
        CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)phObject, sizeof(CK_OBJECT_HANDLE)*ulMaxObjectCount);
      KMS_LL_IsBufferInSecureEnclave((void *)pulObjectCount, sizeof(void *));
#endif
        
        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_FindObjects != NULL))
        {
          /* If the Token support FindObjects() function, call it */
          pC_FindObjects = pExtToken_FunctionList -> C_FindObjects;

          /* Call the C_FindObjects function in the library */
          e_ret_status = (*pC_FindObjects)(hSession, phObject, ulMaxObjectCount, pulObjectCount);

        }
        else
        {
          /*  KMS_FindObjects(); not yet supported */
          e_ret_status = CKR_FUNCTION_FAILED;
        }
      }
      break;

#else /* KMS_EXT_TOKEN_ENABLED */

      /*  KMS_FindObjects(); not yet supported */
      e_ret_status = CKR_FUNCTION_FAILED;
      break;
#endif /* KMS_EXT_TOKEN_ENABLED */
    }


    case KMS_FIND_OBJECTS_FINAL_FCT_ID:
    {
#ifdef KMS_EXT_TOKEN_ENABLED
      {

        /* CK_DEFINE_FUNCTION( CK_RV, C_FindObjectsFinal )( CK_SESSION_HANDLE hSession ) */
        CK_SESSION_HANDLE hSession;

        CK_C_FindObjectsFinal pC_FindObjectsFinal;

        hSession          = va_arg(arguments, CK_SESSION_HANDLE);
        CHECK_SESSION_HANDLE(hSession);

        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_FindObjectsFinal != NULL))
        {
          /* If the Token support FindObjectsFinal() function, call it */
          pC_FindObjectsFinal = pExtToken_FunctionList -> C_FindObjectsFinal;

          /* Call the C_FindObjects function in the library */
          e_ret_status = (*pC_FindObjectsFinal)(hSession);

        }
        else
        {
          /*  KMS_FindObjectsFinal(); not yet supported */
          e_ret_status = CKR_FUNCTION_FAILED;
        }
      }
      break;

#else /* KMS_EXT_TOKEN_ENABLED */

      /*  KMS_FindObjectsFinal(); not yet supported */
      e_ret_status = CKR_FUNCTION_FAILED;
      break;
#endif /* KMS_EXT_TOKEN_ENABLED */
    }

    case KMS_ENCRYPT_INIT_FCT_ID:
    {
      /* C_EncryptInit initializes an encryption operation. */
      CK_SESSION_HANDLE hSession;    /* the session's handle */
      CK_MECHANISM_PTR  pMechanism;  /* the encryption mechanism */
      CK_OBJECT_HANDLE  hKey;        /* handle of encryption key */

      hSession        = va_arg(arguments, CK_SESSION_HANDLE);
      pMechanism      = va_arg(arguments, CK_MECHANISM_PTR);
      hKey            = va_arg(arguments, CK_OBJECT_HANDLE);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hKey);

      e_ret_status = KMS_EncryptInit(hSession, pMechanism, hKey);

      break;
    }

    case KMS_ENCRYPT_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;            /* session's handle */
      CK_BYTE_PTR       pData;               /* the plaintext data */
      CK_ULONG          ulDataLen;           /* bytes of plaintext */
      CK_BYTE_PTR       pEncryptedData;      /* gets ciphertext */
      CK_ULONG_PTR      pulEncryptedDataLen; /* gets c-text size */

      hSession              = va_arg(arguments, CK_SESSION_HANDLE);
      pData                 = va_arg(arguments, CK_BYTE_PTR);
      ulDataLen             = va_arg(arguments, CK_ULONG);
      pEncryptedData        = va_arg(arguments, CK_BYTE_PTR);
      pulEncryptedDataLen   = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pData, ulDataLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pEncryptedData, ulDataLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pulEncryptedDataLen, sizeof(void *));
#endif

      e_ret_status = KMS_Encrypt(hSession, pData, ulDataLen, pEncryptedData, pulEncryptedDataLen);

      break;
    }

    case KMS_ENCRYPT_UPDATE_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;           /* session's handle */
      CK_BYTE_PTR       pPart;              /* the plaintext data */
      CK_ULONG          ulPartLen;          /* plaintext data len */
      CK_BYTE_PTR       pEncryptedPart;     /* gets ciphertext */
      CK_ULONG_PTR      pulEncryptedPartLen;/* gets c-text size */

      hSession              = va_arg(arguments, CK_SESSION_HANDLE);
      pPart                 = va_arg(arguments, CK_BYTE_PTR);
      ulPartLen             = va_arg(arguments, CK_ULONG);
      pEncryptedPart        = va_arg(arguments, CK_BYTE_PTR);
      pulEncryptedPartLen   = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pPart, ulPartLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pEncryptedPart, ulPartLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pulEncryptedPartLen, sizeof(void *));
#endif

      e_ret_status = KMS_EncryptUpdate(hSession, pPart, ulPartLen, pEncryptedPart, pulEncryptedPartLen);

      break;
    }

    case KMS_ENCRYPT_FINAL_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;                /* session handle */
      CK_BYTE_PTR       pLastEncryptedPart;      /* last c-text */
      CK_ULONG_PTR      pulLastEncryptedPartLen; /* gets last size */

      hSession                      = va_arg(arguments, CK_SESSION_HANDLE);
      pLastEncryptedPart            = va_arg(arguments, CK_BYTE_PTR);
      pulLastEncryptedPartLen       = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
      if(pulLastEncryptedPartLen != NULL)
      {
        KMS_LL_IsBufferInSecureEnclave((void *)pLastEncryptedPart, *pulLastEncryptedPartLen);
        KMS_LL_IsBufferInSecureEnclave((void *)pulLastEncryptedPartLen, sizeof(void *));
      }
#endif

      e_ret_status = KMS_EncryptFinal(hSession, pLastEncryptedPart, pulLastEncryptedPartLen);

      break;
    }

    case KMS_DECRYPT_INIT_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;    /* the session's handle */
      CK_MECHANISM_PTR  pMechanism;  /* the decryption mechanism */
      CK_OBJECT_HANDLE  hKey;        /* handle of decryption key */

      hSession      = va_arg(arguments, CK_SESSION_HANDLE);
      pMechanism    = va_arg(arguments, CK_MECHANISM_PTR);
      hKey          = va_arg(arguments, CK_OBJECT_HANDLE);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hKey);

      e_ret_status = KMS_DecryptInit(hSession, pMechanism, hKey);

      break;
    }

    case KMS_DECRYPT_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;           /* session's handle */
      CK_BYTE_PTR       pEncryptedData;     /* ciphertext */
      CK_ULONG          ulEncryptedDataLen; /* ciphertext length */
      CK_BYTE_PTR       pData;              /* gets plaintext */
      CK_ULONG_PTR      pulDataLen;         /* gets p-text size */

      hSession              = va_arg(arguments, CK_SESSION_HANDLE);
      pEncryptedData        = va_arg(arguments, CK_BYTE_PTR);
      ulEncryptedDataLen    = va_arg(arguments, CK_ULONG);
      pData                 = va_arg(arguments, CK_BYTE_PTR);
      pulDataLen            = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pEncryptedData, ulEncryptedDataLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pData, ulEncryptedDataLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pulDataLen, sizeof(void *));
#endif

      e_ret_status = KMS_Decrypt(hSession, pEncryptedData, ulEncryptedDataLen,
                                 pData, pulDataLen);
      break;
    }

    case KMS_DECRYPT_UPDATE_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;            /* session's handle */
      CK_BYTE_PTR       pEncryptedPart;      /* encrypted data */
      CK_ULONG          ulEncryptedPartLen;  /* input length */
      CK_BYTE_PTR       pPart;               /* gets plaintext */
      CK_ULONG_PTR      pulPartLen;          /* p-text size */

      hSession              = va_arg(arguments, CK_SESSION_HANDLE);
      pEncryptedPart        = va_arg(arguments, CK_BYTE_PTR);
      ulEncryptedPartLen    = va_arg(arguments, CK_ULONG);
      pPart                 = va_arg(arguments, CK_BYTE_PTR);
      pulPartLen            = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pEncryptedPart, ulEncryptedPartLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pPart, ulEncryptedPartLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pulPartLen, sizeof(void *));
#endif

      e_ret_status = KMS_DecryptUpdate(hSession, pEncryptedPart, ulEncryptedPartLen, pPart, pulPartLen);

      break;
    }

    case KMS_DECRYPT_FINAL_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;       /* the session's handle */
      CK_BYTE_PTR       pLastPart;      /* gets plaintext */
      CK_ULONG_PTR      pulLastPartLen; /* p-text size */

      hSession        = va_arg(arguments, CK_SESSION_HANDLE);
      pLastPart       = va_arg(arguments, CK_BYTE_PTR);
      pulLastPartLen  = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

 #ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      if(pulLastPartLen != NULL)
        KMS_LL_IsBufferInSecureEnclave((void *)pLastPart, *pulLastPartLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pulLastPartLen, sizeof(void *));
#endif

      e_ret_status = KMS_DecryptFinal(hSession, pLastPart, pulLastPartLen);
      break;
    }


    case KMS_DIGEST_INIT_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;       /* the session's handle */
      CK_MECHANISM_PTR pMechanism;

      hSession        = va_arg(arguments, CK_SESSION_HANDLE);
      pMechanism       = va_arg(arguments, CK_MECHANISM_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_DIGEST
      e_ret_status = KMS_DigestInit(hSession, pMechanism);
#endif /* KMS_DIGEST */
      break;
    }

    case KMS_DIGEST_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;       /* the session's handle */
      CK_BYTE_PTR pData;
      CK_ULONG ulDataLen;
      CK_BYTE_PTR pDigest;
      CK_ULONG_PTR pulDigestLen;

      hSession      = va_arg(arguments, CK_SESSION_HANDLE);
      pData         = va_arg(arguments, CK_BYTE_PTR);
      ulDataLen     = va_arg(arguments, CK_ULONG);
      pDigest       = va_arg(arguments, CK_BYTE_PTR);
      pulDigestLen  = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      /* The HAsh for SHA256 is 32 byte, so we consider this max size */
      /* supported in current version*/
      KMS_LL_IsBufferInSecureEnclave((void *)pDigest, 32);
      KMS_LL_IsBufferInSecureEnclave((void *)pulDigestLen, sizeof(void *));
#endif
      
#ifdef KMS_DIGEST
      e_ret_status = KMS_Digest(hSession, pData, ulDataLen, pDigest, pulDigestLen);
#endif /* KMS_DIGEST */
      break;
    }

    case KMS_DIGEST_UPDATE_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;       /* the session's handle */
      CK_BYTE_PTR pPart;
      CK_ULONG ulPartLen;

      hSession      = va_arg(arguments, CK_SESSION_HANDLE);
      pPart         = va_arg(arguments, CK_BYTE_PTR);
      ulPartLen     = va_arg(arguments, CK_ULONG);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_DIGEST
      e_ret_status = KMS_DigestUpdate(hSession, pPart, ulPartLen);
#endif /* KMS_DIGEST */
      break;
    }

    case KMS_DIGEST_KEY_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;       /* the session's handle */
      CK_OBJECT_HANDLE hKey;

      hSession     = va_arg(arguments, CK_SESSION_HANDLE);
      hKey         = va_arg(arguments, CK_OBJECT_HANDLE);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hKey);

#ifdef KMS_DIGEST
      e_ret_status = KMS_DigestKey(hSession, hKey);
#endif /* KMS_DIGEST */
      break;
    }

    case KMS_DIGEST_FINAL_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;       /* the session's handle */
      CK_BYTE_PTR pDigest;
      CK_ULONG_PTR pulDigestLen;

      hSession      = va_arg(arguments, CK_SESSION_HANDLE);
      pDigest       = va_arg(arguments, CK_BYTE_PTR);
      pulDigestLen  = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
      
      /* The HAsh for SHA256 is 32 byte, so we consider this max size */
      /* supported in current version*/ 
      KMS_LL_IsBufferInSecureEnclave((void *)pDigest, 32);
      KMS_LL_IsBufferInSecureEnclave((void *)pulDigestLen, sizeof(void *));
#endif

#ifdef KMS_DIGEST
      e_ret_status = KMS_DigestFinal(hSession, pDigest, pulDigestLen);
#endif /* KMS_DIGEST */
      break;
    }


    case KMS_SIGN_INIT_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;    /* the session's handle */
      CK_MECHANISM_PTR  pMechanism;  /* the decryption mechanism */
      CK_OBJECT_HANDLE  hKey;        /* handle of key */

      hSession        = va_arg(arguments, CK_SESSION_HANDLE);
      pMechanism      = va_arg(arguments, CK_MECHANISM_PTR);
      hKey            = va_arg(arguments, CK_OBJECT_HANDLE);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hKey);

#ifdef KMS_RSA
#ifdef KMS_EXT_TOKEN_ENABLED
      {
        kms_obj_range_t  ObjectRange = KMS_Objects_GetRange(hKey);

        /* The object is in EXT TOKEN Range */
        if ((ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID) ||
            (ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID))
        {
          CK_C_SignInit pC_SignInit;

          if ((pExtToken_FunctionList != NULL) &&
              (pExtToken_FunctionList->C_SignInit != NULL))
          {
            /* If the Token support SignInit() function, call it */
            pC_SignInit = pExtToken_FunctionList -> C_SignInit;

            /* Call the C_SignInit function in the library */
            e_ret_status = (*pC_SignInit)(hSession, pMechanism, hKey);

            /* Store the key handle, to use it in the next calls */
            KMS_GETSESSSION(hSession).hKey = hKey;
          }
        }
        else
        {
          e_ret_status = KMS_SignInit(hSession, pMechanism, hKey);
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = KMS_SignInit(hSession, pMechanism, hKey);
#endif /* KMS_EXT_TOKEN_ENABLED */

#else /* KMS_RSA */
      e_ret_status = CKR_FUNCTION_FAILED;
#endif /* KMS_RSA */
      break;
    }

    case KMS_SIGN_FCT_ID:
    {
      CK_SESSION_HANDLE     hSession;    /* the session's handle */
      CK_BYTE_PTR           pData;
      CK_ULONG              ulDataLen;
      CK_BYTE_PTR           pSignature;
      CK_ULONG_PTR          pulSignatureLen;

      hSession      = va_arg(arguments, CK_SESSION_HANDLE);
      pData         = va_arg(arguments, CK_BYTE_PTR);
      ulDataLen       = va_arg(arguments, CK_ULONG);
      pSignature    = va_arg(arguments, CK_BYTE_PTR);
      pulSignatureLen = va_arg(arguments, CK_ULONG_PTR);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pData, ulDataLen);
      /* pSignature verif is done as soon as we would have the output size */
      /* KMS_LL_IsBufferInSecureEnclave((void *)pSignature, *pulSignatureLen);*/
      KMS_LL_IsBufferInSecureEnclave((void *)pulSignatureLen, sizeof(void *));
#endif

      
#ifdef KMS_RSA
#ifdef KMS_EXT_TOKEN_ENABLED
      {
        kms_obj_range_t  ObjectRange;

        if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
        {
          ObjectRange = KMS_Objects_GetRange(KMS_GETSESSSION(hSession).hKey);
        }
        else
        {
          ObjectRange = KMS_OBJECT_RANGE_UNKNOWN;
        }

        /* The object is in EXT TOKEN Range */
        if ((ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID) ||
            (ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID))
        {
          CK_C_Sign pC_Sign;

          if ((pExtToken_FunctionList != NULL) &&
              (pExtToken_FunctionList->C_Sign != NULL))
          {
            /* If the Token support Sign() function, call it */
            pC_Sign = pExtToken_FunctionList -> C_Sign;

            /* Call the C_Sign function in the library */
            e_ret_status = (*pC_Sign)(hSession, pData, ulDataLen, pSignature, pulSignatureLen);

            if (!((e_ret_status == CKR_BUFFER_TOO_SMALL) || ((e_ret_status == CKR_OK) && (pSignature == NULL))))
            {
              /* A call to C_Sign always terminates the active signing       */
              /* operation unless it returns CKR_BUFFER_TOO_SMALL or is a    */
              /* successful call (i.e., one which returns CKR_OK) to         */
              /* determine the length of the buffer needed to hold the       */
              /* signature.                                                  */
              KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;
            }

          }
        }
        else
        {
          if (ObjectRange != KMS_OBJECT_RANGE_UNKNOWN)
          {
            e_ret_status = KMS_Sign(hSession, pData, ulDataLen, pSignature, pulSignatureLen);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      /* pSignature verif is done as soon as we would have the output size */
      KMS_LL_IsBufferInSecureEnclave((void *)pSignature, *pulSignatureLen);
#endif
          }
          else
          {
            e_ret_status = CKR_FUNCTION_FAILED;
          }
        }
      }

#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = KMS_Sign(hSession, pData, ulDataLen, pSignature, pulSignatureLen);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      /* pSignature verif is done as soon as we would have the output size */
      KMS_LL_IsBufferInSecureEnclave((void *)pSignature, *pulSignatureLen);
#endif

#endif /* KMS_EXT_TOKEN_ENABLED */
#else /* KMS_RSA */
      e_ret_status = CKR_FUNCTION_FAILED;
#endif /* KMS_RSA */
      break;
    }

    case KMS_VERIFY_INIT_FCT_ID:
    {
      CK_SESSION_HANDLE hSession;    /* the session's handle */
      CK_MECHANISM_PTR  pMechanism;  /* the decryption mechanism */
      CK_OBJECT_HANDLE  hKey;        /* handle of key */

      hSession        = va_arg(arguments, CK_SESSION_HANDLE);
      pMechanism      = va_arg(arguments, CK_MECHANISM_PTR);
      hKey            = va_arg(arguments, CK_OBJECT_HANDLE);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hKey);

#if defined(KMS_RSA) || defined(KMS_ECDSA)
#ifdef KMS_EXT_TOKEN_ENABLED
      {
        kms_obj_range_t  ObjectRange = KMS_Objects_GetRange(hKey);

        /* The object is in EXT TOKEN Range */
        if ((ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID) ||
            (ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID))
        {
          CK_C_VerifyInit pC_VerifyInit;

          if ((pExtToken_FunctionList != NULL) &&
              (pExtToken_FunctionList->C_VerifyInit != NULL))
          {
            /* If the Token support VerifyInit() function, call it */
            pC_VerifyInit = pExtToken_FunctionList -> C_VerifyInit;

            /* Call the C_VerifyInit function in the library */
            e_ret_status = (*pC_VerifyInit)(hSession, pMechanism, hKey);

            /* Store the key handle, to use it in the next calls */
            KMS_GETSESSSION(hSession).hKey = hKey;
          }
        }
        else
        {
          e_ret_status = KMS_VerifyInit(hSession, pMechanism, hKey);
        }
      }

#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = KMS_VerifyInit(hSession, pMechanism, hKey);
#endif /* KMS_EXT_TOKEN_ENABLED */
#else /* KMS_RSA || KMS_ECDSA */
      e_ret_status = CKR_FUNCTION_FAILED;
#endif /* KMS_RSA || KMS_ECDSA */
      break;
    }

    case KMS_VERIFY_FCT_ID:
    {
      CK_SESSION_HANDLE     hSession;    /* the session's handle */
      CK_BYTE_PTR           pData;
      CK_ULONG              ulDataLen;
      CK_BYTE_PTR           pSignature;
      CK_ULONG              ulSignatureLen;

      hSession       = va_arg(arguments, CK_SESSION_HANDLE);
      pData          = va_arg(arguments, CK_BYTE_PTR);
      ulDataLen      = va_arg(arguments, CK_ULONG);
      pSignature     = va_arg(arguments, CK_BYTE_PTR);
      ulSignatureLen = va_arg(arguments, CK_ULONG);
      CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pData, ulDataLen);
      KMS_LL_IsBufferInSecureEnclave((void *)pSignature, ulSignatureLen);
#endif

#if defined(KMS_RSA) || defined(KMS_ECDSA)
#ifdef KMS_EXT_TOKEN_ENABLED
      {
        kms_obj_range_t  ObjectRange;

        if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
        {
          ObjectRange = KMS_Objects_GetRange(KMS_GETSESSSION(hSession).hKey);
        }
        else
        {
          ObjectRange = KMS_OBJECT_RANGE_UNKNOWN;
        }

        /* The object is in EXT TOKEN Range */
        if ((ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID) ||
            (ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID))
        {
          CK_C_Verify pC_Verify;

          if ((pExtToken_FunctionList != NULL) &&
              (pExtToken_FunctionList->C_Verify != NULL))
          {
            /* If the Token support Verify() function, call it */
            pC_Verify = pExtToken_FunctionList -> C_Verify;

            /* Call the C_Verify function in the library */
            e_ret_status = (*pC_Verify)(hSession, pData, ulDataLen, pSignature, ulSignatureLen);

            /* As soon as the Verify has been called once, the operation is considered finished */
            KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;
          }
        }
        else
        {
          if (ObjectRange != KMS_OBJECT_RANGE_UNKNOWN)
          {
            e_ret_status = KMS_Verify(hSession, pData, ulDataLen, pSignature, ulSignatureLen);
          }
          else
          {
            e_ret_status = CKR_FUNCTION_FAILED;
          }
        }
      }

#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = KMS_Verify(hSession, pData, ulDataLen, pSignature, ulSignatureLen);
#endif /* KMS_EXT_TOKEN_ENABLED */
#else /* KMS_RSA || KMS_ECDSA */
      e_ret_status = CKR_FUNCTION_FAILED;
#endif /*KMS_RSA || KMS_ECDSA */
      break;
    }

    case KMS_DERIVE_KEY_FCT_ID:
    {
#ifdef KMS_DERIVE_KEY
      CK_SESSION_HANDLE    hSession;          /* session's handle */
      CK_MECHANISM_PTR     pMechanism;        /* key deriv. mech. */
      CK_OBJECT_HANDLE     hBaseKey;          /* base key */
      CK_ATTRIBUTE_PTR     pTemplate;         /* new key template */
      CK_ULONG             ulAttributeCount;  /* template length */
      CK_OBJECT_HANDLE_PTR phKey;             /* gets new handle */

      hSession          = va_arg(arguments, CK_SESSION_HANDLE);
      pMechanism        = va_arg(arguments, CK_MECHANISM_PTR);
      hBaseKey          = va_arg(arguments, CK_OBJECT_HANDLE);
      pTemplate         = va_arg(arguments, CK_ATTRIBUTE_PTR);
      ulAttributeCount  = va_arg(arguments, CK_ULONG);
      phKey             = va_arg(arguments, CK_OBJECT_HANDLE_PTR);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hBaseKey);
      
      
#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pTemplate, sizeof(CK_ATTRIBUTE)*ulAttributeCount);
      KMS_LL_IsBufferInSecureEnclave((void *)phKey, sizeof(void *));
#endif
      
      
#endif /* KMS_DERIVE_KEY */
#ifdef KMS_EXT_TOKEN_ENABLED
      {
        kms_obj_range_t  ObjectRange = KMS_Objects_GetRange(hBaseKey);

        /* The object is in EXT TOKEN Range */
        if ((ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID) ||
            (ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID))
        {
          CK_C_DeriveKey pC_DeriveKey;

          if ((pExtToken_FunctionList != NULL) &&
              (pExtToken_FunctionList->C_DeriveKey != NULL))
          {
            /* If the Token support DeriveKey() function, call it */
            pC_DeriveKey = pExtToken_FunctionList -> C_DeriveKey;

            /* Call the C_DeriveKey function in the library */
            e_ret_status = (*pC_DeriveKey)(hSession, pMechanism, hBaseKey, pTemplate, ulAttributeCount, phKey);
          }
        }
        else
        {
          e_ret_status = KMS_DeriveKey(hSession, pMechanism, hBaseKey, pTemplate, ulAttributeCount, phKey);
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */
#ifdef KMS_DERIVE_KEY
      e_ret_status = KMS_DeriveKey(hSession, pMechanism, hBaseKey, pTemplate, ulAttributeCount, phKey);
#else /* KMS_DERIVE_KEY */
      e_ret_status = CKR_FUNCTION_FAILED;
#endif /* KMS_DERIVE_KEY */
#endif /* KMS_EXT_TOKEN_ENABLED */
      break;

    }

    case     KMS_WRAP_KEY_FCT_ID:  /* TBDevelopped later */
    {

      e_ret_status = CKR_FUNCTION_FAILED;

      break;
    }

    case KMS_GET_ATTRIBUTE_VALUE_FCT_ID:  /* TBDevelopped later */
    {
#ifdef KMS_GET_ATTRIBUTE_VALUE
      CK_SESSION_HANDLE hSession;   /* the session's handle */
      CK_OBJECT_HANDLE  hObject;    /* the object's handle */
      CK_ATTRIBUTE_PTR  pTemplate;  /* specifies attrs; gets vals */
      CK_ULONG          ulCount;    /* attributes in template */

      hSession       = va_arg(arguments, CK_SESSION_HANDLE);
      hObject        = va_arg(arguments, CK_OBJECT_HANDLE);
      pTemplate      = va_arg(arguments, CK_ATTRIBUTE_PTR);
      ulCount        = va_arg(arguments, CK_ULONG);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hObject);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pTemplate, sizeof(CK_ATTRIBUTE)*ulCount);
#endif
      
      
#ifdef KMS_EXT_TOKEN_ENABLED
      {
        kms_obj_range_t  ObjectRange = KMS_Objects_GetRange(hObject);

        /* The object is in EXT TOKEN Range */
        if ((ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID) ||
            (ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID))
        {
          CK_C_GetAttributeValue pC_GetAttributeValue;

          if ((pExtToken_FunctionList != NULL) &&
              (pExtToken_FunctionList->C_GetAttributeValue != NULL))
          {
            /* If the Token support GetAttribute() function, call it */
            pC_GetAttributeValue = pExtToken_FunctionList -> C_GetAttributeValue;

            /* Call the C_GetAttribute function of the External Token */
            e_ret_status = (*pC_GetAttributeValue)(hSession, hObject, pTemplate, ulCount);
          }
        }
        else
        {
          e_ret_status = KMS_GetAttributeValue(hSession, hObject, pTemplate, ulCount);
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = KMS_GetAttributeValue(hSession, hObject, pTemplate, ulCount);
#endif /* KMS_EXT_TOKEN_ENABLED */

#else /* KMS_GET_ATTRIBUTE_VALUE */
      e_ret_status = CKR_FUNCTION_FAILED;
#endif /* KMS_GET_ATTRIBUTE_VALUE */
      break;

    }

    case KMS_SET_ATTRIBUTE_VALUE_FCT_ID:
    {
#ifdef KMS_EXT_TOKEN_ENABLED

      CK_SESSION_HANDLE hSession;   /* the session's handle */
      CK_OBJECT_HANDLE  hObject;    /* the object's handle */
      CK_ATTRIBUTE_PTR  pTemplate;  /* specifies attrs and values */
      CK_ULONG          ulCount;     /* attributes in template */

      hSession       = va_arg(arguments, CK_SESSION_HANDLE);
      hObject        = va_arg(arguments, CK_OBJECT_HANDLE);
      pTemplate      = va_arg(arguments, CK_ATTRIBUTE_PTR);
      ulCount        = va_arg(arguments, CK_ULONG);
      CHECK_SESSION_HANDLE(hSession);
      CHECK_OBJECT_HANDLE(hObject);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pTemplate, sizeof(CK_ATTRIBUTE)*ulCount);
#endif
      {
        kms_obj_range_t  ObjectRange = KMS_Objects_GetRange(hObject);

        /* The object is in EXT TOKEN Range */
        if ((ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID) ||
            (ObjectRange == KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID))
        {
          CK_C_SetAttributeValue pC_SetAttributeValue;

          if ((pExtToken_FunctionList != NULL) &&
              (pExtToken_FunctionList->C_SetAttributeValue != NULL))
          {
            /* If the Token support SetAttributeValue() function, call it */
            pC_SetAttributeValue = pExtToken_FunctionList -> C_SetAttributeValue;

            /* Call the C_SetAttributeValue function of the External Token */
            e_ret_status = (*pC_SetAttributeValue)(hSession, hObject, pTemplate, ulCount);
          }
        }
        else
        {
          e_ret_status = CKR_ARGUMENTS_BAD;
        }
      }
#else /* KMS_EXT_TOKEN_ENABLED */
      e_ret_status = CKR_FUNCTION_FAILED;
#endif /* KMS_EXT_TOKEN_ENABLED */
      break;
    }

    case KMS_GENERATE_KEYPAIR_FCT_ID:
    {
#ifdef KMS_EXT_TOKEN_ENABLED
      {

        CK_SESSION_HANDLE    hSession;                    /* session handle */
        CK_MECHANISM_PTR     pMechanism;                  /* key-gen mech. */
        CK_ATTRIBUTE_PTR     pPublicKeyTemplate;          /* template for pub. key */
        CK_ULONG             ulPublicKeyAttributeCount;   /* # pub. attrs. */
        CK_ATTRIBUTE_PTR     pPrivateKeyTemplate;         /* template for priv. key */
        CK_ULONG             ulPrivateKeyAttributeCount;  /* # priv.  attrs. */
        CK_OBJECT_HANDLE_PTR phPublicKey;                 /* gets pub. key handle */
        CK_OBJECT_HANDLE_PTR phPrivateKey;                 /* gets priv. key handle */

        CK_C_GenerateKeyPair pC_GenerateKeyPair;

        hSession                   = va_arg(arguments, CK_SESSION_HANDLE);
        pMechanism                 = va_arg(arguments, CK_MECHANISM_PTR);
        pPublicKeyTemplate         = va_arg(arguments, CK_ATTRIBUTE_PTR);
        ulPublicKeyAttributeCount  = va_arg(arguments, CK_ULONG);
        pPrivateKeyTemplate        = va_arg(arguments, CK_ATTRIBUTE_PTR);
        ulPrivateKeyAttributeCount = va_arg(arguments, CK_ULONG);
        phPublicKey                = va_arg(arguments, CK_OBJECT_HANDLE_PTR);
        phPrivateKey               = va_arg(arguments, CK_OBJECT_HANDLE_PTR);
        CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pPublicKeyTemplate, sizeof(CK_ATTRIBUTE)*ulPublicKeyAttributeCount);
      KMS_LL_IsBufferInSecureEnclave((void *)pPrivateKeyTemplate, sizeof(CK_ATTRIBUTE)*ulPrivateKeyAttributeCount);
      KMS_LL_IsBufferInSecureEnclave((void *)phPublicKey, sizeof(void *));
      KMS_LL_IsBufferInSecureEnclave((void *)phPrivateKey, sizeof(void *));
#endif
        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_GenerateKeyPair != NULL))
        {
          /* If the Token support FindObjectsFinal() function, call it */
          pC_GenerateKeyPair = pExtToken_FunctionList -> C_GenerateKeyPair;

          /* Call the C_GenerateKeyPair function in the library */
          e_ret_status = (*pC_GenerateKeyPair)(hSession, pMechanism, pPublicKeyTemplate,
                                               ulPublicKeyAttributeCount, pPrivateKeyTemplate,
                                               ulPrivateKeyAttributeCount, phPublicKey, phPrivateKey);

        }
        else
        {
          /*  KMS_GenerateKeyPair(); not yet supported */
          e_ret_status = CKR_FUNCTION_FAILED;
        }
      }
      break;

#else /* KMS_EXT_TOKEN_ENABLED */

      /*  KMS_GenerateKeyPair(); not yet supported */
      e_ret_status = CKR_FUNCTION_FAILED;
      break;
#endif /* KMS_EXT_TOKEN_ENABLED */
    }

    case KMS_GENERATE_RANDOM_FCT_ID:
    {
#ifdef KMS_EXT_TOKEN_ENABLED
      {

        CK_SESSION_HANDLE hSession;    /* the session's handle */
        CK_BYTE_PTR       pRandomData;  /* receives the random data */
        CK_ULONG          ulRandomLen; /* # of bytes to generate */

        CK_C_GenerateRandom pC_GenerateRandom;

        hSession      = va_arg(arguments, CK_SESSION_HANDLE);
        pRandomData   = va_arg(arguments, CK_BYTE_PTR);
        ulRandomLen   = va_arg(arguments, CK_ULONG);
        CHECK_SESSION_HANDLE(hSession);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pRandomData, sizeof(CK_ULONG)*ulRandomLen);
#endif
        
        if ((pExtToken_FunctionList != NULL) &&
            (pExtToken_FunctionList->C_GenerateRandom != NULL))
        {
          /* If the Token support C_GenerateRandom() function, call it */
          pC_GenerateRandom = pExtToken_FunctionList -> C_GenerateRandom;

          /* Call the C_GenerateRandom function in the library */
          e_ret_status = (*pC_GenerateRandom)(hSession, pRandomData, ulRandomLen);

        }
        else
        {
          /*  KMS_GenerateRandom(); not yet supported */
          e_ret_status = CKR_FUNCTION_FAILED;
        }
      }
      break;

#else /* KMS_EXT_TOKEN_ENABLED */

      /*  KMS_GenerateKeyPair(); not yet supported */
      e_ret_status = CKR_FUNCTION_FAILED;
      break;
#endif /* KMS_EXT_TOKEN_ENABLED */
    }

    case KMS_IMPORT_BLOB_FCT_ID:
    {
      CK_BYTE_PTR           pData;
      CK_ULONG_PTR          pImportBlobState;

      /* This function is only accessible when in SECURE BOOT */
      /* Check that the Secure Engine services are not locked, LOCK shall be called only once */

      pData             = va_arg(arguments, CK_BYTE_PTR);
      pImportBlobState  = va_arg(arguments, CK_ULONG_PTR);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)pImportBlobState, sizeof(void *));
#endif

      /* This function is called while one Received Blob is not fully taken into account. */
      /* If the Update has not been finalized, then we try again till the end of the installation. */

      e_ret_status = KMS_Objects_ImportBlob(pData, pImportBlobState);

      break;
    }

#ifdef KMS_UNITARY_TEST
    case KMS_UNITARY_TEST_FCT_ID:
    {
      CK_ULONG          ulTestID;
      CK_ULONG_PTR      plNbPassed;
      CK_ULONG_PTR      plNbFailed;

      ulTestID        = va_arg(arguments, CK_ULONG);
      plNbPassed      = va_arg(arguments, CK_ULONG_PTR);
      plNbFailed      = va_arg(arguments, CK_ULONG_PTR);

#ifdef KMS_CHECK_PARAMS
      /* Check that pointer are outside Secure Engine                   */
      /* IF buffer point inside Firewall, then a NVIC_SystemReset();    */
      /* is call inside the function  ==> thats's why we do not manage  */
      /* the error case in this code                                    */
       
      KMS_LL_IsBufferInSecureEnclave((void *)plNbPassed, sizeof(void *));
      KMS_LL_IsBufferInSecureEnclave((void *)plNbFailed, sizeof(void *));
#endif
      e_ret_status = KMS_UnitaryTest(ulTestID, plNbPassed, plNbFailed);

      break;
    }
#endif /* KMS_UNITARY_TEST */

    default:
    {
      e_ret_status = CKR_FUNCTION_FAILED;
      break;
    }

  }

  /* Clean up */
  va_end(arguments);

  return e_ret_status;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
