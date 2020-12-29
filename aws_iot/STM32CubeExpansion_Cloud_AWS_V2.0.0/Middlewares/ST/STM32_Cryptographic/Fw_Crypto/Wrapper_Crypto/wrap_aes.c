/**
  ******************************************************************************
  * @file    wrap_aes.c
  * @author  MCD Application Team
  * @brief   Provides wrapper functions for AES methods to abstract call to MBED
  *          cryptographic library.
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
#include "crypto.h"
#include "psa/crypto.h"
#include <string.h>
#include "wrap_config.h"

#ifdef WRAP_AES
#include "mbedtls/aes.h"

#ifdef WRAP_AES_CCM
#include "mbedtls/ccm.h"
#endif /*WRAP_AES_CCM*/

/* Private typedef -----------------------------------------------------------*/
#ifdef WRAP_AES_CBC
/**
  * @brief CBC internals values
  *
  */
typedef struct
{
  psa_cipher_operation_t cipher_op;     /*!< Psa Cipher Operation*/
  psa_key_handle_t psa_key_handle;      /*!< Psa Key Handle*/
} wrap_aes_cbc_t;
#endif /*WRAP_AES_CBC*/

#ifdef WRAP_AES_GCM
/**
  * @brief GCM internals values
  */
typedef struct
{
  psa_key_handle_t psa_key_handle;      /*!< Psa Key Handle*/
  uint8_t wrap_size_cipher;             /*!< Size of the Cipher*/
  uint8_t wrap_is_use;                  /*!< To know what operation we are doing*/
  mbedtls_gcm_context *mbedtls_ctx;     /*!< Context for the mbedtls layer*/
} wrap_aes_gcm_t;
#endif /*WRAP_AES_GCM*/

#ifdef WRAP_AES_CCM
/**
  * @brief CCM internals values
  */
typedef struct
{
  mbedtls_ccm_context *mbedtls_ctx;     /*!< Context for the mbedtls layer*/
  uint8_t const *p_wrap_aad;            /*!< AAD*/
  uint8_t *p_wrap_tag;                  /*!< Save the Tag's pointer*/
  uint8_t *p_output_save;               /*!< Save the output's pointer*/
  const uint8_t *p_input_save;          /*!< Save the input's pointer*/
  uint8_t wrap_is_use;                  /*!< Are we using the wrapper ?*/
  int32_t input_size_save;              /*!< Save the input's size*/
} wrap_aes_ccm_t;
#endif /*WRAP_AES_CCM*/

#ifdef WRAP_AES_CMAC
/**
  * @brief CMAC internals values
  */
typedef struct
{
  psa_mac_operation_t cmac_op;          /*!< Psa Mac Operation*/
  psa_key_handle_t psa_key_handle;      /*!< Psa Key Handle*/
} wrap_aes_cmac_t;
#endif /*WRAP_AES_CMAC*/

#ifdef WRAP_AES_EBC
/**
  * @brief EBC internals values
  */
typedef struct
{
  mbedtls_aes_context *mbedtls_ctx;     /*!< Context for the mbedtls layer*/
} wrap_aes_ecb_t;
#endif /*WRAP_AES_EBC*/

/* Private defines -----------------------------------------------------------*/
#define MBEDTLS_SUCCESS 0               /*!< Mbed's return's type*/
#define TAG_MAXSIZE 128                 /*!< Maximum size of the return TAG*/
#define WRAP_IS_USE 1U                   /*!< To know what operations is currently used*/
#define WRAP_IS_NOT_USE 0U               /*!< To know what operations is currently used*/


/* Private variables ---------------------------------------------------------*/
#ifdef WRAP_AES_CBC
static wrap_aes_cbc_t wrap_aes_cbc_enc = {PSA_CIPHER_OPERATION_INIT, 0};
static wrap_aes_cbc_t wrap_aes_cbc_dec = {PSA_CIPHER_OPERATION_INIT, 0};
#endif /*WRAP_AES_CBC*/

#ifdef WRAP_AES_GCM
/*To not use any dynamic allocation*/
static mbedtls_gcm_context static_mbedtls_gcm_enc_ctx;
static mbedtls_gcm_context static_mbedtls_gcm_dec_ctx;
static wrap_aes_gcm_t wrap_aes_gcm_enc = {0, 0, 0, &static_mbedtls_gcm_enc_ctx};
static wrap_aes_gcm_t wrap_aes_gcm_dec = {0, 0, 0, &static_mbedtls_gcm_dec_ctx};
#endif /*WRAP_AES_GCM*/

#ifdef WRAP_AES_CCM
/*To not use any dynamic allocation*/
static mbedtls_ccm_context static_mbedtls_ccm_enc_ctx;
static uint8_t static_wrap_enc_tag[TAG_MAXSIZE];
static mbedtls_ccm_context static_mbedtls_ccm_dec_ctx;
static uint8_t static_wrap_dec_tag[TAG_MAXSIZE];

static wrap_aes_ccm_t wrap_aes_ccm_enc = {&static_mbedtls_ccm_enc_ctx,
                                          NULL,
                                          static_wrap_enc_tag,
                                          NULL,
                                          NULL,
                                          0,
                                          0};
static wrap_aes_ccm_t wrap_aes_ccm_dec = {&static_mbedtls_ccm_dec_ctx,
                                          NULL,
                                          static_wrap_dec_tag,
                                          NULL,
                                          NULL,
                                          0,
                                          0};
#endif /*WRAP_AES_CCM*/

#ifdef WRAP_AES_CMAC
static wrap_aes_cmac_t wrap_aes_cmac_enc = {{0}, 0};
static wrap_aes_cmac_t wrap_aes_cmac_dec = {{0}, 0};
#endif /*WRAP_AES_CMAC*/

#ifdef WRAP_AES_EBC
/*To not use any dynamic allocation*/
static mbedtls_aes_context static_mbedtls_ecb_enc_ctx;
static mbedtls_aes_context static_mbedtls_ecb_dec_ctx;
static wrap_aes_ecb_t wrap_aes_ecb_enc = {&static_mbedtls_ecb_enc_ctx};
static wrap_aes_ecb_t wrap_aes_ecb_dec = {&static_mbedtls_ecb_dec_ctx};
#endif /*WRAP_AES_EBC*/

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief      Change the iv's format from a psa format to a cryptoLib one.
    *           (from a continuous buffer to a buffer dived into 4 sections)
  * @param[in]  *P_pPsaIv: IV in psa format
  * @param[out] *P_pCryptoIv: IV in CryptoLib format
  *
  */
static void wrap_iv_psa_to_crypto(uint8_t  *P_pPsaIv,
                                  uint32_t *P_pCryptoIv)
{
  uint8_t i;
  uint8_t j;

  for (i = 0; i < 4U; i++) /*4 is the size of the array amIv*/
  {
    P_pCryptoIv[i] = 0;
    for (j = 0U; j < MBEDTLS_MAX_IV_LENGTH / 4U; j++)
    {
      P_pCryptoIv[i] += (P_pPsaIv[j + (i*4U)] << ( 8U *( (MBEDTLS_MAX_IV_LENGTH / 4U) - j - 1U) ));
    }
  }
}

/**
  * @brief      Import a raw key into a psa struct with the good parameters
  * @param[in,out]
                *P_Key_Handle: Handle of the key
  * @param[in]  P_Psa_Usage: Key usage
  * @param[in]  P_Psa_Algorithm: Algorithm that will be used with the key
  * @param[in]  *P_pAes_Key: The key
  * @param[in]  P_KeySize: The size of the key in bytes
  * @retval     AES_ERR_BAD_PARAMETER: Could not import the key
  * @retval     AES_SUCCESS: Operation Successful
  */
static psa_status_t wrap_import_raw_aes_key_into_psa(psa_key_handle_t *P_Key_Handle,
                                                     psa_key_usage_t P_Psa_Usage,
                                                     psa_algorithm_t  P_Psa_Algorithm,
                                                     const uint8_t *P_pAes_Key,
                                                     uint32_t P_KeySize)
{
  psa_status_t psa_ret_status;
  psa_key_policy_t psa_key_policy = {0};

  psa_ret_status = psa_allocate_key(P_Key_Handle);
  if (psa_ret_status == AES_SUCCESS)
  {
    /*Link the Algorithm with the Key*/
    psa_key_policy_set_usage(&psa_key_policy, P_Psa_Usage, P_Psa_Algorithm);
    psa_ret_status = psa_set_key_policy(*P_Key_Handle, &psa_key_policy);

    if (psa_ret_status == AES_SUCCESS)
    {
      /* Transform the raw key into the right format*/
      psa_ret_status = psa_import_key(*P_Key_Handle,
                                      PSA_KEY_TYPE_AES,
                                      P_pAes_Key,
                                      P_KeySize);
    }
    else
    {
      psa_ret_status = AES_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    psa_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return psa_ret_status;
}

/* Functions Definition ------------------------------------------------------*/

#ifdef WRAP_AES_CBC

/**
  * @brief      Initialization for AES Encryption in CBC Mode
  * @param[in,out]
  *             *P_pAESCBCctx: AES CBC context
  * @param[in]  *P_pKey: Buffer with the Key
  * @param[in]  *P_pIv: Buffer with the IV
  * @note       1. P_pAESCBCctx.mKeySize (see AESCBCctx_stt) must be set with
  *             the size of the key prior to calling this function.
  *             Instead of the size of the key, you can also use the following
  *             predefined values:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  *             2. P_pAESCBCctx.mIvSize must be set with the size of the IV
  *             (default CRL_AES_BLOCK) prior to calling this function.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values.
  *                                  See note
  */
int32_t AES_CBC_Encrypt_Init(AESCBCctx_stt *P_pAESCBCctx,
                             const uint8_t *P_pKey,
                             const uint8_t *P_pIv)
{
  int32_t aes_ret_status = AES_SUCCESS;
  psa_status_t psa_ret_status;
  const psa_algorithm_t psa_algorithm = PSA_ALG_CBC_NO_PADDING;

  if ((P_pAESCBCctx == NULL) || (P_pKey == NULL) || (P_pIv == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_pAESCBCctx->mKeySize == 0)
      || (P_pAESCBCctx->mIvSize == 0))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /*Fill the ctx with pointers*/
  if (P_pAESCBCctx->pmKey != P_pKey)
  {
    P_pAESCBCctx->pmKey = P_pKey;
  }
  if (P_pAESCBCctx->pmIv != P_pIv)
  {
    P_pAESCBCctx->pmIv = P_pIv;
  }

  /*Output the IV into the right format*/
  //wrap_iv_psa_to_crypto((uint8_t *)P_pIv, P_pAESCBCctx->amIv);

  wrap_aes_cbc_enc.cipher_op = psa_cipher_operation_init();

  /* Initialize MBED crypto library*/
  psa_ret_status = psa_crypto_init();

  if ( (psa_ret_status == PSA_SUCCESS) && (aes_ret_status == AES_SUCCESS) )
  {
    /*Import the raw AES key into a PSA struct*/
    psa_ret_status = wrap_import_raw_aes_key_into_psa(&(wrap_aes_cbc_enc.psa_key_handle),
                                                      PSA_KEY_USAGE_ENCRYPT,
                                                      psa_algorithm,
                                                      P_pKey,
                                                      (uint32_t) P_pAESCBCctx->mKeySize);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Init the operation and setup the encryption*/
      psa_ret_status = psa_cipher_encrypt_setup(&(wrap_aes_cbc_enc.cipher_op),
                                                wrap_aes_cbc_enc.psa_key_handle,
                                                psa_algorithm);
      if (psa_ret_status == PSA_SUCCESS)
      {
        psa_ret_status = psa_cipher_set_iv(&(wrap_aes_cbc_enc.cipher_op),
                                           P_pIv,
                                           (uint32_t) P_pAESCBCctx->mIvSize);
      }
      else
      {
        aes_ret_status = AES_ERR_BAD_PARAMETER;
      }
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}

/**
  * @brief      AES Encryption in CBC Mode
  * @param[in]  *P_pAESCBCctx: AES CBC, already initialized, context
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data, expressed in bytes
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Pointer to integer containing size of written
  *             output data, expressed in bytes
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer.
  * @retval     AES_ERR_BAD_INPUT_SIZE:Size of input is less than CRL_AES_BLOCK
  * @retval     AES_ERR_BAD_OPERATION: Append not allowed.
  */
int32_t AES_CBC_Encrypt_Append(AESCBCctx_stt *P_pAESCBCctx,
                               const uint8_t *P_pInputBuffer,
                               int32_t P_inputSize,
                               uint8_t *P_pOutputBuffer,
                               int32_t *P_pOutputSize)
{
  psa_status_t psa_ret_status;
  int32_t aes_ret_status;

  if ((P_pAESCBCctx == NULL)
      || (P_pInputBuffer == NULL)
      || (P_pOutputBuffer == NULL)
      || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_inputSize < CRL_AES_BLOCK)
  {
    return AES_ERR_BAD_INPUT_SIZE;
  }

  psa_ret_status = psa_cipher_update(&(wrap_aes_cbc_enc.cipher_op),
                                     P_pInputBuffer,
                                     (uint32_t) P_inputSize,
                                     P_pOutputBuffer,
                                     (uint32_t) P_inputSize,
                                     (size_t *)P_pOutputSize);

  if (psa_ret_status == PSA_SUCCESS)
  {
    aes_ret_status = AES_SUCCESS;
    /*Update of the ouput context with the new IV*/
    wrap_iv_psa_to_crypto(wrap_aes_cbc_enc.cipher_op.ctx.cipher.iv, P_pAESCBCctx->amIv);
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }

  return aes_ret_status;
}

/**
  * @brief      AES Finalization of CBC mode
  * @param[in,out]
  *             *P_pAESCBCctx: AES CBC, already initialized, context
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Pointer to integer containing size of written
  *             output data, in bytes
  * @note       This function will write output data.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  */
int32_t AES_CBC_Encrypt_Finish(AESCBCctx_stt *P_pAESCBCctx,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  psa_status_t psa_ret_status;
  int32_t aes_ret_status;

  if ((P_pAESCBCctx == NULL) || (P_pOutputBuffer == NULL) || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }

  psa_ret_status = psa_destroy_key(wrap_aes_cbc_enc.psa_key_handle);
  if (psa_ret_status == PSA_SUCCESS)
  {
    psa_ret_status = psa_cipher_finish(&(wrap_aes_cbc_enc.cipher_op),
                                       P_pOutputBuffer,
                                       (uint32_t) P_pAESCBCctx->mIvSize,
                                       (size_t *)P_pOutputSize);
    if (psa_ret_status == PSA_SUCCESS)
    {
      aes_ret_status = AES_SUCCESS;
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    return AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}

/**
  * @brief      Initialization for AES Decryption in CBC Mode
  * @param[in,out]
  *             *P_pAESCBCctx: AES CBC context
  * @param[in]  *P_pKey: Buffer with the Key
  * @param[in]  *P_pIv: Buffer with the IV
  * @note       1. P_pAESCBCctx.mKeySize (see AESCBCctx_stt) must be set with
  *             the size of the key prior to calling this function.
  *             Instead of the size of the key, you can also use the following
  *             predefined values:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  *             2. P_pAESCBCctx.mIvSize must be set with the size of the IV
  *             (default CRL_AES_BLOCK) prior to calling this function.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values.
  *                                  See note
  */
int32_t AES_CBC_Decrypt_Init(AESCBCctx_stt *P_pAESCBCctx,
                             const uint8_t *P_pKey,
                             const uint8_t *P_pIv)
{
  int32_t aes_ret_status = AES_SUCCESS;
  psa_status_t psa_ret_status;
  const psa_algorithm_t psa_algorithm = PSA_ALG_CBC_NO_PADDING;

  if ((P_pAESCBCctx == NULL) || (P_pKey == NULL) || (P_pIv == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_pAESCBCctx->mIvSize == 0) || (P_pAESCBCctx->mKeySize == 0))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /*Output the IV into the right format*/
  wrap_iv_psa_to_crypto((uint8_t *)P_pIv, P_pAESCBCctx->amIv);

  wrap_aes_cbc_dec.cipher_op = psa_cipher_operation_init();
  /* Initialize MBED crypto library*/
  psa_ret_status = psa_crypto_init();
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Import the raw AES key into a PSA struct*/
    psa_ret_status = wrap_import_raw_aes_key_into_psa(&(wrap_aes_cbc_dec.psa_key_handle),
                                                      PSA_KEY_USAGE_DECRYPT,
                                                      psa_algorithm,
                                                      P_pKey,
                                                      (uint32_t) P_pAESCBCctx->mKeySize);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Cipher setup*/
      psa_ret_status = psa_cipher_decrypt_setup(&wrap_aes_cbc_dec.cipher_op,
                                                wrap_aes_cbc_dec.psa_key_handle,
                                                psa_algorithm);
      if (psa_ret_status == PSA_SUCCESS)
      {
        psa_ret_status = psa_cipher_set_iv(&wrap_aes_cbc_dec.cipher_op,
                                           P_pIv,
                                           (uint32_t) P_pAESCBCctx->mIvSize);
      }
      else
      {
        aes_ret_status =  AES_ERR_BAD_PARAMETER;
      }
    }
    else
    {
      aes_ret_status =  AES_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }

  return aes_ret_status;
}

/**
  * @brief      AES Decryption in CBC Mode
  * @param[in]  *P_pAESCBCctx: AES CBC context
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data in bytes
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Size of written output data in bytes
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer.
  * @retval     AES_ERR_BAD_INPUT_SIZE: P_inputSize < 16
  * @retval     AES_ERR_BAD_OPERATION: Append not allowed.
  */
int32_t AES_CBC_Decrypt_Append(AESCBCctx_stt *P_pAESCBCctx,
                               const uint8_t *P_pInputBuffer,
                               int32_t        P_inputSize,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t aes_ret_status = AES_SUCCESS;
  psa_status_t psa_ret_status;

  if ((P_pAESCBCctx == NULL)
      || (P_pInputBuffer == NULL)
      || (P_pOutputBuffer == NULL)
      || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_inputSize < CRL_AES_BLOCK)
  {
    return AES_ERR_BAD_INPUT_SIZE;
  }

  psa_ret_status = psa_cipher_update(&(wrap_aes_cbc_dec.cipher_op),
                                     P_pInputBuffer,
                                     (uint32_t) P_inputSize,
                                     P_pOutputBuffer,
                                     (uint32_t) P_inputSize,
                                     (size_t *)P_pOutputSize);
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Update of the ouput context with the new IV*/
    wrap_iv_psa_to_crypto(wrap_aes_cbc_dec.cipher_op.ctx.cipher.iv,
                          P_pAESCBCctx->amIv);
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }

  return aes_ret_status;
}

/**
  * @brief      AES Decryption Finalization of CBC mode
  * @param[in,out]
  *             *P_pAESCBCctx: AES CBC, already initialized, context
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Pointer to integer containing size of written
  *             output data, in bytes
  * @note       This function will write output data.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  */
int32_t AES_CBC_Decrypt_Finish(AESCBCctx_stt *P_pAESCBCctx,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t aes_ret_status;
  psa_status_t psa_ret_status;

  if ((P_pAESCBCctx == NULL) || (P_pOutputBuffer == NULL) || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }

  psa_ret_status = psa_destroy_key(wrap_aes_cbc_dec.psa_key_handle);
  if (psa_ret_status == PSA_SUCCESS)
  {
    psa_ret_status = psa_cipher_finish(&(wrap_aes_cbc_dec.cipher_op),
                                       P_pOutputBuffer,
                                       (uint32_t) P_pAESCBCctx->mIvSize,
                                       (size_t *)P_pOutputSize);
    if (psa_ret_status == PSA_SUCCESS)
    {
      aes_ret_status = AES_SUCCESS;
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}
#endif /*WRAP_AES_CBC*/

#ifdef WRAP_AES_GCM

/**
  * @brief      Initialization for AES GCM encryption
  * @param[in,out]
  *             *P_pAESGCMctx: AES GCM context
  * @param[in]  *P_pKey: Buffer with the Key
  * @param[out] *P_pIv: Buffer with the IV

  * @note       1. P_pAESGCMctx.mKeySize (see AESGCMctx_stt) must be set with
  *             the size of the key prior to calling this function.
  *             Otherwise the following predefined values can be used:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  *             2. P_pAESGCMctx.mIvSize must be set with the size of the IV
  *             (12 is the only supported value) prior to calling this function.
  *             3. P_pAESGCMctx.mTagSize must be set with the size of
  *             authentication TAG that will be generated by the
  *             AES_GCM_Encrypt_Finish.
  *             4. Following recommendation by NIST expressed in section 5.2.1.1
  *             of NIST SP 800-38D, this implementation supports only IV whose
  *             size is of 96 bits.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values,
  *                                  see note
  */
int32_t AES_GCM_Encrypt_Init(AESGCMctx_stt *P_pAESGCMctx,
                             const uint8_t *P_pKey,
                             const uint8_t *P_pIv)
{
  int32_t aes_ret_status = AES_SUCCESS;
  int32_t mbedtls_status;
  if ((P_pAESGCMctx == NULL) || (P_pKey == NULL) || (P_pIv == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  /* 12 is the only valid value for the Iv size */
  if ((P_pAESGCMctx->mKeySize == 0)
      || (P_pAESGCMctx->mTagSize == 0)
      || (P_pAESGCMctx->mIvSize != 12))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /*Fill the ctx*/
  if (P_pAESGCMctx->pmKey != P_pKey)
  {
    P_pAESGCMctx->pmKey = P_pKey;
  }
  if (P_pAESGCMctx->pmIv != P_pIv)
  {
    P_pAESGCMctx->pmIv = P_pIv;
  }
  
  /*Init mbedtls*/
  mbedtls_gcm_init(wrap_aes_gcm_enc.mbedtls_ctx);
  /*Set the key*/
  mbedtls_status = mbedtls_gcm_setkey(wrap_aes_gcm_enc.mbedtls_ctx,
                                      MBEDTLS_CIPHER_ID_AES,
                                      P_pKey,
                                      P_pAESGCMctx->mKeySize * 8U); /* 8: to pass from bytes to bits*/
  if (mbedtls_status == MBEDTLS_SUCCESS)
  {
    /* We are doing an encryption, so we set the wrap_is_use var to know it*/
    wrap_aes_gcm_enc.wrap_is_use = WRAP_IS_USE;
    /*Output the IV into the right format*/
    wrap_iv_psa_to_crypto((uint8_t *)P_pIv, P_pAESGCMctx->amIv);
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }
  
  return aes_ret_status;
}

/**
  * @brief      AES GCM Header processing function
  * @param[in,out]
  *             *P_pAESGCMctx: AES GCM, already initialized, context
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data, expressed in bytes
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_OPERATION Append not allowed
  */
int32_t AES_GCM_Header_Append(AESGCMctx_stt *P_pAESGCMctx,
                              const uint8_t *P_pInputBuffer,
                              int32_t        P_inputSize)
{
  int32_t aes_ret_status;
  int32_t mbedtls_status;
  
  /*Manage AAD: input = AAD*/
  if ((P_pAESGCMctx == NULL) || (P_pInputBuffer == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (wrap_aes_gcm_enc.wrap_is_use == WRAP_IS_USE)
  {
    P_pAESGCMctx->mAADsize = P_inputSize; /*Update done by the ST lib*/
    mbedtls_status = mbedtls_gcm_starts(wrap_aes_gcm_enc.mbedtls_ctx,
                                        MBEDTLS_GCM_ENCRYPT,
                                        P_pAESGCMctx->pmIv,
                                        (uint32_t) P_pAESGCMctx->mIvSize,
                                        P_pInputBuffer,
                                        (uint32_t) P_pAESGCMctx->mAADsize);
    if (mbedtls_status == MBEDTLS_SUCCESS)
    {
      aes_ret_status = AES_SUCCESS;
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_OPERATION;
    }
  }

  /*Or are we decrypting something ?*/
  else if (wrap_aes_gcm_dec.wrap_is_use == WRAP_IS_USE)
  {
    P_pAESGCMctx->mAADsize = P_inputSize; /*Update done by the ST lib*/
    mbedtls_status = mbedtls_gcm_starts(wrap_aes_gcm_dec.mbedtls_ctx,
                                        MBEDTLS_GCM_DECRYPT,
                                        P_pAESGCMctx->pmIv,
                                        (uint32_t) P_pAESGCMctx->mIvSize,
                                        P_pInputBuffer,
                                        (uint32_t) P_pAESGCMctx->mAADsize);
    if (mbedtls_status == MBEDTLS_SUCCESS)
    {
      aes_ret_status = AES_SUCCESS;
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_OPERATION;
    }
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }

  return aes_ret_status;
}

/**
  * @brief      AES GCM Encryption function
  * @param[in,out]
  *             *P_pAESGCMctx: AES GCM, already initialized, context
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data, expressed in bytes
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Pointer to integer that will contain the size
  *             of written output data, expressed in bytes
  * @retval    AES_SUCCESS: Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval    AES_ERR_BAD_OPERATION: Append not allowed
  */
int32_t AES_GCM_Encrypt_Append(AESGCMctx_stt *P_pAESGCMctx,
                               const uint8_t *P_pInputBuffer,
                               int32_t        P_inputSize,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t mbedtls_status;
  if ((P_pAESGCMctx == NULL)
      || (P_pInputBuffer == NULL)
      || (P_pOutputBuffer == NULL)
      || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  mbedtls_status = mbedtls_gcm_update(wrap_aes_gcm_enc.mbedtls_ctx,
                                      (uint32_t) P_inputSize,
                                      P_pInputBuffer,
                                      P_pOutputBuffer);
  /*Put data into struct*/
  if (mbedtls_status == MBEDTLS_SUCCESS)
  {
    *P_pOutputSize = P_inputSize;
    /*Transform the IV into the right format*/
    wrap_iv_psa_to_crypto((uint8_t *)P_pAESGCMctx->pmIv, P_pAESGCMctx->amIv);
  }
  else
  {
    return AES_ERR_BAD_OPERATION;
  }
  return AES_SUCCESS;
}

/**
  * @brief      AES GCM Finalization during encryption, this will create the Authentication TAG
  * @param[in,out]
  *             *P_pAESGCMctx: AES GCM, already initialized, context
  * @param[out] *P_pOutputBuffer: Output Authentication TAG
  * @param[out] *P_pOutputSize: Size of returned TAG
  * @note       This function requires P_pAESGCMctx mTagSize to contain a valid
  *             value between 1 and 16
  * @retval     AES_SUCCESS: Operation Successfu
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values.
  *                                  See note
  */
int32_t AES_GCM_Encrypt_Finish(AESGCMctx_stt *P_pAESGCMctx,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t aes_ret_status = AES_SUCCESS;
  int32_t mbedtls_status;
  if ((P_pAESGCMctx == NULL) || (P_pOutputBuffer == NULL) || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_pAESGCMctx->mTagSize < 0) || (P_pAESGCMctx->mTagSize > 16))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  mbedtls_status = mbedtls_gcm_finish(wrap_aes_gcm_enc.mbedtls_ctx,
                                      P_pOutputBuffer,
                                      (uint32_t) P_pAESGCMctx->mTagSize);
  if (mbedtls_status == MBEDTLS_SUCCESS)
  {
    *P_pOutputSize = P_pAESGCMctx->mTagSize;
    mbedtls_gcm_free(wrap_aes_gcm_enc.mbedtls_ctx);
    /*The encryption is finished*/
    wrap_aes_gcm_enc.wrap_is_use = WRAP_IS_NOT_USE;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_CONTEXT;
  }

  return aes_ret_status;
}

/**
  * @brief      Initialization for AES GCM Decryption
  * @param[in,out]
  *             *P_pAESGCMctx: AES GCM context
  * @param[in]  *P_pKey: Buffer with the Key
  * @param[out] *P_pIv: Buffer with the IV

  * @note       1. P_pAESGCMctx.mKeySize (see AESGCMctx_stt) must be set with
  *             the size of the key prior to calling this function.
  *             Otherwise the following predefined values can be used:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  *             2. P_pAESGCMctx.mIvSize must be set with the size of the IV
  *             (12 is the only supported value) prior to calling this function.
  *             4. Following recommendation by NIST expressed in section 5.2.1.1
  *             of NIST SP 800-38D, this implementation supports only IV whose
  *             size is of 96 bits.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values,
  *                                  see note
  */
int32_t AES_GCM_Decrypt_Init(AESGCMctx_stt *P_pAESGCMctx,
                             const uint8_t *P_pKey,
                             const uint8_t *P_pIv)
{
  int32_t aes_ret_status = AES_SUCCESS;
  int8_t  mbedtls_status;
  if ((P_pAESGCMctx == NULL) || (P_pKey == NULL) || (P_pIv == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  /* 12 is the only valid value for the Iv size */
  if ((P_pAESGCMctx->mKeySize == 0)
      || (P_pAESGCMctx->mTagSize == 0)
      || (P_pAESGCMctx->mIvSize != 12))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /*Fill the ctx*/
  if (P_pAESGCMctx->pmKey != P_pKey)
  {
    P_pAESGCMctx->pmKey = P_pKey;
  }
  if (P_pAESGCMctx->pmIv != P_pIv)
  {
    P_pAESGCMctx->pmIv = P_pIv;
  }
  
  /*Init mbedtls*/
  mbedtls_gcm_init(wrap_aes_gcm_dec.mbedtls_ctx);
  mbedtls_status = mbedtls_gcm_setkey(wrap_aes_gcm_dec.mbedtls_ctx,
                                      MBEDTLS_CIPHER_ID_AES,
                                      P_pKey,
                                      P_pAESGCMctx->mKeySize * 8U);
  if (mbedtls_status == MBEDTLS_SUCCESS)
  {
    /* We are doing a decryption, so we set the wrap_is_use var to know it*/
    wrap_aes_gcm_dec.wrap_is_use = WRAP_IS_USE;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}

/**
  * @brief      AES GCM Decryption function
  * @param[in,out]
  *             *P_pAESGCMctx: AES GCM context
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data in uint8_t (octets)
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Size of written output data in uint8_t
  * @retval    AES_SUCCESS: Operation Successful
  * @retval    AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval    AES_ERR_BAD_OPERATION: Append not allowed
  */
int32_t AES_GCM_Decrypt_Append(AESGCMctx_stt *P_pAESGCMctx,
                               const uint8_t *P_pInputBuffer,
                               int32_t        P_inputSize,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t aes_ret_status = AES_SUCCESS;
  int32_t mbedtls_status;
  if ((P_pAESGCMctx == NULL)
      || (P_pInputBuffer == NULL)
      || (P_pOutputBuffer == NULL)
      || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }

  mbedtls_status = mbedtls_gcm_update(wrap_aes_gcm_dec.mbedtls_ctx,
                                      (uint32_t) P_inputSize,
                                      P_pInputBuffer,
                                      P_pOutputBuffer);
  if (mbedtls_status == MBEDTLS_SUCCESS)
  {
    *P_pOutputSize = P_inputSize;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }

  return aes_ret_status;
}

/**
  * @brief      AES GCM Finalization during decryption, the authentication
  *             TAG will be checked
  * @param[in,out]
  *             *P_pAESGCMctx: AES GCM, already initialized, context
  * @param[out] *P_pOutputBuffer: Kept for API compatibility but won't be used,
  *             should be NULL
  * @param[out] *P_pOutputSize: Kept for API compatibility, must be provided
  *             but will be set to zero
  * @note       This function requires:
  *             P_pAESGCMctx->pmTag to be set to a valid pointer to the tag
  *             to be checked.
  *             P_pAESGCMctx->mTagSize to contain a valid value between 1 and 16
  * @retval     AUTHENTICATION_SUCCESSFUL: The Tag is verified
  * @retval     AUTHENTICATION_FAILED: The Tag is not verified
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values.
  *                                  See note
  */
int32_t AES_GCM_Decrypt_Finish(AESGCMctx_stt *P_pAESGCMctx,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t aes_ret_status;
  int8_t mbedtls_status;
  if ((P_pAESGCMctx == NULL) || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_pAESGCMctx == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_pAESGCMctx->pmTag == NULL)
      || ((P_pAESGCMctx->mTagSize > 0) && (P_pAESGCMctx->mTagSize < 16)))
  {
    return AES_ERR_BAD_OPERATION;
  }

  mbedtls_status = mbedtls_gcm_finish(wrap_aes_gcm_dec.mbedtls_ctx,
                                      (unsigned char *)P_pAESGCMctx->pmTag,
                                      (uint32_t) P_pAESGCMctx->mTagSize);
  if (mbedtls_status == MBEDTLS_ERR_GCM_AUTH_FAILED)
  {
    aes_ret_status = AUTHENTICATION_FAILED;
  }
  else if (mbedtls_status == MBEDTLS_SUCCESS)
  {
    aes_ret_status = AUTHENTICATION_SUCCESSFUL;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }
  mbedtls_gcm_free(wrap_aes_gcm_dec.mbedtls_ctx);

  /*The encryption is finished*/
  wrap_aes_gcm_dec.wrap_is_use = WRAP_IS_NOT_USE;

  /*According to documentation as to be set to 0*/
  *P_pOutputSize = 0;
  return aes_ret_status;
}
#endif /*WRAP_AES_GCM*/

#ifdef WRAP_AES_CCM

/**
  * @brief      Initialization for AES CCM encryption
  * @param[in, out]
  *             *P_pAESCCMctx: AES CCM context
  * @param[in]  *P_pKey: Buffer with the Key
  * @param[in]  *P_pNonce: Buffer with the Nonce
  * @note       1. P_pAESCCMctx->mKeySize (see AESCCMctx_stt) must be set
  *             with the size of the key prior to calling this function.
  *             Otherwise the following predefined values can be used:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  *             2. P_pAESCCMctx->mNonceSize must be set with the size
  *             of the CCM Nonce. Possible values are {7,8,9,10,11,12,13}.
  *             3. P_pAESCCMctx->mAssDataSize must be set with the size of
  *             the Associated Data (i.e. Header or any data that will be
  *             authenticated but not encrypted).
  *             4. P_pAESCCMctx->mPayloadSize must be set with the size of
  *             the Payload (i.e. Data that will be authenticated and encrypted).
  * @retval     AES_ERR_BAD_PARAMETER: Operation NOT Successful
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values
  * @retval     AES_SUCCESS: Operation Successful
  */
int32_t AES_CCM_Encrypt_Init(AESCCMctx_stt *P_pAESCCMctx,
                             const uint8_t *P_pKey,
                             const uint8_t *P_pNonce)
{
  int32_t mbedtls_ret_status;
  int32_t aes_ret_status = AES_SUCCESS;

  if ( (P_pAESCCMctx == NULL)
       || (P_pKey == NULL)
       || (P_pNonce == NULL) )
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_pAESCCMctx->mKeySize <= 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }
  if ( (P_pAESCCMctx->mNonceSize < 7)  && (P_pAESCCMctx->mNonceSize > 13) )
  {
    return AES_ERR_BAD_CONTEXT;
  }
  if (P_pAESCCMctx->mTagSize <= 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }
  if (P_pAESCCMctx->mAssDataSize < 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }
  if (P_pAESCCMctx->mPayloadSize  < 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /* Initialize MBED crypto library*/
  mbedtls_ccm_init(wrap_aes_ccm_enc.mbedtls_ctx);
  
  /*Import the raw AES key into a PSA struct*/
  mbedtls_ret_status = mbedtls_ccm_setkey(wrap_aes_ccm_enc.mbedtls_ctx,
                                          MBEDTLS_CIPHER_ID_AES,
                                          P_pKey,
                                          P_pAESCCMctx->mKeySize * 8U);
  if (mbedtls_ret_status == MBEDTLS_SUCCESS)
  {
    /*Save the data*/
    P_pAESCCMctx->pmKey = P_pKey;
    P_pAESCCMctx->pmNonce = P_pNonce;

    /*We are doing an encryption*/
    wrap_aes_ccm_enc.wrap_is_use = WRAP_IS_USE;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }
  return aes_ret_status;
}

/**
  * @brief      AES CCM Header processing function
  * @param[in, out]
  *             *P_pAESCCMctx: AES CCM context
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data, expressed in bytes
  * @retval     AES_ERR_BAD_PARAMETER: P_pAESCCMctx is a NULL pointer
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_OPERATION : Append not allowed
  */
int32_t AES_CCM_Header_Append(AESCCMctx_stt *P_pAESCCMctx,
                              const uint8_t *P_pInputBuffer,
                              int32_t        P_inputSize)
{
  int32_t aes_ret_status = AES_ERR_BAD_OPERATION;

  if (P_pAESCCMctx == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  /* No AAD is possible*/
  if (P_pInputBuffer == NULL)
  {
    return AES_SUCCESS;
  }
  else
  {
    if (wrap_aes_ccm_enc.wrap_is_use == WRAP_IS_USE)
    {
      wrap_aes_ccm_enc.p_wrap_aad = P_pInputBuffer;
      P_pAESCCMctx->mAssDataSize = P_inputSize;
      aes_ret_status = AES_SUCCESS;
    }
    if (wrap_aes_ccm_dec.wrap_is_use == WRAP_IS_USE)
    {
      wrap_aes_ccm_dec.p_wrap_aad = P_pInputBuffer;
      P_pAESCCMctx->mAssDataSize  = P_inputSize;
      aes_ret_status = AES_SUCCESS;
    }
  }
  return aes_ret_status;
}

/**
  * @brief      AES CCM Encryption function
  * @param[in, out]
  *             *P_pAESCCMctx: AES CCM context
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data, expressed in bytes
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Size of written output data, expressed in bytes
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a not valid
  * @retval     AES_ERR_BAD_OPERATION: Append not allowed
  */
int32_t AES_CCM_Encrypt_Append(AESCCMctx_stt *P_pAESCCMctx,
                               const uint8_t *P_pInputBuffer,
                               int32_t        P_inputSize,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t mbedtls_ret_status;
  int32_t aes_ret_status;

  if ((P_pAESCCMctx == NULL)
       || (P_pInputBuffer == NULL)
       || (P_pOutputBuffer == NULL)
       || (P_pOutputSize == NULL) )
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_inputSize < 0)
  {
    return AES_ERR_BAD_PARAMETER;
  }

  mbedtls_ret_status = mbedtls_ccm_encrypt_and_tag(wrap_aes_ccm_enc.mbedtls_ctx,
                                                   (uint32_t) P_inputSize,
                                                   P_pAESCCMctx->pmNonce,
                                                   (uint32_t) P_pAESCCMctx->mNonceSize,
                                                   wrap_aes_ccm_enc.p_wrap_aad,
                                                   (uint32_t) P_pAESCCMctx->mAssDataSize,
                                                   P_pInputBuffer,
                                                   P_pOutputBuffer,
                                                   wrap_aes_ccm_enc.p_wrap_tag,
                                                   (size_t) P_pAESCCMctx->mTagSize);
  if (mbedtls_ret_status == MBEDTLS_SUCCESS)
  {
    *P_pOutputSize = P_inputSize;
    aes_ret_status = AES_SUCCESS;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }

  return aes_ret_status;
}

/**
  * @brief      AES CCM Finalization during encryption,
  *             this will create the Authentication TAG
  * @param[in, out]
  *             *P_pAESCCMctx: AES CCM context
  * @param[out] *P_pOutputBuffer: Output Authentication TAG
  * @param[out] *P_pOutputSize: Size of returned TAG
  * @retval     AES_SUCCESS: Operation Successful
  */
int32_t AES_CCM_Encrypt_Finish(AESCCMctx_stt *P_pAESCCMctx,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  /*Output data*/
  (void)memcpy(P_pOutputBuffer, wrap_aes_ccm_enc.p_wrap_tag, (uint32_t) P_pAESCCMctx->mTagSize);
  *P_pOutputSize = P_pAESCCMctx->mTagSize;

  /*Reset tag*/
  (void)memset(wrap_aes_ccm_enc.p_wrap_tag, 0, (uint32_t) P_pAESCCMctx->mTagSize);

  mbedtls_ccm_free(wrap_aes_ccm_enc.mbedtls_ctx);
  wrap_aes_ccm_enc.wrap_is_use = WRAP_IS_NOT_USE;
  return AES_SUCCESS;
}

/**
  * @brief       Initialization for AES CCM Decryption
  * @param[in, out]
  *             *P_pAESCCMctx: AES CCM context
  * @param[in]  *P_pKey: Buffer with the Key
  * @param[in]  *P_pNonce: Buffer with the Nonce
  * @note       1. P_pAESCCMctx->mKeySize (see AESCCMctx_stt) must be set
  *             with the size of the key prior to calling this function.
  *             Otherwise the following predefined values can be used:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  *             2. P_pAESCCMctx->mNonceSize must be set with the size
  *             of the CCM Nonce. Possible values are {7,8,9,10,11,12,13}.
  *             3. P_pAESCCMctx->mTagSize must be set with the size of
  *             authentication TAG that will be generated by
  *             the AES_CCM_Encrypt_Finish.
  *             Possible values are values are {4,6,8,10,12,14,16}.
  *             4. P_pAESCCMctx->mAssDataSize must be set with the size of
  *             the Associated Data (i.e. Header or any data that will be
  *             authenticated but not encrypted).
  *             5. P_pAESCCMctx->mPayloadSize must be set with the size of
  *             the Payload (i.e. Data that will be authenticated and encrypted).
  *             6. CCM standard expects the authentication TAG to be passed
  *             as part of the ciphertext. In this implementations the tag
  *             should be not be passed to AES_CCM_Decrypt_Append.
  *             Instead a pointer to the TAG must be set in P_pAESCCMctx.pmTag
  *             and this will be checked by AES_CCM_Decrypt_Finish
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: Operation NOT Successful
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values
  */
int32_t AES_CCM_Decrypt_Init(AESCCMctx_stt *P_pAESCCMctx,
                             const uint8_t *P_pKey,
                             const uint8_t *P_pNonce)
{
  int32_t mbedtls_ret_status;
  int32_t aes_ret_status = AES_SUCCESS;

  if ((P_pAESCCMctx == NULL)
      || (P_pKey == NULL)
      || (P_pNonce == NULL) )
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_pAESCCMctx->mKeySize <= 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }
  if ( (P_pAESCCMctx->mNonceSize < 7)  && (P_pAESCCMctx->mNonceSize > 13) )
  {
    return AES_ERR_BAD_CONTEXT;
  }
  if ( (P_pAESCCMctx->mTagSize < 4) || (P_pAESCCMctx->mTagSize > 16) )
  {
    /* Tag has an even size*/
    if ((P_pAESCCMctx->mTagSize % 2) != 0)
    {
      return AES_ERR_BAD_CONTEXT;
    }
  }
  if (P_pAESCCMctx->mAssDataSize < 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }
  if (P_pAESCCMctx->mPayloadSize  < 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /* Initialize MBED crypto library*/
  mbedtls_ccm_init(wrap_aes_ccm_dec.mbedtls_ctx);

  /*Import the raw AES key into a PSA struct*/
  mbedtls_ret_status = mbedtls_ccm_setkey(wrap_aes_ccm_dec.mbedtls_ctx,
                                          MBEDTLS_CIPHER_ID_AES,
                                          P_pKey,
                                          P_pAESCCMctx->mKeySize * 8U);
  if (mbedtls_ret_status == MBEDTLS_SUCCESS)
  {
    /*Save the data*/
    P_pAESCCMctx->pmKey = P_pKey;
    P_pAESCCMctx->pmNonce = P_pNonce;

    /*We are doing an decryption*/
    wrap_aes_ccm_dec.wrap_is_use = WRAP_IS_USE;
    P_pAESCCMctx->mFlags = E_SK_DEFAULT;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}

/**
  * @brief      AES CCM Decryption function
  * @param[in, out]
  *             *P_pAESCCMctx: AES CCM context
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data, expressed in bytes
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Size of written output data, expressed in bytes
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a not valid
  * @retval     AES_ERR_BAD_OPERATION: Append not allowed
  */
int32_t AES_CCM_Decrypt_Append(AESCCMctx_stt *P_pAESCCMctx,
                               const uint8_t *P_pInputBuffer,
                               int32_t        P_inputSize,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  if ((P_pAESCCMctx == NULL)
       || (P_pInputBuffer == NULL)
       || (P_pOutputBuffer == NULL)
       || (P_pOutputSize == NULL) )
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_inputSize < 0)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_pAESCCMctx->mFlags == E_SK_NO_MORE_APPEND_ALLOWED)
  {
    return AES_ERR_BAD_OPERATION;
  }

  *P_pOutputSize = P_inputSize;
  wrap_aes_ccm_dec.p_output_save = P_pOutputBuffer;
  wrap_aes_ccm_dec.input_size_save = P_inputSize;
  wrap_aes_ccm_dec.p_input_save = P_pInputBuffer;

  P_pAESCCMctx->mFlags = E_SK_NO_MORE_APPEND_ALLOWED;

  return AES_SUCCESS;
}

/**
  * @brief      AES CCM Finalization during decryption,
  *             the authentication TAG will be checked
  * @param[in, out]
  *             *P_pAESCCMctx: AES CCM context
  * @param[in]  *P_pOutputBuffer: Not Used
  * @param[in]  *P_pOutputSize: No Used
  * @note       This function requires:
  *             - P_pAESCCMctx->pmTag to be set to a valid pointer
  *             to the tag to be checked
  *             - P_pAESCCMctx->mTagSize to contain a valid value in
  *             the set {4,6,8,10,12,14,16}
  * @retval     AUTHENTICATION_SUCCESSFUL: the TAG is checked
  *             and match the compute one
  * @retval     AES_ERR_BAD_PARAMETER: *P_pAESCCMctx is NULL
  * @retval     AUTHENTICATION_FAILED: the TAG does NOT match the compute one
  */
int32_t AES_CCM_Decrypt_Finish(AESCCMctx_stt *P_pAESCCMctx,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t mbedtls_ret_status;
  int32_t aes_ret_status;

  if (P_pAESCCMctx == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  mbedtls_ret_status = mbedtls_ccm_auth_decrypt(wrap_aes_ccm_dec.mbedtls_ctx,
                                                (uint32_t) wrap_aes_ccm_dec.input_size_save,
                                                P_pAESCCMctx->pmNonce,
                                                (uint32_t) P_pAESCCMctx->mNonceSize,
                                                wrap_aes_ccm_dec.p_wrap_aad,
                                                (uint32_t) P_pAESCCMctx->mAssDataSize,
                                                wrap_aes_ccm_dec.p_input_save,
                                                wrap_aes_ccm_dec.p_output_save,
                                                P_pAESCCMctx->pmTag,
                                                (size_t)P_pAESCCMctx->mTagSize);
  if (mbedtls_ret_status == MBEDTLS_SUCCESS)
  {
    aes_ret_status = AUTHENTICATION_SUCCESSFUL;
  }
  else
  {
    aes_ret_status = AUTHENTICATION_FAILED;
  }
  
  mbedtls_ccm_free(wrap_aes_ccm_dec.mbedtls_ctx);
  wrap_aes_ccm_dec.wrap_is_use = WRAP_IS_NOT_USE;
  if (P_pOutputSize != NULL)
  {
    /*According to documentation as to be set to 0*/
    *P_pOutputSize = 0;
  }
  
  return aes_ret_status;
}
#endif /*WRAP_AES_CCM*/

#ifdef WRAP_AES_CMAC

/**
  * @brief      Inilization for AES-CMAC for Authentication TAG Generation
  * @param[in,out]
  *             *P_pAESCMACctx: AES CMAC context
  * @note       1. P_pAESCMACctx.pmKey (see AESCMACctx_stt) must be set with
  *             a pointer to the AES key before calling this function.
  *             2. P_pAESCMACctx.mKeySize must be set with the size of
  *             the key prior to calling this function.
  *             Otherwise the following predefined values can be used:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  *             3. P_pAESCMACctx.mTagSize must be set with the size of
  *             authentication TAG that will be generated by the
  *             AES_CMAC_Encrypt_Finish.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values
  */
int32_t AES_CMAC_Encrypt_Init(AESCMACctx_stt *P_pAESCMACctx)
{
  int32_t aes_ret_status;
  psa_status_t psa_ret_status;
  wrap_aes_cmac_enc.cmac_op = psa_mac_operation_init();
  psa_algorithm_t psa_algorithm = PSA_ALG_CMAC;

  if (P_pAESCMACctx == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_pAESCMACctx->pmKey == NULL)
      || (P_pAESCMACctx->mKeySize == 0)
      || (P_pAESCMACctx->mTagSize == 0))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /* Initialize MBED crypto library*/
  psa_ret_status = psa_crypto_init();
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Import the raw AES key into a PSA struct*/
    psa_ret_status = wrap_import_raw_aes_key_into_psa(&(wrap_aes_cmac_enc.psa_key_handle),
                                                      PSA_KEY_USAGE_SIGN,
                                                      psa_algorithm,
                                                      P_pAESCMACctx->pmKey,
                                                      (uint32_t) P_pAESCMACctx->mKeySize);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Setup CMAC*/
      psa_ret_status = psa_mac_sign_setup(&(wrap_aes_cmac_enc.cmac_op),
                                          wrap_aes_cmac_enc.psa_key_handle,
                                          psa_algorithm);
      if (psa_ret_status == PSA_SUCCESS)
      {
        aes_ret_status = AES_SUCCESS;
      }
      else
      {
        aes_ret_status = AES_ERR_BAD_PARAMETER;
      }
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }
  return aes_ret_status;
}

/**
  * @brief      AES Encryption in CMAC Mode
  * @param[in,out]
  *             *P_pAESCMACctx AES CMAC, already initialized, context
  * @param[in]  *P_pInputBuffer Input buffer
  * @param[in]  P_inputSize Size of input data in uint8_t (octets)
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_INPUT_SIZE: P_inputSize < 0
  * @retval     AES_ERR_BAD_OPERATION: Append not allowed
  */
int32_t AES_CMAC_Encrypt_Append(AESCMACctx_stt *P_pAESCMACctx,
                                const uint8_t  *P_pInputBuffer,
                                int32_t         P_inputSize)
{
  int32_t aes_ret_status;
  psa_status_t psa_ret_status;
  if ((P_pAESCMACctx == NULL) || (P_pInputBuffer == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_inputSize < 0)
  {
    return AES_ERR_BAD_INPUT_SIZE;
  }

  /*Update CMAC*/
  psa_ret_status = psa_mac_update(&(wrap_aes_cmac_enc.cmac_op),
                                  P_pInputBuffer,
                                  (uint32_t) P_inputSize);
  if (psa_ret_status == PSA_SUCCESS)
  {
    aes_ret_status = AES_SUCCESS;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }

  return aes_ret_status;
}

/**
  * @brief      AES Finalization of CMAC Mode
  * @param[in,out]
  *             *P_pAESCMACctx AES CMAC, already initialized, context
  * @param[out] *P_pOutputBuffer Output buffer
  * @param[out] *P_pOutputSize Size of written output data in uint8_t
  * @note       This function requires P_pAESCMACctx mTagSize to contain valid
  *             value between 1 and 16.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values,
  *             see note
  */
int32_t AES_CMAC_Encrypt_Finish(AESCMACctx_stt *P_pAESCMACctx,
                                uint8_t        *P_pOutputBuffer,
                                int32_t        *P_pOutputSize)
{
  int32_t aes_ret_status;
  psa_status_t psa_ret_status;
  if ((P_pAESCMACctx == NULL) || (P_pOutputBuffer == NULL) || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_pAESCMACctx->mTagSize < 1) || (P_pAESCMACctx->mTagSize > 16))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /*Free the key*/
  psa_ret_status = psa_destroy_key(wrap_aes_cmac_enc.psa_key_handle);
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Finish CMAC*/
    psa_ret_status = psa_mac_sign_finish(&(wrap_aes_cmac_enc.cmac_op),
                                         P_pOutputBuffer,
                                         (uint32_t) P_pAESCMACctx->mTagSize,
                                         (size_t *)P_pOutputSize);
    if (psa_ret_status == PSA_SUCCESS)
    {
      aes_ret_status = AES_SUCCESS;
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_OPERATION;
    }
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}

/**
  * @brief      Initialization for AES-CMAC for Authentication TAG Verification
  * @param[in,out]
  *             *P_pAESCMACctx: AES CMAC context
  * @note       1. P_pAESCMACctx.pmKey (see AESCMACctx_stt) must be set with
  *             a pointer to the AES key before calling this function.
  *             2. P_pAESCMACctx.mKeySize must be set with the size of
  *             the key prior to calling this function.
  *             Otherwise the following predefined values can be used:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  *             3. P_pAESCMACctx.pmTag must be set with a pointer to
  *             the authentication TAG that will be checked during
  *             AES_CMAC_Decrypt_Finish.
  *             4. P_pAESCMACctx.mTagSize must be set with the size of
  *             authentication TAG.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values,
  *                                   see the note below
  */
int32_t AES_CMAC_Decrypt_Init(AESCMACctx_stt *P_pAESCMACctx)
{
  int32_t aes_ret_status;
  psa_status_t psa_ret_status;
  wrap_aes_cmac_dec.cmac_op = psa_mac_operation_init();
  psa_algorithm_t psa_algorithm = PSA_ALG_CMAC;

  if (P_pAESCMACctx == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_pAESCMACctx->pmKey == NULL)
      || (P_pAESCMACctx->pmTag == NULL)
      || (P_pAESCMACctx->mKeySize == 0)
      || (P_pAESCMACctx->mTagSize == 0))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /* Initialize MBED crypto library*/
  psa_ret_status = psa_crypto_init();
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Import the raw AES key into a PSA struct*/
    psa_ret_status = wrap_import_raw_aes_key_into_psa(&(wrap_aes_cmac_dec.psa_key_handle),
                                                      PSA_KEY_USAGE_VERIFY,
                                                      psa_algorithm,
                                                      P_pAESCMACctx->pmKey,
                                                      (uint32_t) P_pAESCMACctx->mKeySize);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Setup CMAC*/
      psa_ret_status = psa_mac_verify_setup(&(wrap_aes_cmac_dec.cmac_op),
                                            wrap_aes_cmac_dec.psa_key_handle,
                                            psa_algorithm);
      if (psa_ret_status == PSA_SUCCESS)
      {
        aes_ret_status = AES_SUCCESS;
      }
      else
      {
        aes_ret_status = AES_ERR_BAD_PARAMETER;
      }
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}

/**
  * @brief      AES-CMAC Data Processing
  * @param[in,out]
  *             *P_pAESCMACctx AES CMAC, already initialized, context
  * @param[in]  *P_pInputBuffer Input buffer
  * @param[in]  P_inputSize Size of input data in uint8_t (octets)
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_INPUT_SIZE: P_inputSize < 0
  * @retval     AES_ERR_BAD_OPERATION: Append not allowed
  */
int32_t AES_CMAC_Decrypt_Append(AESCMACctx_stt *P_pAESCMACctx,
                                const uint8_t  *P_pInputBuffer,
                                int32_t         P_inputSize)
{
  int32_t aes_ret_status;
  psa_status_t psa_ret_status;

  if ((P_pAESCMACctx == NULL) || (P_pInputBuffer == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_inputSize < 0)
  {
    return AES_ERR_BAD_INPUT_SIZE;
  }

  /*Update CMAC*/
  psa_ret_status = psa_mac_update(&(wrap_aes_cmac_dec.cmac_op),
                                  P_pInputBuffer,
                                  (uint32_t) P_inputSize);
  if (psa_ret_status == PSA_SUCCESS)
  {
    aes_ret_status = AES_SUCCESS;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_OPERATION;
  }

  return aes_ret_status;
}

/**
  * @brief      AES Finalization of CMAC Mode
  * @param[in,out]
  *             *P_pAESCMACctx AES CMAC, already initialized, context
  * @param[out] *P_pOutputBuffer Output buffer NOT USED
  * @param[out] *P_pOutputSize Size of written output data in uint8_t
  * @note       This function requires:
  *             P_pAESGCMctx->pmTag to be set to a valid pointer to the tag
  *             to be checked.
  *             P_pAESCMACctx->mTagSize to contain a valid value between 1 and 16.
  * @retval     AUTHENTICATION_SUCCESSFUL if the TAG is verified
  * @retval     AUTHENTICATION_FAILED if the TAG is not verified
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values,
  *             see note
  */
int32_t AES_CMAC_Decrypt_Finish(AESCMACctx_stt *P_pAESCMACctx,
                                uint8_t        *P_pOutputBuffer,
                                int32_t        *P_pOutputSize)
{
  int32_t aes_ret_status = AES_SUCCESS;
  psa_status_t psa_ret_status = AES_SUCCESS;

  if ((P_pAESCMACctx == NULL) || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_pAESCMACctx->mTagSize < 1)
      || (P_pAESCMACctx->mTagSize > 16)
      || (P_pAESCMACctx->pmTag == NULL))
  {
    return AES_ERR_BAD_CONTEXT;
  }

  /*Free the key*/
  psa_ret_status = psa_destroy_key(wrap_aes_cmac_dec.psa_key_handle);
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Finish CMAC*/
    psa_ret_status = psa_mac_verify_finish(&(wrap_aes_cmac_dec.cmac_op),
                                           P_pAESCMACctx->pmTag,
                                           P_pAESCMACctx->mTagSize);
    if (psa_ret_status == PSA_ERROR_INVALID_SIGNATURE)
    {
      aes_ret_status = AUTHENTICATION_FAILED;
    }
    else if (psa_ret_status == PSA_SUCCESS)
    {
      *P_pOutputSize = P_pAESCMACctx->mTagSize;
      aes_ret_status = AUTHENTICATION_SUCCESSFUL;
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}
#endif /*WRAP_AES_CMAC*/

#ifdef WRAP_AES_EBC

/**
  * @brief      Initialization for AES Encryption in ECB Mode
  * @param[in,out]
  *             *P_pAESECBctx: AES ECB context
  * @param[in]  *P_pKey: Buffer with the Key
  * @param[in]  *P_pIv: Not used since no IV is required in ECB
  * @note       1. P_pAESECBctx.mKeySize (see AESECBctx_stt) must be set with
  *             the size of the key prior to calling this function.
  *             Instead of the size of the key, you can also use the following
  *             predefined values:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values.
  *                                  See note
  */
int32_t AES_ECB_Encrypt_Init(AESECBctx_stt *P_pAESECBctx,
                             const uint8_t *P_pKey,
                             const uint8_t *P_pIv)
{
  int32_t aes_ret_status;
  int32_t mbedtls_ret_status;

  if ((P_pAESECBctx == NULL) || (P_pKey == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_pAESECBctx->mKeySize == 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }

  mbedtls_aes_init(wrap_aes_ecb_enc.mbedtls_ctx);
  mbedtls_ret_status = mbedtls_aes_setkey_enc(wrap_aes_ecb_enc.mbedtls_ctx,
                                              P_pKey,
                                              P_pAESECBctx->mKeySize * 8U);
  if (mbedtls_ret_status == MBEDTLS_SUCCESS)
  {
    aes_ret_status = AES_SUCCESS;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}

/**
  * @brief      AES Encryption in ECB Mode
  * @param[in]  *P_pAESECBctx: AES ECB, already initialized, context. Not used.
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data, expressed in bytes
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Pointer to integer containing size of written
  *             output data, expressed in bytes
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer.
  * @retval     AES_ERR_BAD_INPUT_SIZE:Size of input is not a multiple
  *                                    of CRL_AES_BLOCK
  * @retval     AES_ERR_BAD_OPERATION: Append not allowed.
  */
int32_t AES_ECB_Encrypt_Append(AESECBctx_stt *P_pAESECBctx,
                               const uint8_t *P_pInputBuffer,
                               int32_t        P_inputSize,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t aes_ret_status = AES_SUCCESS;
  int32_t mbedtls_ret_status;
  uint32_t wrap_computed;

  if ((P_pOutputBuffer == NULL) || (P_pInputBuffer == NULL) || (P_pOutputSize == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if ((P_inputSize < 16) || ((P_inputSize % 16) != 0))
  {
    return AES_ERR_BAD_INPUT_SIZE;
  }

  for (wrap_computed = 0U; wrap_computed < P_inputSize; wrap_computed += 16U)
  {
    mbedtls_ret_status = mbedtls_aes_crypt_ecb(wrap_aes_ecb_enc.mbedtls_ctx,
                                               MBEDTLS_AES_ENCRYPT,
                                               &P_pInputBuffer[wrap_computed],
                                               &P_pOutputBuffer[wrap_computed]);
    if (mbedtls_ret_status == MBEDTLS_SUCCESS)
    {
      aes_ret_status = AES_SUCCESS;
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_OPERATION;
    }
  }
  *P_pOutputSize = P_inputSize;
  return aes_ret_status;
}

/**
  * @brief      AES Finalization of ECB mode
  * @param[in,out]
  *             *P_pAESECBctx: AES ECB, already initialized, context. Not used.
  * @param[out] *P_pOutputBuffer: Output buffer. Not used.
  * @param[out] *P_pOutputSize: Pointer to integer containing size of written
  *             output data, in bytes. Not used.
  * @retval     AES_SUCCESS: Operation Successful
  */
int32_t AES_ECB_Encrypt_Finish(AESECBctx_stt *P_pAESECBctx,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  /*According to documentation does not output any data*/
  mbedtls_aes_free(wrap_aes_ecb_enc.mbedtls_ctx);
  return AES_SUCCESS;
}

/**
  * @brief      Initialization for AES Decryption in ECB Mode
  * @param[in,out]
  *             *P_pAESECBctx: AES ECB context
  * @param[in]  *P_pKey: Buffer with the Key
  * @param[in]  *P_pIv: Not used since no IV is required in ECB
  * @note       1. P_pAESECBctx.mKeySize (see AESECBctx_stt) must be set with
  *             the size of the key prior to calling this function.
  *             Instead of the size of the key, you can also use the following
  *             predefined values:
  *             - CRL_AES128_KEY
  *             - CRL_AES192_KEY
  *             - CRL_AES256_KEY
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer
  * @retval     AES_ERR_BAD_CONTEXT: Context not initialized with valid values.
  *                                  See note
  */
int32_t AES_ECB_Decrypt_Init(AESECBctx_stt *P_pAESECBctx,
                             const uint8_t *P_pKey,
                             const uint8_t *P_pIv)
{
  int32_t aes_ret_status;
  int32_t mbedtls_ret_status;

  if ((P_pAESECBctx == NULL) || (P_pKey == NULL))
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_pAESECBctx->mKeySize == 0)
  {
    return AES_ERR_BAD_CONTEXT;
  }

  mbedtls_aes_init(wrap_aes_ecb_dec.mbedtls_ctx);
  mbedtls_ret_status = mbedtls_aes_setkey_dec(wrap_aes_ecb_dec.mbedtls_ctx,
                                              P_pKey,
                                              P_pAESECBctx->mKeySize * 8U);
  if (mbedtls_ret_status == MBEDTLS_SUCCESS)
  {
    aes_ret_status = AES_SUCCESS;
  }
  else
  {
    aes_ret_status = AES_ERR_BAD_PARAMETER;
  }

  return aes_ret_status;
}

/**
  * @brief      AES Decryption in ECB Mode
  * @param[in]  *P_pAESECBctx: AES ECB context. Not used.
  * @param[in]  *P_pInputBuffer: Input buffer
  * @param[in]  P_inputSize: Size of input data in bytes
  * @param[out] *P_pOutputBuffer: Output buffer
  * @param[out] *P_pOutputSize: Size of written output data in bytes.
  * @retval     AES_SUCCESS: Operation Successful
  * @retval     AES_ERR_BAD_PARAMETER: At least one parameter is a NULL pointer.
  * @retval     AES_ERR_BAD_INPUT_SIZE: P_inputSize is not a multiple of CRL_AES_BLOCK
  * @retval     AES_ERR_BAD_OPERATION: Append not allowed.
  */
int32_t AES_ECB_Decrypt_Append(AESECBctx_stt *P_pAESECBctx,
                               const uint8_t *P_pInputBuffer,
                               int32_t        P_inputSize,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  int32_t aes_ret_status = AES_SUCCESS;
  uint8_t mbedtls_ret_status;
  uint32_t wrap_computed;

  if ((P_inputSize < 16) || ((P_inputSize % 16) != 0))
  {
    return AES_ERR_BAD_INPUT_SIZE;
  }

  for (wrap_computed = 0U; wrap_computed < P_inputSize; wrap_computed += 16U)
  {
    mbedtls_ret_status = mbedtls_aes_crypt_ecb(wrap_aes_ecb_dec.mbedtls_ctx,
                                               MBEDTLS_AES_DECRYPT,
                                               &P_pInputBuffer[wrap_computed],
                                               &P_pOutputBuffer[wrap_computed]);
    if (mbedtls_ret_status == MBEDTLS_SUCCESS)
    {
      aes_ret_status = AES_SUCCESS;
    }
    else
    {
      aes_ret_status = AES_ERR_BAD_OPERATION;
    }
  }

  *P_pOutputSize = P_inputSize;
  return aes_ret_status;
}

/**
  * @brief      AES Decryption Finalization of ECB mode
  * @param[in,out]
  *             *P_pAESECBctx: AES ECB context. Not used.
  * @param[out] *P_pOutputBuffer: Output buffer. Not used.
  * @param[out] *P_pOutputSize: Pointer to integer containing size of written
  *             output data, in bytes. Not used.
  * @retval     AES_SUCCESS: Operation Successful
  */
int32_t AES_ECB_Decrypt_Finish(AESECBctx_stt *P_pAESECBctx,
                               uint8_t       *P_pOutputBuffer,
                               int32_t       *P_pOutputSize)
{
  /*According to documentation does not output any data*/
  mbedtls_aes_free(wrap_aes_ecb_dec.mbedtls_ctx);
  return AES_SUCCESS;
}
#endif /*WRAP_AES_EBC*/

#endif /*WRAP_AES*/
/**
  * @}
  */

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
