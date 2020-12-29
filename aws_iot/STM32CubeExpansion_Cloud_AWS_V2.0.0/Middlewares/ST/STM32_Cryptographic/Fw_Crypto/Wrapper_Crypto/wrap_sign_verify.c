/**
  ******************************************************************************
  * @file    wrap_sign_verify.c
  * @author  MCD Application Team
  * @brief   Provides wrapper functions for ECC methods to abstract call to MBED
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
#include <string.h>        /* To add memcpy*/
#include "wrap_config.h"
#include "bignum.h"
#include "pk.h"
#include "pk_internal.h"

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#ifdef WRAP_ECDSA
/*Only P-192, P-256, p-384 supported for the moment*/
/**
  * @brief ECDSA's Macros
  */
#define DIGIT_MAXSIZE         (48U)                     /*!< Maximum number of Digits*/
#define ECDSA_PUBKEY_MAXSIZE  (2 * DIGIT_MAXSIZE + 1)   /*!< Maximum Size of ECDSA's Public key */
#define ECDSA_PRIVKEY_MAXSIZE 48                        /*!< Maximum Size of ECDSA's Private key in bytes*/
#define ECDSA_SIGN_MAXSIZE    48*2                      /*!< Maximum Size of Buffer containing R and S*/
#endif /*WRAP_ECDSA*/

#ifdef WRAP_RSA
/**
  * @brief RSA's Macros
  */
#define WRAP_SHA1_SIZE      20          /*!< Size of a Sha1 digest*/
#define WRAP_SHA256_SIZE    32          /*!< Size of a Sha256 digest*/
#define RSA_PUBKEY_MAXSIZE  528         /*!< Maximum size of RSA's public key in bits*/
#define RSA_PRIVKEY_MAXSIZE 1320        /*!< Maximum size of RSA's private key in bits*/
#endif /*WRAP_RSA*/

/**
  * @brief      Used for creating the DER type
  */
#define DER_NB_PUB_TYPE 3               /*!< Number of type in a DER public key*/
#define DER_NB_PUB_SIZE 3               /*!< Number of size in a DER public key*/

/*Return type for functions*/
/**
  * @brief WRAPPER's Macro
  */
#define WRAP_SUCCESS 1U                 /*!< Return's Type on Succes*/
#define WRAP_FAILURE 0U                 /*!< Return's Type on Failure*/
#define WRAP_BAD_KEY 2U                 /*!< Return's Type with a Bad key*/

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef WRAP_ECDSA
/*Static variables to not use dynamique allocation*/
static BigNum_stt static_ECC_bigNum_privKey;
static BigNum_stt static_ECDSA_R;
static BigNum_stt static_ECDSA_S;
static ECDSAsignature_stt static_ECC_Sign;
static ECCprivKey_stt static_ECC_privKey;
static uint8_t static_ECDSA_privKey[ECDSA_PRIVKEY_MAXSIZE];
static uint8_t static_ECDSA_pubKey[ECDSA_PUBKEY_MAXSIZE];
static uint8_t static_ECDSA_Sign[ECDSA_SIGN_MAXSIZE];
#endif /*WRAP_ECDSA*/

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#ifdef WRAP_ECDSA
/**
  * @brief      Import a ECC public key to a psa struct
  * @param[in,out]
                *P_Key_Handle: Handle of the key
  * @param[in]  P_Psa_Usage: Key usage
  * @param[in]  P_Psa_Algorithm: Algorithm that will be used with the key
  * @param[in]  ecc_curve: The ECC curve
  * @param[in]  *P_pEcc_pubKey: The key
  * @param[in]  P_KeySize: The size of the key in bytes
  * @note       The only supported ecc curves for now are PSA_ECC_CURVE_SECP192R1,
  *             PSA_ECC_CURVE_SECP256R1 and PSA_ECC_CURVE_SECP384R1
  * @retval     AES_ERR_BAD_PARAMETER: Could not import the key
  * @retval     PSA_SUCCESS: Operation Successful
  */
static psa_status_t wrap_import_ecc_pubKey_into_psa(psa_key_handle_t *P_Key_Handle,
                                                    psa_key_usage_t P_Psa_Usage,
                                                    psa_algorithm_t  P_Psa_Algorithm,
                                                    psa_ecc_curve_t ecc_curve,
                                                    const uint8_t *P_pEcc_pubKey,
                                                    uint32_t P_KeySize)
{
  psa_status_t psa_ret_status;
  psa_key_policy_t key_policy = {0};

  if (P_Key_Handle == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_pEcc_pubKey == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  /* Import key into psa struct*/
  /*Allocate a Key*/
  psa_ret_status = psa_allocate_key(P_Key_Handle);
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Set algorithm*/
    psa_key_policy_set_usage(&key_policy, P_Psa_Usage, P_Psa_Algorithm);
    psa_ret_status = psa_set_key_policy(*P_Key_Handle, &key_policy);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Import key*/
      psa_ret_status = psa_import_key(*P_Key_Handle,
                                      PSA_KEY_TYPE_ECC_PUBLIC_KEY(ecc_curve),
                                      P_pEcc_pubKey,
                                      P_KeySize);
      if (psa_ret_status != PSA_SUCCESS)
      {
        psa_ret_status = ECC_ERR_BAD_OPERATION;
      }
    }
    else
    {
      psa_ret_status = ECC_ERR_BAD_CONTEXT;
    }
  }
  else
  {
    psa_ret_status = ECC_ERR_BAD_CONTEXT;
  }
  return psa_ret_status;
}

/**
  * @brief      Import a ECC private key to a psa struct
  * @param[in,out]
                *P_Key_Handle: Handle of the key
  * @param[in]  P_Psa_Usage: Key usage
  * @param[in]  P_Psa_Algorithm: Algorithm that will be used with the key
  * @param[in]  ecc_curve: The ECC curve
  * @param[in]  *P_pEcc_privKey: The key
  * @param[in]  P_KeySize: The size of the key in bytes
  * @note       The only supported ecc curves for now are PSA_ECC_CURVE_SECP192R1,
  *             PSA_ECC_CURVE_SECP256R1 and PSA_ECC_CURVE_SECP384R1
  * @retval     AES_ERR_BAD_PARAMETER: Could not import the key
  * @retval     PSA_SUCCESS: Operation Successful
  */
static psa_status_t wrap_import_ecc_privKey_into_psa(psa_key_handle_t *P_Key_Handle,
                                                     psa_key_usage_t P_Psa_Usage,
                                                     psa_algorithm_t  P_Psa_Algorithm,
                                                     psa_ecc_curve_t ecc_curve,
                                                     const uint8_t *P_pEcc_privKey,
                                                     uint32_t P_KeySize)
{
  psa_status_t psa_ret_status;
  psa_key_policy_t key_policy = {0};

  if (P_Key_Handle == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }
  if (P_pEcc_privKey == NULL)
  {
    return AES_ERR_BAD_PARAMETER;
  }

  /* Import key into psa struct*/
  /*Allocate the key*/
  psa_ret_status = psa_allocate_key(P_Key_Handle);
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Set algorithm*/
    psa_key_policy_set_usage(&key_policy, P_Psa_Usage, P_Psa_Algorithm);
    psa_ret_status = psa_set_key_policy(*P_Key_Handle, &key_policy);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Import key*/
      psa_ret_status = psa_import_key(*P_Key_Handle,
                                      PSA_KEY_TYPE_ECC_KEYPAIR(ecc_curve),
                                      P_pEcc_privKey,
                                      P_KeySize);
      if (psa_ret_status != PSA_SUCCESS)
      {
        psa_ret_status = ECC_ERR_BAD_OPERATION;
      }
    }
    else
    {
      psa_ret_status = ECC_ERR_BAD_CONTEXT;
    }
  }
  else
  {
    psa_ret_status = ECC_ERR_BAD_CONTEXT;
  }
  return psa_ret_status;
}


#endif /*WRAP_ECDSA*/

/**
  * @brief      Function to pass an uint8_t type array to a Bignum type
  * @param[out] *P_pBigNum: Output data
  * @param[in]  *P_pArray: Data that will be transform
  * @param[in]  Psize: Size of the Array in bytes
  * @retval     WRAP_SUCCESS: On success
  * @reval      WRAP_FAILURE: An error occur
  */
static uint8_t wrap_uint8_to_BigNum(BigNum_stt *P_pBigNum, const uint8_t *P_pArray, int32_t Psize)
{
  uint8_t i;
  uint8_t j;
  uint32_t size_tp;
  int32_t size_counter = 0;

  if ((P_pBigNum == NULL) 
      || (P_pArray == NULL) )
  {
    return WRAP_FAILURE;
  }
  if (Psize <= 0)
  {
    return WRAP_FAILURE;
  }
  /*Allocate a multiple of 4*/
  /*Dynamic allocation, because we don't know the number of calls to this function,
  and we need a buffer*/
  if ( (Psize % 4U) > 0)
  {
    P_pBigNum->pmDigit = malloc((uint32_t)(Psize + 4 - (Psize % 4)) ); /*uin32_t*/
    size_tp = (uint32_t)((Psize / 4) + 1);
  }
  else
  {
    P_pBigNum->pmDigit = malloc(Psize); /*uin32_t*/
    size_tp = (uint32_t)(Psize / 4);
  }

  if (P_pBigNum->pmDigit == NULL)
  {
    return WRAP_FAILURE;
  }
  else
  {
    /*Set to zero*/
    (void)memset(P_pBigNum->pmDigit, 0, size_tp * 4U);
    /*Fill the array*/
    for (i = 0U; i < size_tp; i++)
    {
      for (j = 0U; j < 4U; j++)
      {
        size_counter ++;
        if (size_counter <= Psize)
        {
          P_pBigNum->pmDigit[size_tp - i - 1U] += P_pArray[ (4U * i) + j] << (8U * (3U - j));
        }
        else
        {
          /*Padding*/
          P_pBigNum->pmDigit[size_tp - i - 1U] = P_pBigNum->pmDigit[size_tp - i - 1U] >> 8U;
        }

      }
    }
    /*Fill the rest of the structure*/
    P_pBigNum->mNumDigits = (uint16_t) size_tp;
    P_pBigNum->mSize = (uint8_t) size_tp;
    P_pBigNum->mSignFlag = SIGN_POSITIVE;
  }
  return WRAP_SUCCESS;
}

/**
  * @brief      Function to pass an BigNum type to a uint8_t type array
  * @param[out]  *P_pArray: Output data
  * @param[in] *P_pBigNum: Data that will be transform
  * @param[out]  *Psize: Size of the Array in bytes
  * @retval     WRAP_SUCCESS: On success
  * @reval      WRAP_FAILURE: An error occure
  */
static uint8_t wrap_BigNum_to_uint8(uint8_t *P_pArray,
                                    const BigNum_stt *P_pBigNum,
                                    int32_t *P_psize)
{
  uint8_t i;
  uint8_t j;
  if ((P_pBigNum == NULL) 
      || (P_pArray == NULL) 
      || (P_psize == NULL) )
  {
    return WRAP_FAILURE;
  }

  /*uint32 -> uint8*/
  for (i = 0U; i < P_pBigNum->mSize; i++)
  {
    for (j = 4U; j > 0U; j--)
    {
      P_pArray[(P_pBigNum->mSize * 4U) - (i * 4U) - j] = P_pBigNum->pmDigit[i] >> ((j - 1U) * 8U);
    }
  }
  *P_psize = P_pBigNum->mSize * 4U; /*uint8 -> uint32*/

  return WRAP_SUCCESS;
}
/**
  * @brief Transform a BigNum into a mbedtls_mpi
  * @param[out] P_pmpi: The mpi equivalent of the BigNum
  * @param[in]  P_pbignum: A BigNum that will be transform
  * @retval WRAP_SUCCESS: on Success
  * @retval WRAP_FAILURE: if an input is NULL
  */
static uint8_t bignum_to_mpi(mbedtls_mpi *P_pMpi, BigNum_stt *P_pBigNum)
{
  if ((P_pMpi == NULL) 
      || (P_pBigNum == NULL) )
  {
    return WRAP_FAILURE;
  }
  /*mSignFlag is 0 for positive or 1 for negative,
  and s is 1 for positive and -1 for negative */
  P_pMpi->s = (P_pBigNum->mSignFlag == 0) ? 1 : -1;
  P_pMpi->n = P_pBigNum->mSize;
  P_pMpi->p = P_pBigNum->pmDigit;

  return WRAP_SUCCESS;
}

/**
  * @brief Transform a uint8_t into a mbedtls_mpi
  * @param[out] P_pmpi: The mpi equivalent of the Array
  * @param[in]  P_pbignum: A array that will be transform
  * @retval WRAP_SUCCESS: on Success
  * @retval WRAP_FAILURE: otherwise
  */
static uint8_t uint8_t_to_mpi(mbedtls_mpi *P_pMpi, const uint8_t *P_pArray, int32_t Psize)
{
  uint8_t wrap_ret_status;
  BigNum_stt BigNum;

  if ((P_pMpi == NULL)
      || (P_pArray == NULL) )
  {
    return WRAP_FAILURE;
  }
  /*We use other functions to reduce code size and help maintenance*/
  /*uint8_t -> BigNum*/
  wrap_ret_status = wrap_uint8_to_BigNum(&BigNum, P_pArray, Psize);
  if (wrap_ret_status == WRAP_SUCCESS)
  {
    /*BigNum -> mpi*/
    wrap_ret_status = bignum_to_mpi(P_pMpi, &BigNum);
    if (wrap_ret_status == WRAP_SUCCESS)
    {
      wrap_ret_status = WRAP_SUCCESS;
    }
    else
    {
      wrap_ret_status = WRAP_FAILURE;
    }
  }
  else
  {
    wrap_ret_status = WRAP_FAILURE;
  }

  return wrap_ret_status;
}


#ifdef WRAP_RSA
/**
  * @brief      Change the key's format from a PUBLIC rsa key to a der one.
  * @param[out] *P_pDer: Der key
  * @param[in]  *P_pRsa_Modulus: Modulus
  * @param[in]  *P_pRsa_PubExponent: Public Exponent
  * @param[out] *P_pDer_size: the size of the Der key
  * @param[in]  P_Rsa_Modulus_Size: Size of the modulus
  * @param[in]  P_Rsa_PubExponent_Size: Size fo the public exponent
  * @retval     WRAP_FAILURE: Operation not possible
  * @retval     WRAP_SUCCESS: Operation Successful
  */
static uint8_t wrap_pubkey_rsa_to_der(uint8_t  *P_pDer,
                                      uint8_t  *P_pRsa_Modulus,
                                      uint8_t  *P_pRsa_PubExponent,
                                      uint32_t  *P_pDer_size,
                                      int32_t  P_Rsa_Modulus_Size,
                                      int32_t  P_Rsa_PubExponent_Size)
{
  uint32_t wrap_pos_buffer;
  uint32_t wrap_out_size;
  int32_t temp_modulus_size;
  int32_t temp_pubexponent_size;
  uint32_t temp_output_size;
  uint8_t wrap_bytes_modulus = 0;
  uint8_t wrap_bytes_exponent = 0;
  uint8_t wrap_bytes_out = 0;
  uint8_t i;

  if ((P_pDer == NULL)
      || (P_pRsa_Modulus == NULL)
      || (P_pRsa_PubExponent == NULL) )
  {
    return WRAP_FAILURE;
  }
  if ((P_Rsa_Modulus_Size < 0) || (P_Rsa_PubExponent_Size < 0) )
  {
    return WRAP_FAILURE;
  }

  temp_modulus_size = P_Rsa_Modulus_Size;
  temp_pubexponent_size = P_Rsa_PubExponent_Size;
  /*What is the size of the modulus in bytes ?*/
  while (temp_modulus_size > 0)
  {
    temp_modulus_size = temp_modulus_size >> 8U;
    wrap_bytes_modulus++;
  };
  /* What is the size of the exponent in bytes ?*/
  while (temp_pubexponent_size > 0)
  {
    temp_pubexponent_size = temp_pubexponent_size >> 8U;
    wrap_bytes_exponent++;
  };

  /*Format RSA to public "DER":
   0x30   0x8x size  0x02 0x8x size     Modulus   0x02    0x8x size     Exponent
   Type |           |Type|   Length     | Value  |Type|    Length     | Value   */

  /*Because we don't know the output size yet,
   we have to use the max bytes size (i.e. 4),
   and after sustract it and add the real one*/
  wrap_out_size = (uint32_t) (P_Rsa_Modulus_Size + P_Rsa_PubExponent_Size + DER_NB_PUB_TYPE \
                  + DER_NB_PUB_SIZE + (int32_t) wrap_bytes_modulus + (int32_t) wrap_bytes_exponent + 4);
  if (wrap_out_size > (uint32_t)RSA_PUBKEY_MAXSIZE)
  {
    return WRAP_FAILURE;
  }

  temp_output_size = wrap_out_size;
  /* What is the size of the output in bytes ?*/
  while (temp_output_size > 0U)
  {
    temp_output_size = temp_output_size >> 8U;
    wrap_bytes_out++;
  };
  wrap_out_size = wrap_out_size - 4U + wrap_bytes_out;

  /*It's a DER specific to MBED*/
  P_pDer[0] = 0x30;
  P_pDer[1] = wrap_bytes_out + (uint8_t)0x80; /*Number of bytes of total der key*/
  for (i = wrap_bytes_out; i > 0; i--)
  {
    /*The size of what is left of the output*/
    P_pDer[1U + i] = (wrap_out_size - 2U - wrap_bytes_out) >> (8 * (wrap_bytes_out - i));
  }

  P_pDer[2U + wrap_bytes_out] = 0x02; /*INTEGER*/
  P_pDer[3U + wrap_bytes_out] = wrap_bytes_modulus + (uint8_t)0x80; /*Number of bytes of datas*/
  for (i = wrap_bytes_modulus; i > 0U; i--)
  {
    /*Modulus Size*/
    P_pDer[3U + wrap_bytes_out + i] = P_Rsa_Modulus_Size >> (8U * (wrap_bytes_modulus - i));
  }
  /*Write the modulus into the buffer*/
  (void)memcpy(&P_pDer[4U + wrap_bytes_out + wrap_bytes_modulus],
               P_pRsa_Modulus,
               (uint32_t) P_Rsa_Modulus_Size);

  /*Save the position */
  wrap_pos_buffer = 4U + wrap_bytes_out + wrap_bytes_modulus + P_Rsa_Modulus_Size;
  P_pDer[wrap_pos_buffer] = 0x02; /*INTEGER*/
  P_pDer[1U + wrap_pos_buffer] = wrap_bytes_exponent + (uint8_t)0x80; /*Number of bytes of datas*/
  for (i = wrap_bytes_exponent; i > 0U; i--)
  {
    /*Exponent Size*/
    P_pDer[1U + wrap_pos_buffer + i] = P_Rsa_PubExponent_Size >>( 8 * (wrap_bytes_exponent - i));
  }
  /*Write the public exponent into the buffer*/
  (void)memcpy(&P_pDer[2U + wrap_pos_buffer + wrap_bytes_exponent],
               P_pRsa_PubExponent,
               (uint32_t) P_Rsa_PubExponent_Size);

  *P_pDer_size = wrap_out_size;
  return WRAP_SUCCESS;
}

/**
  * @brief      Change the key's format from a rsa KEYPAIR to a der one.
  * @param[out] *P_pDer: Der key
  * @param[in]  *P_pRsa_Modulus: Modulus
  * @param[in]  *P_pRsa_PrivExponent: Private Exponent
  * @param[in]  *P_pRsa_PubExponent: Public Exponent
  * @param[out] *P_pDer_size: the size of the Der key
  * @param[in]  P_Rsa_Modulus_Size: Size of the modulus
  * @param[in]  P_Rsa_PrivExponent_Size: Size fo the private exponent
  * @param[in]  P_Rsa_PubExponent_Size: Size fo the public exponent
  * @param[out] *P_plength: Size of the output buffer
  * note        The output buffer write data starting at the end
  * @retval     WRAP_FAILURE: Operation not possible
  * @retval     WRAP_SUCCESS: Operation Successful
  * @retaval    WRAP_BAD_KEY: Bad key
  */
static uint8_t wrap_keypair_rsa_to_der(uint8_t  *P_pDer,
                                       uint8_t  *P_pRsa_Modulus,
                                       uint8_t  *P_pRsa_PrivExponent,
                                       uint8_t  *P_pRsa_PubExponent,
                                       uint32_t P_pDer_size,
                                       int32_t  P_Rsa_Modulus_Size,
                                       int32_t  P_Rsa_PrivExponent_Size,
                                       int32_t  P_Rsa_PubExponent_Size,
                                       uint32_t *P_plength)
{
  int32_t mbedtls_ret;
  uint8_t wrap_ret;
  mbedtls_mpi N;
  mbedtls_mpi D;
  mbedtls_mpi E;
  mbedtls_pk_context pk_ctx;
  mbedtls_rsa_context *rsa_ctx = NULL;

  if ((P_pDer == NULL)
      || (P_pRsa_Modulus == NULL)
      || (P_pRsa_PrivExponent == NULL)
      || (P_pRsa_PubExponent == NULL)
      || (P_plength == NULL) )
  {
    return  WRAP_FAILURE;
  }
  if (P_Rsa_Modulus_Size < 0
      || P_Rsa_PrivExponent_Size < 0
      || P_Rsa_PubExponent_Size < 0)
  {
    return WRAP_FAILURE;
  }

  /*Init Mpi*/
  mbedtls_mpi_init(&N);
  mbedtls_mpi_init(&D);
  mbedtls_mpi_init(&E);

  /*Transform key's components into MPI*/
  /*Pointer already checked*/
  (void)uint8_t_to_mpi(&N, P_pRsa_Modulus, P_Rsa_Modulus_Size);
  (void)uint8_t_to_mpi(&D, P_pRsa_PrivExponent, P_Rsa_PrivExponent_Size);
  (void)uint8_t_to_mpi(&E, P_pRsa_PubExponent, P_Rsa_PubExponent_Size);

  /*Init context*/
  mbedtls_pk_init(&pk_ctx);

  mbedtls_ret = mbedtls_pk_setup(&pk_ctx, &mbedtls_rsa_info);
  if (mbedtls_ret == 0)
  {
    /*Get a RSA context from a PK one*/
    rsa_ctx = mbedtls_pk_rsa(pk_ctx);
    mbedtls_rsa_init(rsa_ctx, MBEDTLS_RSA_PKCS_V15, 0);
    mbedtls_ret = mbedtls_rsa_import(rsa_ctx, &N, NULL, NULL, &D, &E);
    if (mbedtls_ret == 0)
    {
      /*Complete the key's components*/
      mbedtls_ret = mbedtls_rsa_complete(rsa_ctx);
      if (mbedtls_ret == 0)
      {
        /*Export to a DER format the keypair*/
        *P_plength = (uint32_t) mbedtls_pk_write_key_der(&pk_ctx, P_pDer, P_pDer_size);
        wrap_ret = WRAP_SUCCESS;
      }
      else
      {
        wrap_ret = WRAP_FAILURE;
      }
    }
    else
    {
      wrap_ret = WRAP_BAD_KEY;
    }
  }
  else
  {
    wrap_ret = WRAP_FAILURE;
  }

  /*free*/
  mbedtls_rsa_free(rsa_ctx);
  mbedtls_pk_free(&pk_ctx);
  mbedtls_mpi_free(&N);
  mbedtls_mpi_free(&D);
  mbedtls_mpi_free(&E);
  return wrap_ret;
}

/**
  * @brief      Import a DER public key to a psa struct
  * @param[in,out]
                *P_Key_Handle: Handle of the key
  * @param[in]  P_Psa_Usage: Key usage
  * @param[in]  P_Psa_Algorithm: Algorithm that will be used with the key
  * @param[in]  *P_pDer_pubKey: The key
  * @param[in]  P_KeySize: The size of the key in bytes
  * @retval     RSA_ERR_BAD_PARAMETER: Could not import the key
  * @retval     PSA_SUCCESS: Operation Successful
  */
static psa_status_t wrap_import_der_Key_into_psa(psa_key_handle_t *P_Key_Handle,
                                                 psa_key_usage_t P_Psa_Usage,
                                                 psa_algorithm_t  P_Psa_Algorithm,
                                                 psa_key_type_t P_Psa_key_type,
                                                 const uint8_t *P_pDer_pubKey,
                                                 uint32_t P_KeySize)
{
  psa_status_t psa_ret_status;
  psa_key_policy_t key_policy = {0};

  /* Import key into psa struct*/
  /*Allocate key*/
  psa_ret_status = psa_allocate_key(P_Key_Handle);
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Set algorithm*/
    psa_key_policy_set_usage(&key_policy, P_Psa_Usage, P_Psa_Algorithm);
    psa_ret_status = psa_set_key_policy(*P_Key_Handle, &key_policy);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Import key*/
      psa_ret_status = psa_import_key(*P_Key_Handle,
                                      P_Psa_key_type,
                                      P_pDer_pubKey,
                                      P_KeySize);
      if (psa_ret_status == PSA_SUCCESS)
      {
        psa_ret_status = RSA_SUCCESS;
      }
      else
      {
        psa_ret_status = RSA_ERR_BAD_PARAMETER;
      }
    }
    else
    {
      psa_ret_status = RSA_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    psa_ret_status = RSA_ERR_BAD_PARAMETER;
  }
  return psa_ret_status;
}
#endif /*WRAP_RSA*/

#ifdef WRAP_ECDSA
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief      Initialize the elliptic curve parameters into a EC_stt structure
  * @param[in, out]
  *             *P_pECctx: EC_stt context with parameters of ellliptic curve used
  *             NOT USED
  * @param[in, out]
  *             *P_pMemBuf: Pointer to membuf_stt structure that will be used
  *             to store the Ellitpic Curve internal values. NOT USED
  * @retval     ECC_SUCCESS: Operation Successful
  * @retval     ECC_ERR_BAD_PARAMETER: otherwise
  */
int32_t ECCinitEC(EC_stt *P_pECctx, membuf_stt *P_pMemBuf)
{
  int32_t ecc_ret_status = ECC_SUCCESS;
  psa_status_t psa_ret_status;

  /* Initialize MBED crypto library */
  psa_ret_status = psa_crypto_init();
  if (psa_ret_status != PSA_SUCCESS)
  {
    ecc_ret_status = ECC_ERR_BAD_OPERATION;
  }
  return ecc_ret_status;
}

/**
  * @brief      Do nothing
  * @retval     ECC_SUCCESS
  */
int32_t ECCfreeEC(EC_stt *P_pECctx, membuf_stt *P_pMemBuf)
{
  return ECC_SUCCESS;
}

/**
  * @brief      Initialize an ECC point
  * @param[out] **P_ppECPnt: The point that will be initialized
  * @param[in]  *P_pECctx: The EC_stt containing the Elliptic Curve Parameters
  *             NOT USED
  * @param[in, out]
  *             *P_pMemBuf: Pointer to the membuf_stt structure that will
  *             be used to store the Ellitpic Curve Point internal values NOT USED
  * @retval     ECC_SUCCESS: Operation Successful
  * @retval     ECC_ERR_BAD_PARAMETER: P_ppECPnt == NULL
  * @retval     ERR_MEMORY_FAIL: Not enough memory
  */
int32_t ECCinitPoint(ECpoint_stt **P_ppECPnt, const EC_stt *P_pECctx, membuf_stt *P_pMemBuf)
{
  int32_t ecc_ret_status;

  if (P_ppECPnt == NULL)
  {
    ecc_ret_status = ECC_ERR_BAD_PARAMETER;
  }
  else
  {
    /* Allocation of points, use of malloc,
    because we don't know the maximum number of points*/
    *P_ppECPnt = malloc(sizeof(ECpoint_stt));
    if (*P_ppECPnt  == NULL)
    {
      ecc_ret_status = ERR_MEMORY_FAIL;
    }
    else
    {
      /*Allocate pmX*/
      (**P_ppECPnt).pmX = malloc(sizeof(BigNum_stt));
      if ((**P_ppECPnt).pmX == NULL)
      {
        ecc_ret_status =  ERR_MEMORY_FAIL;
      }
      else
      {
        /*Allocate pmY*/
        (**P_ppECPnt).pmY = malloc(sizeof(BigNum_stt));
        if ((**P_ppECPnt).pmY == NULL)
        {
          ecc_ret_status =  ERR_MEMORY_FAIL;
        }
        else
        {
          /*Allocate pmZ*/
          (**P_ppECPnt).pmZ = malloc(sizeof(BigNum_stt));
          if ((**P_ppECPnt).pmZ == NULL)
          {
            ecc_ret_status = ERR_MEMORY_FAIL;
          }
          else
          {
            ecc_ret_status = ECC_SUCCESS;
          }
        }
      }
    }
  }
  return ecc_ret_status;
}

/**
  * @brief      Free Elliptic curve point
  * @param[in]  *P_pECPnt The point that will be free
  * @param[in, out]
  *             *P_pMemBuf Pointer to membuf_stt structure that stores Ellitpic
  *             Curve Point internal values. NOT USED
  * @retval     ECC_SUCCESS: Operation Successful
  * @retval     ECC_ERR_BAD_PARAMETER P_pECPnt == NULL
  * @retval     ECC_ERR_BAD_CONTEXT  *P_pECPnt == NULL
  */
int32_t ECCfreePoint(ECpoint_stt **P_pECPnt, membuf_stt *P_pMemBuf)
{
  if (P_pECPnt == NULL)
  {
    return ECC_ERR_BAD_PARAMETER;
  }
  if (*P_pECPnt == NULL)
  {
    return ECC_ERR_BAD_CONTEXT;
  }

  /*Reset and delete links*/
  free(((**P_pECPnt).pmX)->pmDigit);
  free(((**P_pECPnt).pmY)->pmDigit);
  free((**P_pECPnt).pmX);
  free((**P_pECPnt).pmY);
  free((**P_pECPnt).pmZ);
  free(*P_pECPnt);

  return ECC_SUCCESS;
}

/**
  * @brief      Set the value of one of coordinate of an ECC point
  * @param[in, out]
  *             *P_pECPnt: The ECC point that will have a coordinate set
  * @param[in]  P_Coordinate: Flag used to select which coordinate must be set
  *             (see ECcoordinate_et )
  * @param[in]  *P_pCoordinateValue: Pointer to an uint8_t array that
  *             contains the value to be set
  * @param[in]  P_coordinateSize: The size in bytes of P_pCoordinateValue
  * @retval     ECC_SUCCESS: Operation Successful
  * @retval     ECC_ERR_BAD_PARAMETER: One of the input parameters is invalid
  */
int32_t ECCsetPointCoordinate(ECpoint_stt *P_pECPnt,
                              ECcoordinate_et P_Coordinate,
                              const uint8_t *P_pCoordinateValue,
                              int32_t P_coordinateSize)
{
  int32_t wrap_ret_status;
  int32_t ecc_ret_status;

  if ((P_pECPnt == NULL) || (P_pCoordinateValue == NULL))
  {
    return ECC_ERR_BAD_PARAMETER;
  }

  /* Are we setting the X coordinate ?*/
  if (P_Coordinate == E_ECC_POINT_COORDINATE_X)
  {
    /*Set X*/
    wrap_ret_status = wrap_uint8_to_BigNum(P_pECPnt->pmX,
                                              P_pCoordinateValue,
                                              P_coordinateSize);
    if (wrap_ret_status == WRAP_SUCCESS)
    {
      ecc_ret_status = ECC_SUCCESS;
    }
    else
    {
      ecc_ret_status = ECC_ERR_BAD_PARAMETER;
    }
  }

  /* Are we setting the Y coordinate ?*/
  else if (P_Coordinate == E_ECC_POINT_COORDINATE_Y)
  {
    /*Set Y*/
    wrap_ret_status = wrap_uint8_to_BigNum(P_pECPnt->pmY,
                                              P_pCoordinateValue,
                                              P_coordinateSize);
    if (wrap_ret_status == WRAP_SUCCESS)
    {
      ecc_ret_status = ECC_SUCCESS;
    }
    else
    {
      ecc_ret_status = ECC_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    ecc_ret_status = ECC_ERR_BAD_PARAMETER;
  }

  return ecc_ret_status;
}

/**
  * @brief      Get the value of one of coordinate of an ECC point
  * @param[in]  *P_pECPnt: The ECC point from which extract the coordinate
  * @param[in]  P_Coordinate: Flag used to select which coordinate must
  *             be retrieved (see ECcoordinate_et )
  * @param[out] *P_pCoordinateValue: Pointer to an uint8_t array that will
  *             contain the returned coordinate
  * @param[out] *P_pCoordinateSize: Pointer to an integer that will
  *             contain the size of the returned coordinate
  * @note       The Coordinate size depends only on the size of the Prime (P)
  *             of the elliptic curve. Specifically if P_pECctx->mPsize is not
  *             a multiple of 4, then the size will be expanded to be
  *             a multiple of 4. In this case P_pCoordinateValue will contain
  *             one or more leading zeros.
  * @retval     ECC_SUCCESS: Operation Successful
  * @retval     ECC_ERR_BAD_PARAMETER: One of the input parameters is invalid
  */
int32_t ECCgetPointCoordinate(const ECpoint_stt *P_pECPnt,
                              ECcoordinate_et P_Coordinate,
                              uint8_t *P_pCoordinateValue,
                              int32_t *P_pCoordinateSize)
{

  int32_t ecc_ret_status = ECC_SUCCESS;

  if ((P_pECPnt == NULL)
      || (P_pCoordinateValue == NULL)
      || (P_pCoordinateSize == NULL))
  {
    return ECC_ERR_BAD_OPERATION;
  }

  /* Do we want to retrive X value ?*/
  if (P_Coordinate == E_ECC_POINT_COORDINATE_X)
  {
    if (P_pECPnt->pmX->pmDigit == NULL)
    {
      return ECC_ERR_BAD_OPERATION;
    }
    /*Copy the data to the output*/
    (void)memcpy(P_pCoordinateValue, P_pECPnt->pmX->pmDigit, P_pECPnt->pmX->mNumDigits);
    *P_pCoordinateSize = (int32_t) P_pECPnt->pmX->mNumDigits;
  }

  /* Do we want to retrive Y value ?*/
  else if (P_Coordinate == E_ECC_POINT_COORDINATE_Y)
  {
    if (P_pECPnt->pmY->pmDigit == NULL)
    {
      return ECC_ERR_BAD_OPERATION;
    }
    /*Copy the data to the output*/
    (void)memcpy(P_pCoordinateValue, P_pECPnt->pmY->pmDigit, P_pECPnt->pmY->mNumDigits);
    *P_pCoordinateSize = (int32_t) P_pECPnt->pmY->mNumDigits;
  }
  else
  {
    ecc_ret_status = ECC_ERR_BAD_OPERATION;
  }

  return ecc_ret_status;
}

/**
  * @brief  Do nothing
  * @retval ECC_ERR_BAD_OPERATION
  */
int32_t ECCgetPointFlag(const ECpoint_stt *P_pECPnt)
{
  /*Do nothing*/
  return ECC_ERR_BAD_OPERATION;
}

/**
  * @brief  Do nothing
  */
void ECCsetPointFlag(ECpoint_stt *P_pECPnt,
                     ECPntFlags_et P_newFlag)
{
  /*Do nothing*/
}

/**
  * @brief  Do nothing
  * @retval ECC_ERR_BAD_OPERATION
  */
int32_t ECCsetPointGenerator(ECpoint_stt *P_pPoint, const EC_stt *P_pECctx)
{
  /*Do nothing*/
  return ECC_ERR_BAD_OPERATION;
}

/**
  * @brief  Do nothing
  * @retval ECC_ERR_BAD_OPERATION
  */
int32_t ECCcopyPoint(const ECpoint_stt *P_pOriginalPoint, ECpoint_stt *P_pCopyPoint)
{
  /*Do nothing*/
  return ECC_ERR_BAD_OPERATION;
}

/**
  * @brief  Init the ECC private key structure with a static array
  * @param[in,out]
  *             **P_ppECCprivKey: The ECC private key that will be init
  * @param[in]  *P_pECctx: NOT USED
  * @param[in]  *P_pMemBuf: NOT USED
  * @retval ECC_ERR_BAD_OPERATION: When **P_ppECCprivKey is NULL
  * @retval ECC_SUCCESS: Otherwise
  */
int32_t ECCinitPrivKey(ECCprivKey_stt **P_ppECCprivKey, const EC_stt *P_pECctx, membuf_stt *P_pMemBuf)
{
  if (P_ppECCprivKey == NULL)
  {
    return ECC_ERR_BAD_OPERATION;
  }
  else
  {
    /*init the private key with static array*/
    static_ECC_privKey.pmD = &static_ECC_bigNum_privKey;
    *P_ppECCprivKey = &static_ECC_privKey;

    return ECC_SUCCESS;
  }
}

/**
  * @brief  Free the Private key
  * @param[in,out]
  *          **P_ppECCprivKey: The ECC private key that will be free
  * @param[in]  *P_pMemBuf: NOT USED
  * @retval ECC_ERR_BAD_OPERATION: When **P_ppECCprivKey is NULL
  * @retval ECC_SUCCESS: Otherwise
  */
int32_t ECCfreePrivKey(ECCprivKey_stt **P_ppECCprivKey, membuf_stt *P_pMemBuf)
{
  if (P_ppECCprivKey == NULL)
  {
    return ECC_ERR_BAD_OPERATION;
  }
  else
  {
    /*free and reset*/
    free((**P_ppECCprivKey).pmD->pmDigit);
    (**P_ppECCprivKey).pmD = NULL;
    *P_ppECCprivKey = NULL;
    (void)memset(static_ECDSA_privKey, 0, ECDSA_PRIVKEY_MAXSIZE);
    return ECC_SUCCESS;
  }
}

/**
  * @brief  Set the priv key value
  * @param[in, out]
                *P_pECCprivKey: The ECC private key object to set
  * @param[in]  *P_pPrivateKey: Pointer to an uint8_t array that contains
  *             the value of the private key
  * @param[in]  P_privateKeySize: The size in bytes of P_pPrivateKey
  * @retval ECC_SUCCESS: On success
  * @retval ECC_ERR_BAD_OPERATION: Invalid parameters
  */
int32_t ECCsetPrivKeyValue(ECCprivKey_stt *P_pECCprivKey,
                           const uint8_t *P_pPrivateKey,
                           int32_t P_privateKeySize)
{
  uint8_t wrap_ret_status;
  int32_t ecc_ret_status = ECC_SUCCESS;

  if ((P_pECCprivKey == NULL) || (P_pPrivateKey == NULL) )
  {
    return ECC_ERR_BAD_OPERATION;
  }
  if (P_privateKeySize < 0)
  {
    return ECC_ERR_BAD_OPERATION;
  }

  /*Set teh key*/
  wrap_ret_status = wrap_uint8_to_BigNum(P_pECCprivKey->pmD, P_pPrivateKey, P_privateKeySize);
  if (wrap_ret_status == WRAP_FAILURE)
  {
    ecc_ret_status = ECC_ERR_BAD_OPERATION;
  }
  return ecc_ret_status;
}

/**
  * @brief  Get the private key value
  * @param[in]   *P_pECCprivKey: The ECC private key object to get
  * @param[out]  *P_pPrivateKey: Pointer to an uint8_t array that contains
  *             the value of the private key
  * @param[out]  *P_pPrivateKeySize: The size in bytes of P_pPrivateKey
  * @retval ECC_SUCCESS: On success
  * @retval ECC_ERR_BAD_OPERATION: Invalid parameters
  */
int32_t ECCgetPrivKeyValue(const ECCprivKey_stt *P_pECCprivKey,
                           uint8_t *P_pPrivateKey,
                           int32_t *P_pPrivateKeySize)
{
  uint8_t wrap_ret_status;
  int32_t ecc_ret_status = ECC_SUCCESS;

  if ((P_pECCprivKey == NULL)
      || (P_pPrivateKey == NULL)
      || (P_pPrivateKeySize == NULL) )
  {
    return ECC_ERR_BAD_OPERATION;
  }
  /*Get the key value and put in the output*/
  wrap_ret_status = wrap_BigNum_to_uint8(P_pPrivateKey,
                                         P_pECCprivKey->pmD,
                                         P_pPrivateKeySize);
  if (wrap_ret_status == WRAP_FAILURE)
  {
    ecc_ret_status = ECC_ERR_BAD_OPERATION;
  }
  return ecc_ret_status;
}


/**
  * @brief  Do nothing
  * @retval ECC_ERR_BAD_OPERATION
  */
int32_t ECCscalarMul(const ECpoint_stt *P_pECbasePnt,
                     const ECCprivKey_stt *P_pECCprivKey,
                     ECpoint_stt *P_pECresultPnt,
                     const EC_stt *P_pECctx,
                     membuf_stt *P_pMemBuf)
{
  /*Do nothing*/
  return ECC_ERR_BAD_OPERATION;
}

/**
  * @brief  Init a Signature
  * @param[in]  **P_ppSignature: The initialized Signature
  * @param[in]  *P_pECctx: The ecc context NOT USED
  * @param[in]  *P_pMemBuf: NOT USED
  * @retval ECC_SUCCESS: on Success
  * @retval ECC_ERR_BAD_PARAMETER: P_ppSignature == NULL
  */
int32_t ECDSAinitSign(ECDSAsignature_stt **P_ppSignature, const EC_stt *P_pECctx, membuf_stt *P_pMemBuf)
{
  int32_t ecc_ret_status;
  if (P_ppSignature == NULL)
  {
    ecc_ret_status = ECC_ERR_BAD_PARAMETER;
  }
  else
  {
    /*Init with static array*/
    *P_ppSignature = &static_ECC_Sign;
    (**P_ppSignature).pmR = &static_ECDSA_R;
    (**P_ppSignature).pmS = &static_ECDSA_S;
    ecc_ret_status = ECC_SUCCESS;
  }
  return ecc_ret_status;
}

/**
  * @brief      Free an ECDSA signature structure
  * @param[in, out]
  *             **P_pSignature: The ECDSA signature that will be free
  * @param[in, out]
  *             *P_pMemBuf: Pointer to the membuf_stt structure that currently
  *             stores the ECDSA signature internal values NOT USED
  * @retval     ECC_SUCCESS Operation Successful
  * @retval     ECC_ERR_BAD_PARAMETER: P_pSignature == NULL
  */
int32_t ECDSAfreeSign(ECDSAsignature_stt **P_pSignature, membuf_stt *P_pMemBuf)
{
  if ((P_pSignature == NULL))
  {
    return ECC_ERR_BAD_PARAMETER;
  }

  /*Free and reset*/
  free((**P_pSignature).pmR->pmDigit);
  free((**P_pSignature).pmS->pmDigit);
  (**P_pSignature).pmR = NULL;
  (**P_pSignature).pmS = NULL;
  *P_pSignature = NULL;
  return ECC_SUCCESS;
}

/**
  * @brief      Set the value of the parameters (one at a time) of an ECDSAsignature_stt
  * @param[out] *P_pSignature: The ECDSA signature whose one of the value
  *             will be set.
  * @param[in]  P_RorS: Flag selects if the parameter R or the parameter S
  *             must be set.
  * @param[in]  *P_pValue: Pointer to an uint8_t array containing the signature value
  * @param[in]  P_valueSize: Size of the signature value
  * @retval     ECC_SUCCESS: Operation Successful
  * @retval     ECC_ERR_BAD_PARAMETER: otherwise
  */
int32_t ECDSAsetSignature(ECDSAsignature_stt *P_pSignature,
                          ECDSAsignValues_et P_RorS,
                          const uint8_t *P_pValue,
                          int32_t P_valueSize)
{
  uint8_t wrap_ret_status = WRAP_SUCCESS;
  int32_t ecc_ret_status = ECC_SUCCESS;

  if ((P_pValue == NULL) || (P_pSignature == NULL) )
  {
    return ECC_ERR_BAD_PARAMETER;
  }
  /*Are we setting R ?*/
  if (P_RorS == E_ECDSA_SIGNATURE_R_VALUE)
  {
    /*Set R*/
    wrap_ret_status = wrap_uint8_to_BigNum(P_pSignature->pmR, P_pValue, P_valueSize);
  }
  /*Or S ?*/
  else if (P_RorS == E_ECDSA_SIGNATURE_S_VALUE)
  {
    /*Set S*/
    wrap_ret_status = wrap_uint8_to_BigNum(P_pSignature->pmS, P_pValue, P_valueSize);
  }
  else
  {
    ecc_ret_status = ECC_ERR_BAD_PARAMETER;
  }
  if (wrap_ret_status == WRAP_FAILURE)
  {
    ecc_ret_status = ECC_ERR_BAD_PARAMETER;
  }
  return ecc_ret_status;
}

/**
  * @brief      GEt the value of the parameters (one at a time) of an ECDSAsignature_stt
  * @param[out] *P_pSignature: The ECDSA signature from which retrieve the value
  * @param[in]  P_RorS: Flag selects if the parameter R or the parameter S
  *             must be return.
  * @param[in]  *P_pValue: Pointer to an uint8_t array containing the value
  * @param[in]  *P_pValueSize: Pointer to integer that contains the size of returned value
  * @retval     ECC_SUCCESS: Operation Successful
  * @retval     ECC_ERR_BAD_PARAMETER: otherwise
  */
int32_t ECDSAgetSignature(const ECDSAsignature_stt *P_pSignature,
                          ECDSAsignValues_et P_RorS,
                          uint8_t *P_pValue,
                          int32_t *P_pValueSize)
{
  uint8_t wrap_ret_status = WRAP_SUCCESS;
  int32_t ecc_ret_status = ECC_SUCCESS;

  if ((P_pValue == NULL)
      || (P_pValueSize == NULL)
      || (P_pSignature == NULL) )
  {
    return ECC_ERR_BAD_PARAMETER;
  }
  /*Do we want the R value ?*/
  if (P_RorS == E_ECDSA_SIGNATURE_R_VALUE)
  {
    wrap_ret_status = wrap_BigNum_to_uint8(P_pValue, P_pSignature->pmR, P_pValueSize);
  }
  /*Or the S one ?*/
  else if (P_RorS == E_ECDSA_SIGNATURE_S_VALUE)
  {
    wrap_ret_status = wrap_BigNum_to_uint8(P_pValue, P_pSignature->pmS, P_pValueSize);
  }
  else
  {
    ecc_ret_status = ECC_ERR_BAD_PARAMETER;
  }
  if (wrap_ret_status == WRAP_FAILURE)
  {
    ecc_ret_status = ECC_ERR_BAD_PARAMETER;
  }
  return ecc_ret_status;
}

/**
  * @brief      ECDSA signature verification with a digest input
  * @param[in]  *P_pDigest: The digest of the signed message
  * @param[in]  P_digestSize: The mSize in bytes of the digest
  * @param[in]  *P_pSignature: The public key that will verify the signature.
  * @param[in]  *P_pVerifyCtx: The ECDSA signature that will be verified
  * @param[in, out]
  *             *P_pMemBuf: Pointer to the membuf_stt structure that will be used
  *             to store the internal values required by computation. NOT USED
  * @note       This function requires that:
  *             - P_pVerifyCtx.pmEC points to a valid and initialized EC_stt structure
  *             - P_pVerifyCtx.pmPubKey points to a valid and initialized public key ECpoint_stt structure
  * @retval     ERR_MEMORY_FAIL: There's not enough memory
  * @retval     ECC_ERR_BAD_PARAMETER: An error occur
  * @retval     ECC_ERR_BAD_CONTEXT: One parameter is not valid
  * @retval     SIGNATURE_INVALID: The signature is NOT valid
  * @retval     SIGNATURE_VALID: The signature is valid
  */
int32_t ECDSAverify(const uint8_t      *P_pDigest,
                    int32_t             P_digestSize,
                    const ECDSAsignature_stt   *P_pSignature,
                    const ECDSAverifyCtx_stt *P_pVerifyCtx,
                    membuf_stt *P_pMemBuf)
{
  int8_t wrap_ret_status;
  int32_t ecc_ret_status;
  psa_status_t psa_ret_status;
  psa_status_t psa_ret_status_tp;
  const psa_algorithm_t psa_algorithm = PSA_ALG_ECDSA_BASE;
  int32_t wrap_sign_R_size = 0;
  int32_t wrap_sign_S_size = 0;
  int32_t wrap_pubKey_x = 0;
  int32_t wrap_pubKey_y = 0;
  uint32_t wrap_pubKey;
  psa_ecc_curve_t wrap_ecc_curve;
  psa_key_handle_t ECDSAkeyHandle = {0};

  if ((P_pDigest == NULL) || (P_pVerifyCtx == NULL) || (P_pSignature == NULL) )
  {
    return ECC_ERR_BAD_PARAMETER;
  }
  if ((P_pVerifyCtx->pmPubKey == NULL) || (P_pVerifyCtx->pmEC == NULL) )
  {
    return ECC_ERR_BAD_PARAMETER;
  }
  /*Save R*/
  wrap_ret_status = wrap_BigNum_to_uint8(static_ECDSA_Sign,
                                         P_pSignature->pmR,
                                         &wrap_sign_R_size);
  if (wrap_ret_status == WRAP_SUCCESS)
  {
    /*Save S right after R*/
    wrap_ret_status = wrap_BigNum_to_uint8(&static_ECDSA_Sign[wrap_sign_R_size],
                                           P_pSignature->pmS,
                                           &wrap_sign_S_size);
  }
  else
  {
    return ECC_ERR_BAD_PARAMETER;
  }
  /*Choose which curves to use*/
  if (wrap_sign_R_size == 24) /*192 /8*/
  {
    wrap_ecc_curve = PSA_ECC_CURVE_SECP192R1;
  }
  else if (wrap_sign_R_size == 32) /*256 /8 */
  {
    wrap_ecc_curve = PSA_ECC_CURVE_SECP256R1;
  }
  else if (wrap_sign_R_size == 48) /*384 /8 */
  {
    wrap_ecc_curve = PSA_ECC_CURVE_SECP384R1;
  }
  else
  {
    return ECC_ERR_BAD_PARAMETER;
  }

  /*Pubkey is in the P_pVerifyCtx,
  so we need to get the datas and put those into the right format*/
  /*It as to be in this format: 0x04 + x_P + y_P.
  0x04 mean uncompress key*/
  static_ECDSA_pubKey[0] = 0x04;
  wrap_ret_status = wrap_BigNum_to_uint8(&static_ECDSA_pubKey[1],
                                         P_pVerifyCtx->pmPubKey->pmX,
                                         &wrap_pubKey_x);
  if (wrap_ret_status == WRAP_SUCCESS)
  {
    wrap_ret_status = wrap_BigNum_to_uint8(&static_ECDSA_pubKey[1 + wrap_pubKey_x],
                                           P_pVerifyCtx->pmPubKey->pmY,
                                           &wrap_pubKey_y);
    wrap_pubKey = wrap_pubKey_x + wrap_pubKey_y + 1U;
  }
  else
  {
    return ECC_ERR_BAD_PARAMETER;
  }

  if (wrap_pubKey > ECDSA_PUBKEY_MAXSIZE)
  {
    return ECC_ERR_BAD_PARAMETER;
  }

  /*Import the key*/
  psa_ret_status = wrap_import_ecc_pubKey_into_psa(&ECDSAkeyHandle,
                                                   PSA_KEY_USAGE_VERIFY,
                                                   psa_algorithm,
                                                   wrap_ecc_curve,
                                                   static_ECDSA_pubKey,
                                                   wrap_pubKey);
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Verify*/
    psa_ret_status = psa_asymmetric_verify(ECDSAkeyHandle,
                                           psa_algorithm,
                                           P_pDigest,
                                           (uint32_t) P_digestSize,
                                           static_ECDSA_Sign,
                                           (uint32_t) (wrap_sign_R_size + wrap_sign_S_size) );
    /*We won't be able to reuse the key, so we destroy it*/
    psa_ret_status_tp = psa_destroy_key(ECDSAkeyHandle);
    if (psa_ret_status_tp == PSA_SUCCESS)
    {
      if (psa_ret_status == PSA_ERROR_INVALID_SIGNATURE)
      {
        ecc_ret_status = SIGNATURE_INVALID;
      }
      else if (psa_ret_status == PSA_ERROR_INSUFFICIENT_MEMORY)
      {
        ecc_ret_status = ERR_MEMORY_FAIL;
      }
      else if (psa_ret_status == PSA_ERROR_INVALID_ARGUMENT)
      {
        ecc_ret_status = ECC_ERR_BAD_PARAMETER;
      }
      /*Success*/
      else if (psa_ret_status == PSA_SUCCESS)
      {
        ecc_ret_status = SIGNATURE_VALID;
      }
      /* In case of other return status*/
      else
      {
        ecc_ret_status = ECC_ERR_BAD_CONTEXT;
      }
    }
    else
    {
      ecc_ret_status = ECC_ERR_BAD_OPERATION;
    }
  }
  else
  {
    ecc_ret_status = ECC_ERR_BAD_OPERATION;
  }

  return ecc_ret_status;
}

/**
  * @brief  Checks the validity of a public key
  * @param[in]  *pECCpubKey: The public key to be checked
  * @param[in]  *P_pECctx: Structure describing the curve parameters
  * @param[in, out]
  *             *P_pMemBuf: Pointer to the membuf_stt structure that will
  *             be used to store the internal values required by computation NOT USED
  * @retval ECC_SUCCESS pECCpubKey is a valid point of the curve
  * @retval ECC_ERR_BAD_PUBLIC_KEY pECCpubKey is not a valid point of the curve
  * @retval ECC_ERR_BAD_PARAMETER One of the input parameter is NULL
  * @retval ECC_ERR_BAD_CONTEXT One of the values inside P_pECctx is invalid
  * @retval ECC_ERR_BAD_OPERATION Default error value
  */
int32_t ECCvalidatePubKey(const ECpoint_stt *pECCpubKey, const EC_stt *P_pECctx, membuf_stt *P_pMemBuf)
{
  int32_t ecc_ret_status = ECC_ERR_BAD_OPERATION;
  uint8_t wrap_ret_status;
  int32_t ret_status = 0;
  mbedtls_mpi x;
  mbedtls_mpi y;
  mbedtls_mpi a;
  mbedtls_mpi b;
  mbedtls_mpi p;
  mbedtls_mpi aX;
  mbedtls_mpi X2;
  mbedtls_mpi X3;
  mbedtls_mpi Yc;
  mbedtls_mpi Y;

  if ((pECCpubKey == NULL) || (P_pECctx == NULL) )
  {
    return ECC_ERR_BAD_PARAMETER;
  }

  /*Put x in yc^2 = x^3 + ax + b */
  /*if yc == y it is a valid key, otherwise it's not */


  /*init*/
  wrap_ret_status = bignum_to_mpi(&x, pECCpubKey->pmX);
  wrap_ret_status &= bignum_to_mpi(&y, pECCpubKey->pmY);
  wrap_ret_status &= uint8_t_to_mpi(&a, P_pECctx->pmA, P_pECctx->mAsize);
  wrap_ret_status &= uint8_t_to_mpi(&b, P_pECctx->pmB, P_pECctx->mBsize);
  wrap_ret_status &= uint8_t_to_mpi(&p, P_pECctx->pmP, P_pECctx->mPsize);
  if (wrap_ret_status == WRAP_SUCCESS)
  {  
  
    mbedtls_mpi_init(&aX);
    mbedtls_mpi_init(&X2);
    mbedtls_mpi_init(&X3);
    mbedtls_mpi_init(&Yc);
    mbedtls_mpi_init(&Y);

    /*Compute*/
    ret_status = mbedtls_mpi_mul_mpi(&X2, &x, &x);       /*X2 = x^2*/
    ret_status += mbedtls_mpi_mul_mpi(&X3, &X2, &x);      /*X3 = x^3 = X2*x*/
    ret_status += mbedtls_mpi_mul_mpi(&aX, &a, &x);       /*X = a*x*/
    ret_status += mbedtls_mpi_add_mpi(&Yc, &X3, &aX);     /*Yc = X^3+ax*/
    ret_status += mbedtls_mpi_add_mpi(&Yc, &Yc, &b);      /*Yc = X^3+aX+b*/
    ret_status += mbedtls_mpi_mod_mpi(&Yc, &Yc, &p);      /*Yc = Yc mod p*/

    if (ret_status == 0)
    {
      ret_status = mbedtls_mpi_mul_mpi(&Y, &y, &y);       /*Y = y^2*/
      ret_status += mbedtls_mpi_mod_mpi(&Y, &Y, &p);      /*Y = Y mod p*/
      if (ret_status == 0)
      {
        ret_status = mbedtls_mpi_cmp_mpi(&Yc, &Y);        /*Yc ?= Y */
        if (ret_status == 0)
        {
          ecc_ret_status = ECC_SUCCESS;
        }
        else
        {
          ecc_ret_status = ECC_ERR_BAD_PUBLIC_KEY;
        }
      }
      else
      {
        ecc_ret_status = ECC_ERR_BAD_CONTEXT;
      }
    }
    else
    {
      ecc_ret_status = ECC_ERR_BAD_CONTEXT;
    }
  }
  /*Free all*/
  free(a.p);
  free(b.p);
  free(p.p);
  mbedtls_mpi_free(&aX);
  mbedtls_mpi_free(&X2);
  mbedtls_mpi_free(&X3);
  mbedtls_mpi_free(&Yc);
  mbedtls_mpi_free(&Y);

  return ecc_ret_status;
}

/**
  * @brief  Do nothing
  * @retval ECC_ERR_BAD_OPERATION
  */
int32_t ECCkeyGen(ECCprivKey_stt *P_pPrivKey,
                  ECpoint_stt    *P_pPubKey,
                  RNGstate_stt *P_pRandomState,
                  const EC_stt    *P_pECctx,
                  membuf_stt *P_pMemBuf)
{
  /*Do nothing*/
  return ECC_ERR_BAD_OPERATION;
}
/**
  * @brief  ECDSA Signature Generation
  * @param[in]  *P_pDigest: The message digest that will be signed
  * @param[in]  P_digestSize: The size in bytes of the P_pDigest
  * @param[out] *P_pSignature: Pointer to an initialized signature structure
  *             that will be contain the result of the operaion
  * @param[in]  *P_pSignCtx: Pointer to an initialized ECDSAsignCtx_stt structure
  * @param[in, out]
  *             *P_pMemBuf Pointer to the membuf_stt structure that will
  *             be used to store the internal values required by computation NOT USED
  * @note       Only the curves: P-192, P-256, p-384 are supported for the moment
  *             This function requires that:
  *             - P_pSignCtx.pmEC points to a valid and initialized
  *               EC_stt structure
  *             - P_pSignCtx.pmPrivKey points to a valid and initialized
  *               private key ECCprivKey_stt structure
  *             - P_pSignCtx.pmRNG points to a valid and initialized
  *               Random State RNGstate_stt structure
  * @retval     ECC_SUCCESS: Operation Successfull
  * @retval     ECC_ERR_BAD_PARAMETER: An error occur
  * @retval     ECC_ERR_BAD_CONTEXT: Some values inside P_pSignCtx are invalid
  * @retval     ERR_MEMORY_FAIL: There's not enough memory
  */
int32_t ECDSAsign(const uint8_t         *P_pDigest,
                  int32_t                P_digestSize,
                  const ECDSAsignature_stt *P_pSignature,
                  const ECDSAsignCtx_stt *P_pSignCtx,
                  membuf_stt *P_pMemBuf)
{
  const psa_algorithm_t psa_algorithm = PSA_ALG_ECDSA_BASE;
  psa_status_t psa_ret_status;
  psa_status_t psa_ret_status_tp;
  uint8_t wrap_ret_status;
  int32_t ecc_ret_status;
  int32_t wrap_PrivKey_size = 0;
  int32_t wrap_Sign_size;
  int32_t sign_size = 0;
  psa_ecc_curve_t wrap_ecc_curve;
  psa_key_handle_t ECDSAkeyHandle = {0};

  if ((P_pSignCtx == NULL) || (P_pDigest == NULL) || (P_pSignature == NULL) )
  {
    return ECC_ERR_BAD_PARAMETER;
  }
  if ((P_pSignCtx->pmEC == NULL)
      || (P_pSignCtx->pmPrivKey == NULL)
      || (P_pSignCtx->pmRNG == NULL))
  {
    return ECC_ERR_BAD_PARAMETER;
  }

  /*Put the key in the good format*/
  wrap_ret_status = wrap_BigNum_to_uint8(static_ECDSA_privKey,
                                         P_pSignCtx->pmPrivKey->pmD,
                                         &wrap_PrivKey_size);
  if (wrap_ret_status == WRAP_FAILURE)
  {
    return ECC_ERR_BAD_PARAMETER;
  }
  /*Choose which curves to use*/
  if (wrap_PrivKey_size == 24) /*192 /8*/
  {
    wrap_ecc_curve = PSA_ECC_CURVE_SECP192R1;
  }
  else if (wrap_PrivKey_size == 32) /*256 /8 */
  {
    wrap_ecc_curve = PSA_ECC_CURVE_SECP256R1;
  }
  else if (wrap_PrivKey_size == 48) /*384 /8 */
  {
    wrap_ecc_curve = PSA_ECC_CURVE_SECP384R1;
  }
  else
  {
    return ECC_ERR_BAD_CONTEXT;
  }
  /*import the key*/
  psa_ret_status = wrap_import_ecc_privKey_into_psa(&ECDSAkeyHandle,
                                                    PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY,
                                                    psa_algorithm,
                                                    wrap_ecc_curve,
                                                    static_ECDSA_privKey,
                                                    (uint32_t) wrap_PrivKey_size);
  if (psa_ret_status == PSA_SUCCESS)
  {
    /*Sign is equal to 2 * the key size*/
    wrap_Sign_size = wrap_PrivKey_size * 2;
    /*Sign*/
    psa_ret_status = psa_asymmetric_sign(ECDSAkeyHandle,
                                         psa_algorithm,
                                         P_pDigest,
                                         (uint32_t) P_digestSize,
                                         static_ECDSA_Sign,
                                         (uint32_t) wrap_Sign_size,
                                         (size_t *)&sign_size);
    /*We don't need the key anymore so we destroy it*/
    psa_ret_status_tp = psa_destroy_key(ECDSAkeyHandle);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Output R*/
      wrap_ret_status = wrap_uint8_to_BigNum(P_pSignature->pmR,
                                             static_ECDSA_Sign,
                                             (sign_size / 2) );
      if (wrap_ret_status == WRAP_SUCCESS)
      {
        /*Output s*/
        wrap_ret_status = wrap_uint8_to_BigNum(P_pSignature->pmS,
                                               static_ECDSA_Sign + (sign_size / 2),
                                               (sign_size / 2) );
        if (psa_ret_status_tp == PSA_SUCCESS)
        {
          ecc_ret_status = ECC_SUCCESS;
        }
        else
        {
          ecc_ret_status = ECC_ERR_BAD_OPERATION;
        }
      }
      else
      {
        ecc_ret_status = ECC_ERR_BAD_OPERATION;
      }
    }
    else
    {
      if (psa_ret_status == PSA_ERROR_INSUFFICIENT_MEMORY)
      {
        ecc_ret_status = ERR_MEMORY_FAIL;
      }
      else
      {
        ecc_ret_status = ECC_ERR_BAD_OPERATION;
      }
    }
  }
  else
  {
    ecc_ret_status = ECC_ERR_BAD_OPERATION;
  }
  return ecc_ret_status;
}
#endif /*WRAP_ECDSA*/

#ifdef WRAP_RSA
/**
  * @brief      PKCS#1v1.5 RSA Signature Generation Function
  * @param[in]  *P_pPrivKey: RSA private key structure (RSAprivKey_stt)
  * @param[in]  *P_pDigest: The message digest that will be signed
  * @param[in]  P_hashType: Identifies the type of Hash function used
  * @param[out] *P_pSignature: The returned message signature
  * @param[in]  *P_pMemBuf: Pointer to the membuf_stt structure
                that will be used to store the internal values
                required by computation. NOT USED
  * @note       P_pSignature has to point to a memory area of suitable size
  *             (modulus size).
  *             Only RSA 1024 and 2048 with SHA1 or SHA256 are supported
  * @retval     RSA_SUCCESS: Operation Successful
  * @retval     RSA_ERR_BAD_PARAMETER: Some of the inputs were NULL
  * @retval     RSA_ERR_UNSUPPORTED_HASH: Hash type passed not supported
  * @retval     ERR_MEMORY_FAIL: Not enough memory left available
  */
int32_t RSA_PKCS1v15_Sign(const RSAprivKey_stt *P_pPrivKey,
                          const uint8_t *P_pDigest,
                          hashType_et P_hashType,
                          uint8_t *P_pSignature,
                          membuf_stt *P_pMemBuf)
{
  int32_t RSA_ret_status = RSA_SUCCESS;
  psa_status_t psa_ret_status;
  psa_algorithm_t psa_algorithm;
  psa_key_handle_t psa_key_handle = 0;
  uint8_t wrap_hash_size;
  uint8_t *pubExponent;
  uint8_t pubExponent1024[3] = {0x01, 0x00, 0x01}; /*e = 65537 for RSA1024*/
  uint8_t pubExponent2048[3] = {0x26, 0x04, 0x45};
  uint8_t DER_privKey[RSA_PRIVKEY_MAXSIZE];
  int32_t wrap_pubExponent_size = 0; /*Bytes*/
  int32_t wrap_signature_lenth = 0;
  uint8_t wrap_ret_status = 0;
  uint32_t length = 0;
  uint32_t wrap_der_size = sizeof(DER_privKey);

  if ((P_pPrivKey == NULL)
      || (P_pDigest == NULL)
      || (P_pSignature == NULL))
  {
    return RSA_ERR_BAD_PARAMETER;
  }
  if (P_pPrivKey->mModulusSize > RSA_PUBKEY_MAXSIZE)
  {
    return RSA_ERR_BAD_PARAMETER;
  }
  /*Define algorithm and hash size with hash type*/
  switch (P_hashType)
  {
    /*Supported Hash*/
    case (E_SHA1):
      wrap_hash_size = WRAP_SHA1_SIZE;
      psa_algorithm = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_1);
      break;
    case (E_SHA256):
      wrap_hash_size = WRAP_SHA256_SIZE;
      psa_algorithm = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);
      break;
    default:
      /*Not supported Hash*/
      RSA_ret_status = RSA_ERR_UNSUPPORTED_HASH;
      break;
  }

  /*Are we doing a 1024 or 2048 RSA ?*/
  switch (P_pPrivKey->mModulusSize)
  {
    case (128):
      /*Set e*/
      pubExponent = pubExponent1024;
      wrap_pubExponent_size = (int32_t) sizeof(pubExponent1024);
      break;

    case (256):
      /*Set e*/
      pubExponent = pubExponent2048;
      wrap_pubExponent_size = (int32_t) sizeof(pubExponent2048);
      break;

    default:
      /*Not Supported*/
      RSA_ret_status = RSA_ERR_BAD_PARAMETER;
      break;
  }
  
  /*Stop if any unsupported hash or algorithm is used*/
  if (RSA_ret_status != RSA_SUCCESS)
  {
    return RSA_ret_status;
  }   
    
  /*Fill a DER structure from a psa key*/
  wrap_ret_status = wrap_keypair_rsa_to_der(DER_privKey,
                                            P_pPrivKey->pmModulus,
                                            P_pPrivKey->pmExponent,
                                            pubExponent,
                                            wrap_der_size,
                                            P_pPrivKey->mModulusSize,
                                            P_pPrivKey->mExponentSize,
                                            wrap_pubExponent_size,
                                            &length);
  if (wrap_ret_status == WRAP_SUCCESS)
  {
    /*The size of DER_privKey is upper than the real size
    and the data are written starting at the end of the buffer*/
    psa_ret_status = wrap_import_der_Key_into_psa(&psa_key_handle,
                                                  PSA_KEY_USAGE_SIGN,
                                                  psa_algorithm,
                                                  PSA_KEY_TYPE_RSA_KEYPAIR,
                                                  &DER_privKey[wrap_der_size - length],
                                                  length);
    if (psa_ret_status == PSA_SUCCESS)
    {
      /*Sign*/
      psa_ret_status = psa_asymmetric_sign(psa_key_handle,
                                           psa_algorithm,
                                           P_pDigest,
                                           wrap_hash_size,
                                           P_pSignature,
                                           (uint32_t) P_pPrivKey->mModulusSize,
                                           (size_t *)&wrap_signature_lenth);

      if (psa_ret_status == PSA_ERROR_INSUFFICIENT_MEMORY)
      {
        RSA_ret_status =  ERR_MEMORY_FAIL;
      }
      else if (psa_ret_status == PSA_SUCCESS)
      {
        RSA_ret_status =  RSA_SUCCESS;
      }
      /* In case of other return status*/
      else
      {
        RSA_ret_status = RSA_ERR_BAD_PARAMETER;
      }

      /*We won't be able to reuse the key, so we destroy it*/
      psa_ret_status = psa_destroy_key(psa_key_handle);
      if (psa_ret_status != PSA_SUCCESS)
      {
        return RSA_ERR_BAD_PARAMETER;
      }
    }
    else
    {
      RSA_ret_status = RSA_ERR_BAD_PARAMETER;
    }
  }
  else if (wrap_ret_status == WRAP_BAD_KEY)
  {
    RSA_ret_status = RSA_ERR_BAD_KEY;
  }
  else
  {
    RSA_ret_status = RSA_ERR_BAD_PARAMETER;
  }
  return RSA_ret_status;
}

/**
  * @brief     PKCS#1v1.5 RSA Signature Verification Function
  * @param[in] *P_pPubKey: RSA public key structure (RSApubKey_stt)
  * @param[in] *P_pDigest: The hash digest of the message to be verified
  * @param[in] P_hashType: Identifies the type of Hash function used
  * @param[in] *P_pSignature: The signature that will be checked
  * @param[in] *P_pMemBuf: Pointer to the membuf_stt structure that will be used
  *            to store the internal values required by computation. NOT USED
  * @retval    SIGNATURE_VALID: The Signature is valid
  * @retval    SIGNATURE_INVALID: The Signature is NOT valid
  * @retval    RSA_ERR_BAD_PARAMETER: Some of the inputs were NULL
  * @retval    RSA_ERR_UNSUPPORTED_HASH: The Hash type passed doesn't correspond
  *            to any among the supported ones
  * @retval    ERR_MEMORY_FAIL: Not enough memory left available
  */
int32_t RSA_PKCS1v15_Verify(const RSApubKey_stt *P_pPubKey,
                            const uint8_t *P_pDigest,
                            hashType_et P_hashType,
                            const uint8_t *P_pSignature,
                            membuf_stt *P_pMemBuf)
{
  int32_t RSA_ret_status = RSA_SUCCESS;
  uint32_t pwrap_der_size = 0;
  psa_status_t psa_ret_status;
  psa_status_t psa_ret_status_tp;
  psa_algorithm_t psa_algorithm;
  psa_key_handle_t psa_key_handle = 0;
  uint8_t wrap_ret_status;
  uint8_t wrap_hash_size;
  uint8_t DER_pubKey[RSA_PUBKEY_MAXSIZE];

  if ((P_pPubKey == NULL)
      || (P_pDigest == NULL)
      || (P_pSignature == NULL))
  {
    return RSA_ERR_BAD_PARAMETER;
  }
  if (P_pPubKey->mModulusSize > RSA_PUBKEY_MAXSIZE)
  {
    return RSA_ERR_BAD_PARAMETER;
  }

  switch (P_hashType)
  {
    /*Supported Hash*/
    case (E_SHA1):
      wrap_hash_size = WRAP_SHA1_SIZE;
      psa_algorithm = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_1);
      break;
    case (E_SHA256):
      wrap_hash_size = WRAP_SHA256_SIZE;
      psa_algorithm = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);
      break;
    default:
      /*Not supported Hash*/
      RSA_ret_status = RSA_ERR_UNSUPPORTED_HASH;
      break;
  }
  
  /*Stop program if we have an unsupported hash*/
  if (RSA_ret_status == RSA_ERR_UNSUPPORTED_HASH)
  {
    return RSA_ret_status;
  }
  
  /*Change the key to a DER format*/
  wrap_ret_status = wrap_pubkey_rsa_to_der(DER_pubKey,
                                           P_pPubKey->pmModulus,
                                           P_pPubKey->pmExponent,
                                           &pwrap_der_size,
                                           P_pPubKey->mModulusSize,
                                           P_pPubKey->mExponentSize);
  if (wrap_ret_status == WRAP_SUCCESS)
  {
    /*Import the key into psa*/
    psa_ret_status = wrap_import_der_Key_into_psa(&psa_key_handle,
                                                  PSA_KEY_USAGE_VERIFY,
                                                  psa_algorithm,
                                                  PSA_KEY_TYPE_RSA_PUBLIC_KEY,
                                                  DER_pubKey,
                                                  pwrap_der_size);
    if (psa_ret_status == RSA_SUCCESS)
    {
      /*Verify*/
      psa_ret_status = psa_asymmetric_verify(psa_key_handle,
                                             psa_algorithm,
                                             P_pDigest,
                                             wrap_hash_size,
                                             P_pSignature,
                                             (uint32_t) P_pPubKey->mModulusSize);

      /*We won't be able to reuse the key, so we destroy it*/
      psa_ret_status_tp = psa_destroy_key(psa_key_handle);
      if (psa_ret_status_tp == PSA_SUCCESS)
      {
        if (psa_ret_status == PSA_ERROR_INVALID_SIGNATURE)
        {
          RSA_ret_status = SIGNATURE_INVALID;
        }
        else if (psa_ret_status == PSA_ERROR_INSUFFICIENT_MEMORY)
        {
          RSA_ret_status = ERR_MEMORY_FAIL;
        }
        else if (psa_ret_status == PSA_ERROR_INVALID_ARGUMENT)
        {
          RSA_ret_status = RSA_ERR_BAD_PARAMETER;
        }
        /*Success*/
        else if (psa_ret_status == PSA_SUCCESS)
        {
          RSA_ret_status =  SIGNATURE_VALID;
        }
        /* In case of other return status*/
        else
        {
          RSA_ret_status = RSA_ERR_BAD_PARAMETER;
        }
      }
      else
      {
        RSA_ret_status = RSA_ERR_BAD_PARAMETER;
      }
    }
    else
    {
      RSA_ret_status = RSA_ERR_BAD_PARAMETER;
    }
  }
  else
  {
    RSA_ret_status = RSA_ERR_BAD_PARAMETER;
  }
  return RSA_ret_status;
}
#endif /*WRAP_RSA*/

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
