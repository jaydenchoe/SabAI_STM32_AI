/**
  ******************************************************************************
  * @file    kms_digest.c
  * @author  MCD Application Team
  * @brief   This file contains implementation for Key Management Services (KMS)
  *          module digest functionalities.
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
#include "kms_digest.h"
#include "crypto.h"             /* Crypto services */
#include "kms_low_level.h"      /* CRC configuration for digest init */


/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

#ifdef KMS_DIGEST

/** @addtogroup KMS_DIGEST Digest services
  * @{
  */


/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup KMS_DIGEST_Private_Defines Private Defines
  * @{
  */
#define SHA1_LENGTH     (20)
#define SHA256_LENGTH   (32)
/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/** @addtogroup KMS_DIGEST_Private_Variables Private Variables
  * @{
  */

static HASHctx_stt hash_ctx = {0};     /*!< The HASH context (largest one) */

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_DIGEST_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This function is called upon @ref C_DigestInit call
  * @note   Refer to @ref C_DigestInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pMechanism digest mechanism
  * @retval Operation status
  */
CK_RV     KMS_DigestInit(CK_SESSION_HANDLE hSession,  CK_MECHANISM_PTR pMechanism)
{
  CK_RV e_ret_status = CKR_FUNCTION_FAILED;

  /* The supported Digest mechanisms types are:
  #define CKM_SHA1               0x00000220UL
  #define CKM_SHA256             0x00000250UL
  */

  /* Check that we support the expected mechanism. */
  if ((pMechanism->mechanism == CKM_SHA_1) ||
      (pMechanism->mechanism == CKM_SHA256))
  {
    if (KMS_LL_CRC_Init() == CKR_OK)
    {
      /* No processing already on going. */
      if (KMS_GETSESSSION(hSession).state != KMS_SESSION_IDLE)
      {
        e_ret_status = CKR_KEY_HANDLE_INVALID;
      }
      else
      {
        /* Verify that the HASH Ctxt is available */
        if (hash_ctx.mTagSize == 0)
        {
          KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;

          if (KMS_GETSESSSION(hSession).Mechanism == CKM_SHA_1)
          {
            hash_ctx.mFlags = E_HASH_DEFAULT;
            hash_ctx.mTagSize = CRL_SHA1_SIZE;

            if (SHA1_Init(&hash_ctx) == HASH_SUCCESS)
            {
              e_ret_status = CKR_OK ;
            }
          }

          if (KMS_GETSESSSION(hSession).Mechanism == CKM_SHA256)
          {
            hash_ctx.mFlags = E_HASH_DEFAULT;
            hash_ctx.mTagSize = CRL_SHA256_SIZE;

            if (SHA256_Init(&hash_ctx) == HASH_SUCCESS)
            {
              e_ret_status = CKR_OK ;
            }
          }
        }
        else
        {
          /* We have only one Ctxt available for Digest */
          /* In future version, it would be key to allow */
          /* several context */
          e_ret_status = CKR_DEVICE_MEMORY ;
        }
      }
    }
  }
  else
  {
    e_ret_status = CKR_MECHANISM_INVALID;
  }

  if (e_ret_status == CKR_OK)
  {
    KMS_GETSESSSION(hSession).state = KMS_SESSION_DIGEST;
  }
  return e_ret_status;
}

/**
  * @brief  This function is called upon @ref C_Digest call
  * @note   Refer to @ref C_Digest function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pData data to digest
  * @param  ulDataLen length of data to digest
  * @param  pDigest location to store digest message
  * @param  pulDigestLen length of the digest message
  * @retval Operation status
  */
CK_RV   KMS_Digest(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                   CK_ULONG ulDataLen, CK_BYTE_PTR pDigest, CK_ULONG_PTR pulDigestLen)
{
  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  int32_t cryptolib_status;    /* CryptoLib Error Status */

  /* Check if DigestInit has been called previously */
  if (KMS_GETSESSSION(hSession).state != KMS_SESSION_DIGEST)
  {
    e_ret_status = CKR_OPERATION_NOT_INITIALIZED;
  }
  else if (KMS_GETSESSSION(hSession).Mechanism == CKM_SHA_1)
  {
    /* Add data to be hashed */
    cryptolib_status = SHA1_Append(&hash_ctx, pData, (int32_t)ulDataLen);

    if (cryptolib_status == HASH_SUCCESS)
    {
      /* retrieve */
      cryptolib_status = SHA1_Finish(&hash_ctx, pDigest, (int32_t *)pulDigestLen);
      if (cryptolib_status == HASH_SUCCESS)
      {
        e_ret_status = CKR_OK;
      }
    }
  }
  else
  {
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_SHA256)
    {
      /* Add data to be hashed */
      cryptolib_status = SHA256_Append(&hash_ctx, pData, (int32_t)ulDataLen);

      if (cryptolib_status == HASH_SUCCESS)
      {
        /* retrieve */
        cryptolib_status = SHA256_Finish(&hash_ctx, pDigest, (int32_t *)pulDigestLen);
        if (cryptolib_status == HASH_SUCCESS)
        {
          e_ret_status = CKR_OK;
        }
      }
    }
    else
    {
      e_ret_status = CKR_MECHANISM_INVALID    ;
    }
  }

  /* We termintae the Digest */
  KMS_GETSESSSION(hSession).state = KMS_SESSION_IDLE;

  /* The Hash ctxt is now free */
  hash_ctx.mTagSize = 0;

  return e_ret_status;

}



/**
  * @brief  This function is called upon @ref C_DigestUpdate call
  * @note   Refer to @ref C_DigestUpdate function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pPart data part to digest
  * @param  ulPartLen length of data part to digest
  * @retval Operation status
  */
CK_RV   KMS_DigestUpdate(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{

  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  int32_t cryptolib_status;    /* CryptoLib Error Status */

  /* Check if DigestInit has been called previously */
  if (KMS_GETSESSSION(hSession).state != KMS_SESSION_DIGEST)
  {
    e_ret_status = CKR_OPERATION_NOT_INITIALIZED;
  }
  else if (KMS_GETSESSSION(hSession).Mechanism == CKM_SHA_1)
  {
    /* Add data to be hashed */
    cryptolib_status = SHA1_Append(&hash_ctx, pPart, (int32_t)ulPartLen);
    if (cryptolib_status == HASH_SUCCESS)
    {
      e_ret_status = CKR_OK;
    }
  }
  else
  {

    if (KMS_GETSESSSION(hSession).Mechanism == CKM_SHA256)
    {
      /* Add data to be hashed */
      cryptolib_status = SHA256_Append(&hash_ctx, pPart, (int32_t)ulPartLen);
      if (cryptolib_status == HASH_SUCCESS)
      {
        e_ret_status = CKR_OK;
      }
    }
    else
    {
      e_ret_status = CKR_MECHANISM_INVALID;
    }
  }

  return e_ret_status;

}


/**
  * @brief  This function is called upon @ref C_DigestKey call
  * @note   Refer to @ref C_DigestKey function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  hKey handle of the key to digest
  * @retval Operation status
  */
CK_RV   KMS_DigestKey(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hKey)
{
  (void)(hSession);
  (void)(hKey);
  /* Not supported yet ! */
  return CKR_FUNCTION_FAILED;

}

/**
  * @brief  This function is called upon @ref C_DigestFinal call
  * @note   Refer to @ref C_DigestFinal function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pDigest location to store digest message
  * @param  pulDigestLen length of the digest message
  * @retval Operation status
  */
CK_RV   KMS_DigestFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pDigest,
                        CK_ULONG_PTR pulDigestLen)
{

  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  int32_t cryptolib_status;    /* CryptoLib Error Status */

  /* Check if DigestInit has been called previously */
  if (KMS_GETSESSSION(hSession).state != KMS_SESSION_DIGEST)
  {
    e_ret_status = CKR_OPERATION_NOT_INITIALIZED;
  }
  else if (KMS_GETSESSSION(hSession).Mechanism == CKM_SHA_1)
  {
    KMS_CHECK_BUFFER_SECTION5_2(pDigest, pulDigestLen, SHA1_LENGTH);
    /* retrieve */
    cryptolib_status = SHA1_Finish(&hash_ctx, pDigest, (int32_t *)pulDigestLen);
    if (cryptolib_status == HASH_SUCCESS)
    {
      e_ret_status = CKR_OK;
    }
  }
  else
  {
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_SHA256)
    {
      KMS_CHECK_BUFFER_SECTION5_2(pDigest, pulDigestLen, SHA256_LENGTH);
      /* retrieve */
      cryptolib_status = SHA256_Finish(&hash_ctx, pDigest, (int32_t *)pulDigestLen);
      if (cryptolib_status == HASH_SUCCESS)
      {
        e_ret_status = CKR_OK;
      }
    }
    else
    {
      e_ret_status = CKR_MECHANISM_INVALID;
    }
  }

  /* We termintae the Digest */
  KMS_GETSESSSION(hSession).state = KMS_SESSION_IDLE;

  /* The Hash ctxt is now free */
  hash_ctx.mTagSize = 0;

  return e_ret_status;

}


/**
  * @}
  */


/**
  * @}
  */

#endif  /* KMS_DIGEST */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
