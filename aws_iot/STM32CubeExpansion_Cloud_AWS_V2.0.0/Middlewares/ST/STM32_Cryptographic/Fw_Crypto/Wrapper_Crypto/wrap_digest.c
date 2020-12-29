/**
  ******************************************************************************
  * @file    wrap_digest.c
  * @author  MCD Application Team
  * @brief   Provides wrapper functions for hash methods to abstract call to MBED
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
#include "wrap_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#ifdef WRAP_HASH

#ifdef WRAP_SHA_256
static psa_hash_operation_t hash_op_256 = PSA_HASH_OPERATION_INIT;
#endif /*WRAP_SHA_256*/

#ifdef WRAP_SHA_1
static psa_hash_operation_t hash_op_1 = PSA_HASH_OPERATION_INIT;
#endif /*WRAP_SHA_1*/

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
#ifdef WRAP_SHA_1

/**
  * @brief      Initialize a new SHA1 context
  * @param[in, out]
  *             *P_pSHA1ctx: The context that will be initialized. Not used
  * @retval     HASH_SUCCESS: Operation Successful
  * @retval     HASH_ERR_BAD_PARAMETER
  */
int32_t SHA1_Init(SHA1ctx_stt *P_pSHA1ctx)
{
  int32_t hash_ret_status;
  psa_status_t psa_ret_status;

  /*Init mbed*/
  psa_ret_status = psa_crypto_init();
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Init the hash operation*/
    hash_op_1 = psa_hash_operation_init();
    psa_ret_status = psa_hash_setup(&hash_op_1, PSA_ALG_SHA_1);
    if (psa_ret_status == PSA_SUCCESS)
    {
      hash_ret_status = HASH_SUCCESS;
    }
    else
    {
      hash_ret_status = HASH_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    hash_ret_status = HASH_ERR_BAD_PARAMETER;
  }
  return hash_ret_status;
}

/**
  * @brief      Process input data and update a SHA1ctx_stt 
  * @param[in, out]
    *           *P_pSHA1ctx: SHA1 context that will be updated. Not used
  * @param[in]  *P_pInputBuffer: The data that will be processed using SHA1
  * @param[in]  P_inputSize: Size of input data expressed in bytes
  * @note       This function can be called multiple times with no restrictions 
  *             on the value of P_inputSize
  * @retval     HASH_SUCCESS: Operation Successful
  * @retval     HASH_ERR_BAD_PARAMETER: P_pInputBuffer == NULL
  * @retval     HASH_ERR_BAD_OPERATION
  */
int32_t SHA1_Append(SHA1ctx_stt *P_pSHA1ctx,
                    const uint8_t *P_pInputBuffer,
                    int32_t P_inputSize)
{
  int32_t hash_ret_status;
  psa_status_t psa_ret_status;
  if (P_pInputBuffer == NULL)
  {
    return HASH_ERR_BAD_PARAMETER;
  }

  psa_ret_status = psa_hash_update(&hash_op_1,
                                   P_pInputBuffer,
                                   (uint32_t) P_inputSize);
  if (psa_ret_status == PSA_SUCCESS)
  {
    hash_ret_status = HASH_SUCCESS;
  }
  else
  {
    hash_ret_status = HASH_ERR_BAD_OPERATION;
  }

  return hash_ret_status;
}

/**
  * @brief      
  * @param[in,out]
  *             *P_pSHA1ctx: HASH contex
  * @param[in]  *P_pOutputBuffer: Buffer that will contain the digest
  * @param[in]  *P_pOutputSize: Size of the data written to P_pOutputBuffer
  * @retval     HASH_SUCCESS: Operation Successful
  * @retval     HASH_ERR_BAD_PARAMETER: P_pOutputBuffer == NULL || P_pOutputSize == NULL
  * @retval     HASH_ERR_BAD_CONTEXT
  */
int32_t SHA1_Finish(SHA1ctx_stt *P_pSHA1ctx,
                    uint8_t *P_pOutputBuffer,
                    int32_t *P_pOutputSize)
{
  int32_t hash_ret_status;
  psa_status_t psa_ret_status;
  size_t hash_length = PSA_HASH_SIZE(hash_op_1.alg);
  if ((P_pOutputBuffer == NULL) || (P_pOutputSize == NULL))
  {
    return HASH_ERR_BAD_PARAMETER;
  }

  psa_ret_status = psa_hash_finish(&hash_op_1, P_pOutputBuffer, hash_length, (size_t *)P_pOutputSize);
  if (psa_ret_status == PSA_SUCCESS)
  {
    hash_ret_status = HASH_SUCCESS;
  }
  else
  {
    hash_ret_status = HASH_ERR_BAD_CONTEXT;
  }

  return hash_ret_status;
}
#endif /*WRAP_SHA_1*/

#ifdef WRAP_SHA_256

/**
  * @brief      Initialize a new SHA256 context
  * @param[in, out]
  *             *P_pSHA256ctx: The context that will be initialized. Not used
  * @retval     HASH_SUCCESS: Operation Successful
  * @retval     HASH_ERR_BAD_PARAMETER
  */
int32_t SHA256_Init(SHA256ctx_stt *P_pSHA256ctx)
{
  int32_t hash_ret_status;
  psa_status_t psa_ret_status;

  /* Initialize MBED crypto library*/
  psa_ret_status = psa_crypto_init();
  if (psa_ret_status == PSA_SUCCESS) 
  {
    /* Initialize hash operation*/
    hash_op_256 = psa_hash_operation_init();
    psa_ret_status = psa_hash_setup(&hash_op_256, PSA_ALG_SHA_256);
    if (psa_ret_status == PSA_SUCCESS)
    {
      hash_ret_status = HASH_SUCCESS;
    }
    else
    {
      hash_ret_status = HASH_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    hash_ret_status = HASH_ERR_BAD_PARAMETER;
  }
 
  return hash_ret_status;
}

/**
  * @brief      Process input data and update a SHA256ctx_stt 
  * @param[in, out]
    *           *P_pSHA256ctx: SHA256 context that will be updated. Not used
  * @param[in]  *P_pInputBuffer: The data that will be processed using SHA256
  * @param[in]  P_inputSize: Size of input data expressed in bytes
  * @note       This function can be called multiple times with no restrictions 
  *             on the value of P_inputSize
  * @retval     HASH_SUCCESS: Operation Successful
  * @retval     HASH_ERR_BAD_PARAMETER: P_pInputBuffer == NULL
  * @retval     HASH_ERR_BAD_OPERATION
  */
int32_t SHA256_Append(SHA256ctx_stt *P_pSHA256ctx,
                        const uint8_t *P_pInputBuffer,
                        int32_t P_inputSize)
{
  int32_t hash_ret_status;
  psa_status_t psa_ret_status;

   if (P_pInputBuffer == NULL)
  {
    return HASH_ERR_BAD_PARAMETER;
  }

  psa_ret_status = psa_hash_update(&hash_op_256,
                                   P_pInputBuffer,
                                   (uint32_t) P_inputSize);
  if (psa_ret_status == PSA_SUCCESS)
  {
    hash_ret_status = HASH_SUCCESS;
  }
  else
  {
    hash_ret_status = HASH_ERR_BAD_OPERATION;
  }

  return hash_ret_status;
}

/**
  * @brief      
  * @param[in,out]
  *             *P_pSHA256ctx: HASH contex
  * @param[in]  *P_pOutputBuffer: Buffer that will contain the digest
  * @param[in]  *P_pOutputSize: Size of the data written to P_pOutputBuffer
  * @retval     HASH_SUCCESS: Operation Successful
  * @retval     HASH_ERR_BAD_PARAMETER: P_pOutputBuffer == NULL || P_pOutputSize == NULL
  * @retval     HASH_ERR_BAD_CONTEXT
  */
int32_t SHA256_Finish(SHA256ctx_stt *P_pSHA256ctx,
                      uint8_t *P_pOutputBuffer,
                      int32_t *P_pOutputSize)
{
  int32_t hash_ret_status;
  psa_status_t psa_ret_status;
  size_t hash_length = PSA_HASH_SIZE(hash_op_256.alg);

   if ((P_pOutputBuffer == NULL) || (P_pOutputSize == NULL))
  {
    return HASH_ERR_BAD_PARAMETER;
  }

  psa_ret_status = psa_hash_finish(&hash_op_256, P_pOutputBuffer, hash_length, (size_t *)P_pOutputSize);
  if (psa_ret_status == PSA_SUCCESS)
  {
    hash_ret_status = HASH_SUCCESS;
  }
  else
  {
    hash_ret_status = HASH_ERR_BAD_CONTEXT;
  }

  return hash_ret_status;
}
#endif /*WRAP_SHA_256*/

#endif /*WRAP_HASH*/
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
