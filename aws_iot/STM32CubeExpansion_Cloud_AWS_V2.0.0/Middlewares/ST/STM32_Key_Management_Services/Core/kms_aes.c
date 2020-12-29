/**
  ******************************************************************************
  * @file    kms_aes.c
  * @author  MCD Application Team
  * @brief   This file contains implementation for Key Management Services (KMS)
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

/* Includes ------------------------------------------------------------------*/
#include "kms_aes.h"
#include "crypto.h"             /* Crypto services */
#include "kms_objects.h"        /* KMS object services */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_AES AES services
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @addtogroup KMS_AES_Private_Variables Private Variables
  * @{
  */

#ifdef KMS_AES_GCM
static AESGCMctx_stt aesgcm_ctx;        /*!< AES GCM Context */
#define GCM_TAG_SIZE 16                 /*!< AES GCM TAG size */
#endif /* KMS_AES_GCM */

#ifdef KMS_AES_CBC
static AESCBCctx_stt aescbc_ctx;        /*!< AES CBC Context */
#endif /* KMS_AES_CBC */

#ifdef KMS_AES_ECB
static AESECBctx_stt aesecb_ctx;        /*!< AES ECB Context */
#endif /* KMS_AES_ECB */

#ifdef KMS_AES_CCM
static AESCCMctx_stt aesccm_ctx;        /*!< AES CCM Context */
#endif /* KMS_AES_CCM */

#ifdef KMS_AES_CMAC
static AESCMACctx_stt aescmac_ctx;      /*!< AES CMAC Context */
#define CMAC_CRL_AES_BLOCK 16           /*!< AES CMAC Block size */
#endif /* KMS_AES_CMAC */

/**
  * @}
  */

/** @addtogroup KMS_AES_Private_Allocator Private Allocator
  * @note       As we don't have dynamic allocator in SE, we simulate dynamic allocation
                ==> all this code can be replaced later on with dynamic allocation.
  * @{
  */

#define KMS_SIZE_AES_KEY 32U            /*!< KMS AES key size (256/8) */
#define KMS_MAX_AES_PARRALEL_USAGE 5UL  /*!< KMS AES parallel usage */

/**
  * @brief Buffer table
  */
static uint8_t  *ptKeyValBuffer[KMS_MAX_AES_PARRALEL_USAGE + 1][KMS_SIZE_AES_KEY] =
{
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/**
  * @brief Buffer state table
  */
static uint8_t  ptKeyValBufferState[KMS_MAX_AES_PARRALEL_USAGE] =
{ 0, 0, 0, 0, 0 };

#define KMS_ALLOC_BUFF_NOT_INITIALIZED  0xFAU   /*!< Buffer NOT initialized */
#define KMS_ALLOC_BUFFER_DONE           0x01U   /*!< Buffer initialized */

/**
  * @brief Buffer initialization status
  */
static uint8_t  vFlagInitBufferDone = KMS_ALLOC_BUFF_NOT_INITIALIZED;

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @addtogroup KMS_AES_Private_Functions Private Functions
  * @{
  */
#define KMS_FLAG_ENCRYPT 0    /*!< Encryption requested */
#define KMS_FLAG_DECRYPT 1    /*!< Decryption requested */

static CK_RV          encrypt_decrypt_init(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                                           CK_OBJECT_HANDLE hKey, int32_t encdec_flag);

/**
  * @}
  */

/* Private function ----------------------------------------------------------*/


/** @addtogroup KMS_AES_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  This function is called upon @ref C_EncryptInit or @ref C_DecryptInit call
  * @note   Refer to @ref C_EncryptInit or @ref C_DecryptInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @note   The CKA_ENCRYPT attribute of the encryption key, which indicates
  *         whether the key supports encryption, MUST be CK_TRUE
  * @param  hSession session handle
  * @param  pMechanism encryption or decryption mechanism
  * @param  hKey handle of the encryption or decryption key
  * @param  encdec_flag action to perform: encrypt or decrypt
  * @retval Operation status
  */
static CK_RV          encrypt_decrypt_init(CK_SESSION_HANDLE hSession,
                                           CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey, int32_t encdec_flag)
{
  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  int32_t cryptolib_status = AES_ERR_BAD_OPERATION;    /* CryptoLib Error Status */
  kms_obj_keyhead_t *pkms_object;
  kms_ref_t *P_pKeyAttribute;
  void *pKeyBuffer;
#ifdef KMS_AES_GCM
  static uint8_t gcm_tag[GCM_TAG_SIZE];
#endif /* KMS_AES_GCM */
#ifdef KMS_AES_CMAC
  static uint8_t cmac_tag[CMAC_CRL_AES_BLOCK];
#endif /* KMS_AES_CMAC */

  /* The different AES mechanisms types: not yet covered
  #define CKM_AES_KEY_GEN                0x00001080UL
  #define CKM_AES_MAC                    0x00001083UL
  #define CKM_AES_MAC_GENERAL            0x00001084UL
  #define CKM_AES_CBC_PAD                0x00001085UL
  #define CKM_AES_CTR                    0x00001086UL
  #define CKM_AES_CTS                    0x00001089UL
  #define CKM_AES_CMAC_GENERAL           0x0000108BUL
  */

#ifdef KMS_AES_CBC
  /* Check that we support the expected mechanism. */
  if (pMechanism->mechanism == CKM_AES_CBC)
  {

    /* No processing already on going. */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_KEY_HANDLE_INVALID;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }

    /* Read the key value from the Key Handle                 */
    /* Key Handle is the index to one of static or nvm        */
    pkms_object = KMS_Objects_GetPointer(hKey);

    /* Check that hKey is valid */
    if (pkms_object != NULL)
    {

      /* If IV is defined */
      if ((pMechanism->pParameter != NULL) &&
          (pMechanism->ulParameterLen != 0U))
      {

        /* Search for the Key Value to use */
        e_ret_status = KMS_Objects_SearchAttributes(CKA_VALUE, pkms_object, &P_pKeyAttribute);

        if ((e_ret_status == CKR_OK) &&
            (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
            (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
        {
          /* Set flag field to default value */
          aescbc_ctx.mFlags = E_SK_DEFAULT;

          /* Set key size with value from attribute  */
          if ((P_pKeyAttribute->size == 16U) ||    /* 128 bits */
              (P_pKeyAttribute->size == 24U) ||     /* 192 bits */
              (P_pKeyAttribute->size == 32U))       /* 256 bits */
          {

            aescbc_ctx.mKeySize = (int32_t)P_pKeyAttribute->size ;

            /* Allocate a Key buffer */
            pKeyBuffer = KMS_AllocKey((uint32_t)aescbc_ctx.mKeySize);
            if (pKeyBuffer == NULL)
            {
              return CKR_DEVICE_MEMORY;
            }
            /* Store the allocated key associated to the session */
            KMS_GETSESSSION(hSession).pKeyAllocBuffer = pKeyBuffer;
            aescbc_ctx.pmKey = KMS_GETSESSSION(hSession).pKeyAllocBuffer;

            /* Read value from the structure. Need to be translated from
               (uint32_t*) to (uint8_t *) */
            if (KMS_Objects_TranslateKey(P_pKeyAttribute, KMS_GETSESSSION(hSession).pKeyAllocBuffer) != CKR_OK)
            {
              return  CKR_ATTRIBUTE_VALUE_INVALID;
            }

            /* Set iv size field with value passed through the MECHANISM */
            aescbc_ctx.mIvSize = (int32_t)pMechanism->ulParameterLen;

            /* Call the right init function */
            if (encdec_flag == KMS_FLAG_ENCRYPT)
            {
              /* load the key and ivec, eventually performs key schedule, etc. */
              cryptolib_status = AES_CBC_Encrypt_Init(&aescbc_ctx,
                                                      KMS_GETSESSSION(hSession).pKeyAllocBuffer,
                                                      pMechanism->pParameter);
            }
            else
            {
              /* load the key and ivec, eventually performs key schedule, etc. */
              cryptolib_status = AES_CBC_Decrypt_Init(&aescbc_ctx,
                                                      KMS_GETSESSSION(hSession).pKeyAllocBuffer,
                                                      pMechanism->pParameter);
            }
          }
          else
          {
            cryptolib_status = AES_ERR_BAD_PARAMETER;
          }


        }

      }

    }
    else
    {
      e_ret_status = CKR_SLOT_ID_INVALID;
    }


  }
#endif /* KMS_AES_CBC */

#ifdef KMS_AES_ECB
  /* Check that we support the expected mechanism. */
  if (pMechanism->mechanism == CKM_AES_ECB)
  {

    /* No processing already on going. */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_KEY_HANDLE_INVALID;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }

    /* Read the key value from the Key Handle                 */
    /* Key Handle is the index to one of static or nvm        */
    pkms_object = KMS_Objects_GetPointer(hKey);

    /* Check that hKey is valid */
    if (pkms_object != NULL)
    {

      /* Search for the Key Value to use */
      e_ret_status = KMS_Objects_SearchAttributes(CKA_VALUE, pkms_object, &P_pKeyAttribute);

      if ((e_ret_status == CKR_OK) &&
          (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
          (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
      {
        /* Set flag field to default value */
        aesecb_ctx.mFlags = E_SK_DEFAULT;

        /* Set key size with value from attribute  */
        if ((P_pKeyAttribute->size == 16U) ||    /* 128 bits */
            (P_pKeyAttribute->size == 24U) ||     /* 192 bits */
            (P_pKeyAttribute->size == 32U))       /* 256 bits */
        {

          aesecb_ctx.mKeySize = (int32_t)P_pKeyAttribute->size ;

          /* Allocate a Key buffer */
          pKeyBuffer = KMS_AllocKey((uint32_t)P_pKeyAttribute->size);
          if (pKeyBuffer == NULL)
          {
            return CKR_DEVICE_MEMORY;
          }
          /* Store the allocated key associated to the session */
          KMS_GETSESSSION(hSession).pKeyAllocBuffer = pKeyBuffer;
          aesecb_ctx.pmKey = KMS_GETSESSSION(hSession).pKeyAllocBuffer;

          /* Read value from the structure. Need to be translated from
             (uint32_t*) to (uint8_t *) */
          if (KMS_Objects_TranslateKey(P_pKeyAttribute, KMS_GETSESSSION(hSession).pKeyAllocBuffer) != CKR_OK)
          {
            return  CKR_ATTRIBUTE_VALUE_INVALID;
          }

          /* Call the right init function */
          if (encdec_flag == KMS_FLAG_ENCRYPT)
          {
            /* load the key and ivec, eventually performs key schedule, etc. */
            cryptolib_status = AES_ECB_Encrypt_Init(&aesecb_ctx, KMS_GETSESSSION(hSession).pKeyAllocBuffer, NULL);
          }
          else
          {
            /* load the key and ivec, eventually performs key schedule, etc. */
            cryptolib_status = AES_ECB_Decrypt_Init(&aesecb_ctx, KMS_GETSESSSION(hSession).pKeyAllocBuffer, NULL);
          }
        }
        else
        {
          cryptolib_status = AES_ERR_BAD_PARAMETER;
        }

      }
    }
    else
    {
      e_ret_status = CKR_SLOT_ID_INVALID;
    }


  }
#endif /* KMS_AES_ECB */

#ifdef KMS_AES_CCM
  /* Check that we support the expected mechanism. */
  if (pMechanism->mechanism == CKM_AES_CCM)
  {
    CK_CCM_PARAMS *pCCMParams;

    pCCMParams = (CK_CCM_PARAMS *)pMechanism->pParameter;

    /* No processing already on going. */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_KEY_HANDLE_INVALID;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }

    /* Read the key value from the Key Handle                 */
    /* Key Handle is the index to one of static or nvm        */
    pkms_object = KMS_Objects_GetPointer(hKey);

    /* Check that hKey is valid */
    if (pkms_object != NULL)
    {

      /* If IV is defined */
      if ((pMechanism->pParameter != NULL) &&
          (pMechanism->ulParameterLen != 0UL))
      {

        /* Search for the Key Value to use */
        e_ret_status = KMS_Objects_SearchAttributes(CKA_VALUE, pkms_object, &P_pKeyAttribute);

        if ((e_ret_status == CKR_OK) &&
            (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
            (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
        {
          /* Set flag field to default value */
          aesccm_ctx.mFlags = E_SK_DEFAULT;

          /* Set key size with value from attribute  */
          if ((P_pKeyAttribute->size == 16U) ||    /* 128 bits */
              (P_pKeyAttribute->size == 24U) ||     /* 192 bits */
              (P_pKeyAttribute->size == 32U))       /* 256 bits */
            /* Set key size to using Attribute Size (corresponding to AES-128) */
          {
            aesccm_ctx.mKeySize = (int32_t)P_pKeyAttribute->size;

            /* Allocate a Key buffer */
            pKeyBuffer = KMS_AllocKey((uint32_t)aesccm_ctx.mKeySize);

            if (pKeyBuffer == NULL)
            {
              return CKR_DEVICE_MEMORY;
            }
            /* Store the allocated key associated to the session */
            KMS_GETSESSSION(hSession).pKeyAllocBuffer = pKeyBuffer;

            aesccm_ctx.pmKey = KMS_GETSESSSION(hSession).pKeyAllocBuffer;

            /* Read value from the structure. Need to be translated from
              (uint32_t*) to (uint8_t *) */
            if (KMS_Objects_TranslateKey(P_pKeyAttribute, KMS_GETSESSSION(hSession).pKeyAllocBuffer) != CKR_OK)
            {
              return  CKR_ATTRIBUTE_VALUE_INVALID;
            }

            /* Set nonce size field to IvLength, note that valid values are 7,8,9,10,11,12,13*/
            aesccm_ctx.mNonceSize = (int32_t)pCCMParams->ulNonceLen;

            /* Point to the pNonce used as IV */
            aesccm_ctx.pmNonce = pCCMParams->pNonce;

            /* Size of returned authentication TAG */
            aesccm_ctx.mTagSize = (int32_t)pCCMParams->ulMACLen;

            /* Set the size of the header */
            aesccm_ctx.mAssDataSize = (int32_t)pCCMParams->ulAADLen;

            /* Set the size of thepayload */
            aesccm_ctx.mPayloadSize = (int32_t)pCCMParams->ulDataLen;

            /* Call the right init function */
            if (encdec_flag == KMS_FLAG_ENCRYPT)
            {
              /* Initialize the operation, by passing the key and IV */
              cryptolib_status = AES_CCM_Encrypt_Init(&aesccm_ctx,
                                                      KMS_GETSESSSION(hSession).pKeyAllocBuffer,
                                                      pCCMParams->pNonce);

            }
            else
            {
              /* Initialize the operation, by passing the key and IV */
              cryptolib_status = AES_CCM_Decrypt_Init(&aesccm_ctx,
                                                      KMS_GETSESSSION(hSession).pKeyAllocBuffer,
                                                      pCCMParams->pNonce);
            }

            if ((cryptolib_status == AES_SUCCESS) && (pCCMParams->ulAADLen != 0UL))
            {
              /* If a header (or Additionnal Data is proposed ==> AAD from MECHANISM) */
              cryptolib_status = AES_CCM_Header_Append(&aesccm_ctx, pCCMParams->pAAD, (int32_t)pCCMParams->ulAADLen);
            }

          }
          else
          {
            cryptolib_status = AES_ERR_BAD_INPUT_SIZE;
          }

        }
      }

    }
    else
    {
      e_ret_status = CKR_SLOT_ID_INVALID;
    }

  }
#endif /* KMS_AES_CCM   */

#ifdef KMS_AES_GCM
  /* Check that we support the expected mechanism. */
  if (pMechanism->mechanism == CKM_AES_GCM)
  {
    CK_GCM_PARAMS *pGCMParams;

    pGCMParams = (CK_GCM_PARAMS *)pMechanism->pParameter;

    /* No processing already on going. */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_KEY_HANDLE_INVALID;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }

    /* Read the key value from the Key Handle                 */
    /* Key Handle is the index to one of static or nvm        */
    pkms_object = KMS_Objects_GetPointer(hKey);

    /* Check that hKey is valid */
    if (pkms_object != NULL)
    {

      /* If IV is defined */
      if ((pMechanism->pParameter != NULL) &&
          (pMechanism->ulParameterLen != 0UL))
      {

        /* Search for the Key Value to use */
        e_ret_status = KMS_Objects_SearchAttributes(CKA_VALUE, pkms_object, &P_pKeyAttribute);

        if ((e_ret_status == CKR_OK) &&
            (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
            (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
        {
          /* Set flag field to default value */
          aesgcm_ctx.mFlags = E_SK_DEFAULT;

          /* Set key size with value from attribute  */
          if ((P_pKeyAttribute->size == 16U) ||    /* 128 bits */
              (P_pKeyAttribute->size == 24U) ||     /* 192 bits */
              (P_pKeyAttribute->size == 32U))       /* 256 bits */
          {

            /* Set key size  */
            aesgcm_ctx.mKeySize = (int32_t)P_pKeyAttribute->size ;

            /* Allocate a Key buffer */
            pKeyBuffer = KMS_AllocKey((uint32_t)aesgcm_ctx.mKeySize);

            if (pKeyBuffer == NULL)
            {
              return CKR_DEVICE_MEMORY;
            }
            /* Store the allocated key associated to the session */
            KMS_GETSESSSION(hSession).pKeyAllocBuffer = pKeyBuffer;

            aesgcm_ctx.pmKey = KMS_GETSESSSION(hSession).pKeyAllocBuffer;

            /* Read value from the structure. Need to be translated from
              (uint32_t*) to (uint8_t *) */
            if (KMS_Objects_TranslateKey(P_pKeyAttribute, KMS_GETSESSSION(hSession).pKeyAllocBuffer) != CKR_OK)
            {
              return  CKR_ATTRIBUTE_VALUE_INVALID;
            }

            /* Set nonce size field to iv_length, note that valid values are 7,8,9,10,11,12,13*/
            aesgcm_ctx.mIvSize = (int32_t)pGCMParams->ulIvLen;

            /* Size of returned authentication TAG */
            aesgcm_ctx.mTagSize = ((int32_t)(pGCMParams->ulTagBits) / 8L);

            /* */
            aesgcm_ctx.pmTag = gcm_tag;


            /* Set the size of thepayload */
            aesgcm_ctx.mPayloadSize = 0;


            /* Call the right init function */
            if (encdec_flag == KMS_FLAG_ENCRYPT)
            {
              /* Crypto function call*/
              cryptolib_status = AES_GCM_Encrypt_Init(&aesgcm_ctx,
                                                      KMS_GETSESSSION(hSession).pKeyAllocBuffer,
                                                      pGCMParams->pIv);
            }
            else
            {
              /* Crypto function call*/
              cryptolib_status = AES_GCM_Decrypt_Init(&aesgcm_ctx,
                                                      KMS_GETSESSSION(hSession).pKeyAllocBuffer,
                                                      pGCMParams->pIv);
            }

            if ((cryptolib_status == AES_SUCCESS) && (pGCMParams->ulAADLen != 0UL))
            {
              /* If a header (or Additionnal Data is proposed ==> AAD from MECHANISM) */
              cryptolib_status = AES_GCM_Header_Append(&aesgcm_ctx, pGCMParams->pAAD, (int32_t)pGCMParams->ulAADLen);
            }
          }
          else
          {
            cryptolib_status = AES_ERR_BAD_INPUT_SIZE;
          }
        }
      }
    }
    else
    {
      e_ret_status = CKR_SLOT_ID_INVALID;
    }

  }
#endif /* KMS_AES_GCM   */

#ifdef KMS_AES_CMAC
  /* Check that we support the expected mechanism. */
  if (pMechanism->mechanism == CKM_AES_CMAC)
  {

    /* No processing already on going. */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_KEY_HANDLE_INVALID;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }

    /* Read the key value from the Key Handle                 */
    /* Key Handle is the index to one of static or nvm        */
    pkms_object = KMS_Objects_GetPointer(hKey);

    /* Check that hKey is valid */
    if (pkms_object != NULL)
    {

      /* Search for the Key Value to use */
      e_ret_status = KMS_Objects_SearchAttributes(CKA_VALUE, pkms_object, &P_pKeyAttribute);

      if ((e_ret_status == CKR_OK) &&
          (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
          (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
      {
        /* Set flag field to default value */
        aescmac_ctx.mFlags = E_SK_DEFAULT;

        /* Set key size with value from attribute  */
        if ((P_pKeyAttribute->size == 16U) ||    /* 128 bits */
            (P_pKeyAttribute->size == 24U) ||     /* 192 bits */
            (P_pKeyAttribute->size == 32U))       /* 256 bits */
        {

          /* Set flag field to default value */
          aescmac_ctx.mFlags = E_SK_FINAL_APPEND;

          /* Set the key */
          /* Set key size  */
          aescmac_ctx.mKeySize = (int32_t)P_pKeyAttribute->size ;

          /* Allocate a Key buffer */
          pKeyBuffer = KMS_AllocKey((uint32_t)aescmac_ctx.mKeySize);
          if (pKeyBuffer == NULL)
          {
            return CKR_DEVICE_MEMORY;
          }
          /* Store the allocated key associated to the session */
          KMS_GETSESSSION(hSession).pKeyAllocBuffer = pKeyBuffer;

          aescmac_ctx.pmKey = KMS_GETSESSSION(hSession).pKeyAllocBuffer;

          /* Read value from the structure. Need to be translated from
            (uint32_t*) to (uint8_t *) */
          if (KMS_Objects_TranslateKey(P_pKeyAttribute, KMS_GETSESSSION(hSession).pKeyAllocBuffer) != CKR_OK)
          {
            return  CKR_ATTRIBUTE_VALUE_INVALID;
          }

          aescmac_ctx.mTagSize = CMAC_CRL_AES_BLOCK;

          /* */
          aesgcm_ctx.pmTag = cmac_tag;

          /* Call the right init function */
          if (encdec_flag == KMS_FLAG_ENCRYPT)
          {
            /* Initialize the operation, by passing the context */
            cryptolib_status = AES_CMAC_Encrypt_Init(&aescmac_ctx);
          }
          else
          {
            /* Initialize the operation, by passing the context */
            cryptolib_status = AES_CMAC_Decrypt_Init(&aescmac_ctx);
          }


        }
        else
        {
          cryptolib_status = AES_ERR_BAD_INPUT_SIZE;
        }
      }
    }
    else
    {
      e_ret_status = CKR_SLOT_ID_INVALID;
    }

  }
#endif /* KMS_AES_CMAC   */

  if (cryptolib_status == AES_SUCCESS)
  {
    e_ret_status = CKR_OK;
  }

  return e_ret_status;
}


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup KMS_AES_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This function initialize the structures used to manage key buffer allocation
  *         It is called from KMS_Initialize()
  */
void KMS_InitBufferAlloc(void)
{
  int32_t i;

  for (i = 0; i < KMS_MAX_AES_PARRALEL_USAGE ; i++)
  {
    ptKeyValBuffer[i][0] = 0;
    ptKeyValBufferState[i] = 0;
  }
  vFlagInitBufferDone = KMS_ALLOC_BUFFER_DONE;
}

/**
  * @brief  This function clean up the structures used to manage key buffer allocation
  *         It is called from KMS_Finalize()
  */
void KMS_FinalizeBufferAlloc(void)
{
  int32_t i;
  for (i = 0; i < KMS_MAX_AES_PARRALEL_USAGE ; i++)
  {

    ptKeyValBufferState[i] = 0;
    uint32_t j;
    /* We clean-up the buffer containing the keys (for security reasons) */
    for (j = 0U; j < KMS_SIZE_AES_KEY; j++)
    {
      ptKeyValBuffer[i][j] = 0U;
    }

  }
}


/**
  * @brief  This function allows to allocate a buffer for key
  * @note   This is needed as the keys are translated from the storage
  * @note   The prototype is compatible with standard malloc() function
  * @param  size Size of the buffer to be allocated
  * @retval Pointer to the allocated buffer (NULL in case of failure)
  */
void    *KMS_AllocKey(uint32_t size)
{
  int32_t i;
  (void)(size); /* PArameter for future use */

  /* The buffer alloc has not be initialized */
  if (vFlagInitBufferDone != KMS_ALLOC_BUFFER_DONE)
  {
    return NULL;
  }

  for (i = 0; i < KMS_MAX_AES_PARRALEL_USAGE; i++)
  {
    if (ptKeyValBufferState[i] == 0U)
    {
      ptKeyValBufferState[i] = 1U;
      return ((void *)ptKeyValBuffer[i]) ;
    }
  }
  return NULL;
}

/**
  * @brief  This function allows to free a buffer used for key
  * @note   This is needed as the keys are translated from the storage
  * @note   The prototype is compatible with standard free() function
  * @param  pBufferToRelease Buffer to release
  */
void    KMS_FreeKey(const void *pBufferToRelease)
{

  int32_t i;

  for (i = 0; i < KMS_MAX_AES_PARRALEL_USAGE; i++)
  {
    if (ptKeyValBuffer[i] == pBufferToRelease)
    {
      /* We freeze the buffer usage */
      ptKeyValBufferState[i] = 0;
      uint32_t j;
      /* We clean-up the buffer containing the keys (for security reasons) */
      for (j = 0; j < KMS_SIZE_AES_KEY; j++)
      {
        ptKeyValBuffer[i][j] = 0;
      }
      break;
    }
  }
}

/**
  * @brief  This function is called upon @ref C_EncryptInit call
  * @note   Refer to @ref C_EncryptInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @note   The CKA_ENCRYPT attribute of the encryption key, which indicates
  *         whether the key supports encryption, MUST be CK_TRUE
  * @param  hSession session handle
  * @param  pMechanism encryption mechanism
  * @param  hKey handle of the encryption key
  * @retval Operation status
  */
CK_RV          KMS_EncryptInit(CK_SESSION_HANDLE hSession,
                               CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
  CK_RV e_ret_status;

  /* We reuse similar code between Encrypt & Decrypt Init */
  e_ret_status = encrypt_decrypt_init(hSession, pMechanism, hKey, KMS_FLAG_ENCRYPT);

  return (e_ret_status) ;

}



/**
  * @brief  This function is called upon @ref C_Encrypt call
  * @note   Refer to @ref C_Encrypt function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pData data to encrypt
  * @param  ulDataLen length of data to encrypt
  * @param  pEncryptedData encrypted data
  * @param  pulEncryptedDataLen length of encrypted data
  * @retval Operation status
  */
CK_RV          KMS_Encrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData,
                           CK_ULONG ulDataLen, CK_BYTE_PTR pEncryptedData,
                           CK_ULONG_PTR pulEncryptedDataLen)
{

  CK_RV e_ret_status ;

  e_ret_status = KMS_EncryptUpdate(hSession, pData, ulDataLen, pEncryptedData,
                                   pulEncryptedDataLen);

  KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;
  if (KMS_GETSESSSION(hSession).pKeyAllocBuffer != NULL)
  {
    KMS_FreeKey(KMS_GETSESSSION(hSession).pKeyAllocBuffer);
    KMS_GETSESSSION(hSession).pKeyAllocBuffer = NULL;
  }

  return (e_ret_status) ;

}

/**
  * @brief  This function is called upon @ref C_EncryptUpdate call
  * @note   Refer to @ref C_EncryptUpdate function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pPart data part to encrypt
  * @param  ulPartLen length of data part to encrypt
  * @param  pEncryptedPart encrypted data part
  * @param  pulEncryptedPartLen length of encrypted data part
  * @retval Operation status
  */
CK_RV          KMS_EncryptUpdate(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR pPart, CK_ULONG ulPartLen,
                                 CK_BYTE_PTR pEncryptedPart,
                                 CK_ULONG_PTR pulEncryptedPartLen)
{
  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  int32_t cryptolib_status = AES_ERR_BAD_OPERATION;    /* CryptoLib Error Status */
  int32_t lEncryptPartLen = 0;

  /* Check that a Processing is already on going. */
  if (KMS_GETSESSSION(hSession).hKey == KMS_HANDLE_KEY_NOT_KNOWN)
  {
    return CKR_KEY_HANDLE_INVALID;
  }
  else
  {

#ifdef KMS_AES_CBC
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CBC)
    {

      /* Encrypt Data */
      cryptolib_status = AES_CBC_Encrypt_Append(&aescbc_ctx, pPart, (int32_t)ulPartLen,
                                                pEncryptedPart,
                                                &lEncryptPartLen);

    }
#endif /* KMS_AES_CBC */

#ifdef KMS_AES_ECB
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_ECB)
    {

      /* Encrypt Data */
      cryptolib_status = AES_ECB_Encrypt_Append(&aesecb_ctx, pPart, (int32_t)ulPartLen,
                                                (uint8_t *)pEncryptedPart,
                                                &lEncryptPartLen);

    }
#endif /* KMS_AES_ECB */

#ifdef KMS_AES_CCM
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CCM)
    {
      /* Encrypt Data */
      cryptolib_status = AES_CCM_Encrypt_Append(&aesccm_ctx, pPart, (int32_t)ulPartLen,
                                                pEncryptedPart, &lEncryptPartLen);

    }
#endif /* KMS_AES_CCM */


#ifdef KMS_AES_GCM
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_GCM)
    {
      /* Encrypt Data */
      cryptolib_status = AES_GCM_Encrypt_Append(&aesgcm_ctx, pPart, (int32_t)ulPartLen,
                                                pEncryptedPart, &lEncryptPartLen);

    }
#endif /* KMS_AES_GCM */

#ifdef KMS_AES_CMAC
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CMAC)
    {

      /* Encrypt Data */
      cryptolib_status = AES_CMAC_Encrypt_Append(&aescmac_ctx, pPart,
                                                 (int32_t)ulPartLen);
    }
#endif /* KMS_AES_CMAC */

  }

  *pulEncryptedPartLen = (uint32_t)lEncryptPartLen;

  /* Return status*/
  if (cryptolib_status == AES_SUCCESS)
  {
    e_ret_status = CKR_OK;
  }

  return e_ret_status;
}


/**
  * @brief  This function is called upon @ref C_EncryptFinal call
  * @note   Refer to @ref C_EncryptFinal function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pLastEncryptedPart last encrypted data part
  * @param  pulLastEncryptedPartLen length of the last encrypted data part
  * @retval Operation status
  */
CK_RV          KMS_EncryptFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pLastEncryptedPart,
                                CK_ULONG_PTR pulLastEncryptedPartLen)
{

  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  int32_t cryptolib_status = AES_ERR_BAD_OPERATION;    /* CryptoLib Error Status */
  int32_t lEncryptPartLen = 0;

  /* Check that a Processing is already on going. */
  if (KMS_GETSESSSION(hSession).hKey == KMS_HANDLE_KEY_NOT_KNOWN)
  {
    return CKR_KEY_HANDLE_INVALID;
  }
  else
  {

#ifdef KMS_AES_CCM
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CCM)
    {
      /* aesccm_ctx.pmTag = pLastEncryptedPart; */
      cryptolib_status = AES_CCM_Encrypt_Finish(&aesccm_ctx,
                                                pLastEncryptedPart,
                                                &lEncryptPartLen);
    }
#endif /* KMS_AES_CCM */

#ifdef KMS_AES_GCM
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_GCM)
    {
      aesgcm_ctx.pmTag = pLastEncryptedPart;
      cryptolib_status = AES_GCM_Encrypt_Finish(&aesgcm_ctx,
                                                pLastEncryptedPart,
                                                &lEncryptPartLen);
    }
#endif /* KMS_AES_GCM */

#ifdef KMS_AES_CBC
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CBC)
    {
      cryptolib_status = AES_CBC_Encrypt_Finish(&aescbc_ctx,
                                                pLastEncryptedPart,
                                                &lEncryptPartLen);
    }
#endif /* KMS_AES_CBC */

#ifdef KMS_AES_ECB
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_ECB)
    {
      cryptolib_status = AES_ECB_Encrypt_Finish(&aesecb_ctx,
                                                pLastEncryptedPart,
                                                &lEncryptPartLen);
    }
#endif /* KMS_AES_ECB */

#ifdef KMS_AES_CMAC
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CMAC)
    {

      aescmac_ctx.pmTag = pLastEncryptedPart;

      /* Finalize data */
      cryptolib_status = AES_CMAC_Encrypt_Finish(&aescmac_ctx,
                                                 pLastEncryptedPart,
                                                 &lEncryptPartLen);
    }
#endif /* KMS_AES_CMAC */
  }

  KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;

  if (KMS_GETSESSSION(hSession).pKeyAllocBuffer != NULL)
  {
    KMS_FreeKey(KMS_GETSESSSION(hSession).pKeyAllocBuffer);
    KMS_GETSESSSION(hSession).pKeyAllocBuffer = NULL;
  }

  /* Return status*/
  if (cryptolib_status == AES_SUCCESS)
  {
    e_ret_status = CKR_OK;
  }

  *pulLastEncryptedPartLen = (uint32_t)lEncryptPartLen;

  return e_ret_status;
}


/**
  * @brief  This function is called upon @ref C_DecryptInit call
  * @note   Refer to @ref C_DecryptInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @note   The CKA_ENCRYPT attribute of the decryption key, which indicates
  *         whether the key supports encryption, MUST be CK_TRUE
  * @param  hSession session handle
  * @param  pMechanism decryption mechanism
  * @param  hKey handle of decryption key
  * @retval Operation status
  */
CK_RV          KMS_DecryptInit(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                               CK_OBJECT_HANDLE hKey)
{

  /* We reuse similar code between Encrypt & Decrypt Init */
  return    encrypt_decrypt_init(hSession, pMechanism, hKey, KMS_FLAG_DECRYPT);

}


/**
  * @brief  This function is called upon @ref C_Decrypt call
  * @note   Refer to @ref C_Decrypt function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pEncryptedData encrypted data
  * @param  ulEncryptedDataLen length of encrypted data
  * @param  pData decrypted data
  * @param  pulDataLen length of decrypted data
  * @retval Operation status
  */
CK_RV          KMS_Decrypt(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pEncryptedData,
                           CK_ULONG ulEncryptedDataLen,
                           CK_BYTE_PTR pData, CK_ULONG_PTR pulDataLen)
{

  CK_RV e_ret_status;


  e_ret_status = KMS_DecryptUpdate(hSession, pEncryptedData, ulEncryptedDataLen,
                                   pData,   pulDataLen);


  KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;

  if (KMS_GETSESSSION(hSession).pKeyAllocBuffer != NULL)
  {
    KMS_FreeKey(KMS_GETSESSSION(hSession).pKeyAllocBuffer);
    KMS_GETSESSSION(hSession).pKeyAllocBuffer = NULL;
  }

  return (e_ret_status) ;


}


/**
  * @brief  This function is called upon @ref C_DecryptUpdate call
  * @note   Refer to @ref C_DecryptUpdate function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pEncryptedPart encrypted data part
  * @param  ulEncryptedPartLen length of encrypted data part
  * @param  pPart decrypted data part
  * @param  pulPartLen length of decrypted data part
  * @retval Operation status
  */
CK_RV          KMS_DecryptUpdate(CK_SESSION_HANDLE hSession,
                                 CK_BYTE_PTR pEncryptedPart,
                                 CK_ULONG ulEncryptedPartLen,
                                 CK_BYTE_PTR pPart, CK_ULONG_PTR pulPartLen)
{

  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  int32_t cryptolib_status = AES_ERR_BAD_OPERATION;    /* CryptoLib Error Status */
  int32_t lPartLen = 0;

  /* Check that a Processing is already on going. */
  if (KMS_GETSESSSION(hSession).hKey == KMS_HANDLE_KEY_NOT_KNOWN)
  {
    return CKR_KEY_HANDLE_INVALID;
  }
  else
  {

#ifdef KMS_AES_CBC
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CBC)
    {

      /* Encrypt Data */
      cryptolib_status = AES_CBC_Decrypt_Append(&aescbc_ctx, pEncryptedPart,
                                                (int32_t)ulEncryptedPartLen,  pPart,
                                                (int32_t *)&lPartLen);
      *pulPartLen = (uint32_t)lPartLen;

    }
#endif /* KMS_AES_CBC */

#ifdef KMS_AES_ECB
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_ECB)
    {

      /* Encrypt Data */
      cryptolib_status = AES_ECB_Decrypt_Append(&aesecb_ctx, pEncryptedPart,
                                                (int32_t)ulEncryptedPartLen,  pPart,
                                                (int32_t *)&lPartLen);
      *pulPartLen = (uint32_t)lPartLen;

    }
#endif /* KMS_AES_ECB */

#ifdef KMS_AES_CCM
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CCM)
    {

      /* Set the pointer to the TAG to be checked */
      aesccm_ctx.pmTag = &pEncryptedPart[aesccm_ctx.mPayloadSize];

      /* Encrypt Data */
      cryptolib_status = AES_CCM_Decrypt_Append(&aesccm_ctx, pEncryptedPart,
                                                (int32_t)ulEncryptedPartLen,  pPart,
                                                (int32_t *)&lPartLen);
      *pulPartLen = (uint32_t)lPartLen;
    }
#endif /* KMS_AES_CCM */

#ifdef KMS_AES_GCM
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_GCM)
    {
      /* Encrypt Data */
      cryptolib_status = AES_GCM_Decrypt_Append(&aesgcm_ctx, pEncryptedPart,
                                                (int32_t)ulEncryptedPartLen,  pPart,
                                                (int32_t *)&lPartLen);
      *pulPartLen = (uint32_t)lPartLen;

    }
#endif /* KMS_AES_GCM */

#ifdef KMS_AES_CMAC
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CMAC)
    {
      /* Encrypt Data */
      cryptolib_status = AES_CMAC_Decrypt_Append(&aescmac_ctx, pEncryptedPart,
                                                 (int32_t)ulEncryptedPartLen);
    }
#endif /* KMS_AES_CMAC */
  }


  /* Return status*/
  if (cryptolib_status == AES_SUCCESS)
  {
    e_ret_status = CKR_OK;
  }
  return e_ret_status;

}


/**
  * @brief  This function is called upon @ref C_DecryptFinal call
  * @note   Refer to @ref C_DecryptFinal function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pLastPart last decrypted data part
  * @param  pulLastPartLen length of the last decrypted data part
  * @retval Operation status
  */
CK_RV          KMS_DecryptFinal(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pLastPart, CK_ULONG_PTR pulLastPartLen)
{
  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  int32_t cryptolib_status = AES_ERR_BAD_OPERATION;    /* CryptoLib Error Status */
  int32_t lPartLen = 0;

  /* Check that a Processing is already on going. */
  if (KMS_GETSESSSION(hSession).hKey == KMS_HANDLE_KEY_NOT_KNOWN)
  {
    return CKR_KEY_HANDLE_INVALID;
  }
  else
  {


#ifdef KMS_AES_CCM
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CCM)
    {
      cryptolib_status = AES_CCM_Decrypt_Finish(&aesccm_ctx,
                                                pLastPart,
                                                (int32_t *)&lPartLen);
    }
#endif /* KMS_AES_CCM */

#ifdef KMS_AES_GCM
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_GCM)
    {
      aesgcm_ctx.pmTag = pLastPart;
      cryptolib_status = AES_GCM_Decrypt_Finish(&aesgcm_ctx,
                                                pLastPart,
                                                (int32_t *)&lPartLen);
    }
#endif /* KMS_AES_GCM */

#ifdef KMS_AES_CBC
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CBC)
    {
      cryptolib_status = AES_CBC_Decrypt_Finish(&aescbc_ctx,
                                                pLastPart,
                                                (int32_t *)&lPartLen);
    }
#endif /* KMS_AES_CBC */

#ifdef KMS_AES_ECB
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_ECB)
    {
      cryptolib_status = AES_ECB_Decrypt_Finish(&aesecb_ctx,
                                                pLastPart,
                                                (int32_t *)&lPartLen);
    }
#endif /* KMS_AES_ECB */

#ifdef KMS_AES_CMAC
    if (KMS_GETSESSSION(hSession).Mechanism == CKM_AES_CMAC)
    {
      /* aescmac_ctx.pmTag = pLastPart; */

      /* Finalize data */
      cryptolib_status = AES_CMAC_Decrypt_Finish(&aescmac_ctx,
                                                 pLastPart,
                                                 (int32_t *)&lPartLen);
    }
#endif /* KMS_AES_CMAC */
  }

  *pulLastPartLen = (uint32_t)lPartLen;

  KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;

  if (KMS_GETSESSSION(hSession).pKeyAllocBuffer != NULL)
  {
    KMS_FreeKey(KMS_GETSESSSION(hSession).pKeyAllocBuffer);
    KMS_GETSESSSION(hSession).pKeyAllocBuffer = NULL;
  }

  /* Return status*/
  if ((cryptolib_status == AES_SUCCESS) || (cryptolib_status == AUTHENTICATION_SUCCESSFUL))
  {
    e_ret_status = CKR_OK;
  }

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
