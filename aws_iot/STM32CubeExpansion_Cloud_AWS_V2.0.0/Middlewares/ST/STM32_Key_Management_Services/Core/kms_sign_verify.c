/**
  ******************************************************************************
  * @file    kms_sign_verify.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
  *          module sign and verify functionalities.
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
#include "kms_sign_verify.h"
#include "crypto.h"                     /* Crypto services */
#include "kms_objects.h"                /* KMS objects services */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

#ifdef KMS_SIGN_VERIFY

/** @addtogroup KMS_SIGN Sign and Verify services
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @addtogroup KMS_SIGN_Private_Variables Private Variables
  * @{
  */
#if defined(KMS_RSA) || defined(KMS_ECDSA)
static uint8_t preallocated_buffer[4096];     /*!< Working buffer */
#else
extern uint8_t preallocated_buffer[2048];     /*!< Working buffer */
#endif /* defined(KMS_RSA) || defined(KMS_ECDSA) */


#ifdef KMS_RSA

#define KMS_PRIV_EXP_MAX_SIZE 260U    /*!< Private key exponent maximum size */
#define KMS_PUB_EXP_MAX_SIZE  260U    /*!< Public key exponent maximum size */
#define KMS_MODULUS_MAX_SIZE  260U    /*!< Key modulus maximum size */

static uint8_t mb_exponent[KMS_PRIV_EXP_MAX_SIZE];  /*!< Used to extract exponent from blob */
static uint8_t mb_modulus[KMS_MODULUS_MAX_SIZE];    /*!< Used to extract modulus from blob */

#endif /* KMS_RSA */

/**
  * @}
  */

/** @addtogroup KMS_SIGN_Private_Functions Private Functions
  * @{
  */

/* Private function prototypes -----------------------------------------------*/
#ifdef KMS_ECDSA
static CK_RV     load_curve(kms_ref_t *ec_Param, EC_stt *EC_st);
#endif /* KMS_ECDSA */

/* Private function ----------------------------------------------------------*/
#ifdef KMS_ECDSA

/**
  * @brief  Load the Elliptic Curve matching the CKA_EC_PARAMS
  * @note   CKA_EC_PARAMS is expected in DER format
  * @param  ec_Param CKA_EC_PARAMS to look for match
  * @param  EC_st Elliptic curve
  * @retval CKR_OK if found, CKR_GENERAL_ERROR otherwise
  */
static CK_RV     load_curve(kms_ref_t *ec_Param, EC_stt *EC_st)
{
  static const uint8_t P_192_a[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC
  };

  /* coefficient b */
  static const uint8_t P_192_b[] =
  {
    0x64, 0x21, 0x05, 0x19, 0xE5, 0x9C, 0x80, 0xE7, 0x0F, 0xA7, 0xE9, 0xAB, 0x72,
    0x24, 0x30, 0x49, 0xFE, 0xB8, 0xDE, 0xEC, 0xC1, 0x46, 0xB9, 0xB1
  };

  /* prime modulus p*/
  static const uint8_t P_192_p[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  };

  /* order n*/
  static const uint8_t P_192_n[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x99,
    0xDE, 0xF8, 0x36, 0x14, 0x6B, 0xC9, 0xB1, 0xB4, 0xD2, 0x28, 0x31
  };

  /* base point Gx*/
  static const uint8_t P_192_Gx[] =
  {
    0x18, 0x8D, 0xA8, 0x0E, 0xB0, 0x30, 0x90, 0xF6, 0x7C, 0xBF, 0x20, 0xEB, 0x43,
    0xA1, 0x88, 0x00, 0xF4, 0xFF, 0x0A, 0xFD, 0x82, 0xFF, 0x10, 0x12
  };

  /* base point Gy*/
  static const uint8_t P_192_Gy[] =
  {
    0x07, 0x19, 0x2B, 0x95, 0xFF, 0xC8, 0xDA, 0x78, 0x63, 0x10, 0x11, 0xED, 0x6B,
    0x24, 0xCD, 0xD5, 0x73, 0xF9, 0x77, 0xA1, 0x1E, 0x79, 0x48, 0x11
  };


  static const uint8_t P_256_a[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC
  };
  /* coefficient b */
  static const uint8_t P_256_b[] =
  {
    0x5a, 0xc6, 0x35, 0xd8, 0xaa, 0x3a, 0x93, 0xe7, 0xb3, 0xeb, 0xbd, 0x55, 0x76,
    0x98, 0x86, 0xbc, 0x65, 0x1d, 0x06, 0xb0, 0xcc, 0x53, 0xb0, 0xf6, 0x3b, 0xce,
    0x3c, 0x3e, 0x27, 0xd2, 0x60, 0x4b
  };

  /* prime modulus p*/
  static const uint8_t P_256_p[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
  };

  /* order n*/
  static const uint8_t P_256_n[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17, 0x9E, 0x84, 0xF3, 0xB9,
    0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51
  };

  /* base point Gx*/
  static const uint8_t P_256_Gx[] =
  {
    0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47, 0xF8, 0xBC, 0xE6, 0xE5, 0x63,
    0xA4, 0x40, 0xF2, 0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33, 0xA0, 0xF4, 0xA1,
    0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96
  };

  /* base point Gy*/
  static const uint8_t P_256_Gy[] =
  {
    0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B, 0x8E, 0xE7, 0xEB, 0x4A, 0x7C,
    0x0F, 0x9E, 0x16, 0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE, 0xCB, 0xB6,
    0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5
  };


  static const uint8_t P_384_a[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFC
  };

  /* coefficient b */
  static const uint8_t P_384_b[] =
  {
    0xb3, 0x31, 0x2f, 0xa7, 0xe2, 0x3e, 0xe7, 0xe4, 0x98, 0x8e, 0x05, 0x6b, 0xe3,
    0xf8, 0x2d, 0x19, 0x18, 0x1d, 0x9c, 0x6e, 0xfe, 0x81, 0x41, 0x12, 0x03, 0x14,
    0x08, 0x8f, 0x50, 0x13, 0x87, 0x5a, 0xc6, 0x56, 0x39, 0x8d, 0x8a, 0x2e, 0xd1,
    0x9d, 0x2a, 0x85, 0xc8, 0xed, 0xd3, 0xec, 0x2a, 0xef
  };

  /* prime modulus p*/
  static const uint8_t P_384_p[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF
  };
  /* order n*/
  static const uint8_t P_384_n[] =
  {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0x63,
    0x4D, 0x81, 0xF4, 0x37, 0x2D, 0xDF, 0x58, 0x1A, 0x0D, 0xB2, 0x48, 0xB0, 0xA7,
    0x7A, 0xEC, 0xEC, 0x19, 0x6A, 0xCC, 0xC5, 0x29, 0x73
  };

  /* base point Gx*/
  static const uint8_t P_384_Gx[] =
  {
    0xAA, 0x87, 0xCA, 0x22, 0xBE, 0x8B, 0x05, 0x37, 0x8E, 0xB1, 0xC7, 0x1E, 0xF3,
    0x20, 0xAD, 0x74, 0x6E, 0x1D, 0x3B, 0x62, 0x8B, 0xA7, 0x9B, 0x98, 0x59, 0xF7,
    0x41, 0xE0, 0x82, 0x54, 0x2A, 0x38, 0x55, 0x02, 0xF2, 0x5D, 0xBF, 0x55, 0x29,
    0x6C, 0x3A, 0x54, 0x5E, 0x38, 0x72, 0x76, 0x0A, 0xB7
  };

  /* base point Gy*/
  static const uint8_t P_384_Gy[] =
  {
    0x36, 0x17, 0xDE, 0x4A, 0x96, 0x26, 0x2C, 0x6F, 0x5D, 0x9E, 0x98, 0xBF, 0x92,
    0x92, 0xDC, 0x29, 0xF8, 0xF4, 0x1D, 0xBD, 0x28, 0x9A, 0x14, 0x7C, 0xE9, 0xDA,
    0x31, 0x13, 0xB5, 0xF0, 0xB8, 0xC0, 0x0A, 0x60, 0xB1, 0xCE, 0x1D, 0x7E, 0x81,
    0x9D, 0x7A, 0x43, 0x1D, 0x7C, 0x90, 0xEA, 0x0E, 0x5F
  };

  static uint8_t ref_secp192r1[] = {0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x01};
  static uint8_t ref_secp256r1[] = {0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07};
  static uint8_t ref_secp384r1[] = {0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22};

  if ((ec_Param != NULL) && (ec_Param->size > 0UL) && (EC_st != NULL))
  {

    uint8_t   u8ParamAttrib[32];

    /* Read value from the structure. Need to be translated from
    (uint32_t*) to (uint8_t *) */
    KMS_Objects_TranslateRsaAttributes(ec_Param, u8ParamAttrib);

    /* CKA_EC_PARAMS is DER encoded OIDs for EC supported curves
           secp192r1 = '06082a8648ce3d030101'x          {1 2 840 10045 3 1 1}
           secp256r1 = '06082a8648ce3d030107'x          {1 2 840 10045 3 1 7}
           secp384r1 = '06052b81040022'x                {1 3 132 0 34}        */

    /* secp192r1 */
    if (memcmp(u8ParamAttrib, ref_secp192r1, ec_Param->size) == 0)
    {
      EC_st->pmA = P_192_a;
      EC_st->pmB = P_192_b;
      EC_st->pmP = P_192_p;
      EC_st->pmN = P_192_n;
      EC_st->pmGx = P_192_Gx;
      EC_st->pmGy = P_192_Gy;
      EC_st->mAsize = (int32_t)sizeof(P_192_a);
      EC_st->mBsize = (int32_t)sizeof(P_192_b);
      EC_st->mNsize = (int32_t)sizeof(P_192_n);
      EC_st->mPsize = (int32_t)sizeof(P_192_p);
      EC_st->mGxsize = (int32_t)sizeof(P_192_Gx);
      EC_st->mGysize = (int32_t)sizeof(P_192_Gy);

      return CKR_OK;
    }

    /* secp256r1 */
    if (memcmp(u8ParamAttrib, ref_secp256r1, ec_Param->size) == 0)
    {
      EC_st->pmA = P_256_a;
      EC_st->pmB = P_256_b;
      EC_st->pmP = P_256_p;
      EC_st->pmN = P_256_n;
      EC_st->pmGx = P_256_Gx;
      EC_st->pmGy = P_256_Gy;
      EC_st->mAsize = (int32_t)sizeof(P_256_a);
      EC_st->mBsize = (int32_t)sizeof(P_256_b);
      EC_st->mNsize = (int32_t)sizeof(P_256_n);
      EC_st->mPsize = (int32_t)sizeof(P_256_p);
      EC_st->mGxsize = (int32_t)sizeof(P_256_Gx);
      EC_st->mGysize = (int32_t)sizeof(P_256_Gy);

      return CKR_OK;

    }

    /* secp384r1 */
    if (memcmp(u8ParamAttrib, ref_secp384r1, ec_Param->size) == 0)
    {
      EC_st->pmA = P_384_a;
      EC_st->pmB = P_384_b;
      EC_st->pmP = P_384_p;
      EC_st->pmN = P_384_n;
      EC_st->pmGx = P_384_Gx;
      EC_st->pmGy = P_384_Gy;
      EC_st->mAsize = (int32_t)sizeof(P_384_a);
      EC_st->mBsize = (int32_t)sizeof(P_384_b);
      EC_st->mNsize = (int32_t)sizeof(P_384_n);
      EC_st->mPsize = (int32_t)sizeof(P_384_p);
      EC_st->mGxsize = (int32_t)sizeof(P_384_Gx);
      EC_st->mGysize = (int32_t)sizeof(P_384_Gy);

      return CKR_OK;

    }

  }
  return CKR_GENERAL_ERROR;
}

#endif /* KMS_ECDSA */


/**
  * @}
  */

/** @addtogroup KMS_SIGN_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This function is called upon @ref C_SignInit call
  * @note   Refer to @ref C_SignInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pMechanism signature mechanism
  * @param  hKey signature key
  * @retval Operation status
  */
CK_RV     KMS_SignInit(CK_SESSION_HANDLE hSession,
                       CK_MECHANISM_PTR  pMechanism,
                       CK_OBJECT_HANDLE  hKey)
{
  /* Check that we support the expected mechanism. */
#ifdef KMS_RSA
  if ((pMechanism->mechanism == CKM_RSA_PKCS) ||
      (pMechanism->mechanism == CKM_SHA1_RSA_PKCS) ||
      (pMechanism->mechanism == CKM_SHA256_RSA_PKCS))
  {

    /* Check session handle */
    if (KMS_Check_Session_Handle(hSession) != CKR_OK)
    {
      return CKR_SESSION_HANDLE_INVALID;
    }


    /* Check that session not already used for another processing ! */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }
  }
#endif /* KMS_RSA */

#ifdef KMS_ECDSA
  if ((pMechanism->mechanism == CKM_ECDSA_SHA1) ||
      (pMechanism->mechanism == CKM_ECDSA_SHA256))
  {

    /* Check session handle */
    if (KMS_Check_Session_Handle(hSession) != CKR_OK)
    {
      return CKR_SESSION_HANDLE_INVALID;
    }


    /* Check that session not already used for another processing ! */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }
  }
#endif /* KMS_ECDSA */

  return CKR_OK;
}


/**
  * @brief  This function is called upon @ref C_Sign call
  * @note   Refer to @ref C_Sign function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pData data to sign
  * @param  ulDataLen data to sign length
  * @param  pSignature signature
  * @param  pulSignatureLen signature length
  * @retval Operation status
  */
CK_RV     KMS_Sign(CK_SESSION_HANDLE hSession,         /* the session's handle */
                   CK_BYTE_PTR       pData,           /* the data to sign */
                   CK_ULONG          ulDataLen,       /* count of bytes to sign */
                   CK_BYTE_PTR       pSignature,      /* gets the signature */
                   CK_ULONG_PTR      pulSignatureLen)  /* gets signature length */
{

  CK_RV e_ret_status;
#ifdef KMS_RSA
  int32_t cryptolib_status = HASH_SUCCESS;    /* CryptoLib Error Status */
  hashType_et selected_hash = E_MD5;          /* Initialization at the first value of enum to avoid warnings */   
  RSAprivKey_stt PrivKey_st; /* Structure that will contain the private key*/
  kms_obj_keyhead_t *pkms_object;
  uint8_t Digest[CRL_SHA512_SIZE]; /* Size of the biggest hash result */
  int32_t outputSize;
  /* structure that will contain the preallocated buffer */
  membuf_stt mb_st;
  kms_ref_t *P_pKeyPrivExponentAttribute, *P_pKeyModulusAttribute;

  /* Check that we are not pointing to a session not allowed */
  if (KMS_Check_Session_Handle(hSession) != CKR_OK)
  {
    /* A C_Sign operation consider a Single part operation. As the C_SignFinal */
    /*  will not be called, we can consider that the key usage is finished. */
    KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;

    return CKR_SESSION_HANDLE_INVALID;
  }

  /* If a digest has to be computed */
  switch (KMS_GETSESSSION(hSession).Mechanism)
  {
    case CKM_MD5_RSA_PKCS:
      /* ERROR ==> not supported yet */
      cryptolib_status = RSA_ERR_UNSUPPORTED_HASH;
      break;
    case CKM_SHA1_RSA_PKCS:
    case CKM_ECDSA_SHA1:
    {
      /* The HASH context (largest one) */
      HASHctx_stt HASH_ctxt_st;

      HASH_ctxt_st.mFlags = E_HASH_DEFAULT;
      HASH_ctxt_st.mTagSize = CRL_SHA1_SIZE;

      selected_hash = E_SHA1;

      cryptolib_status = SHA1_Init(&HASH_ctxt_st);

      /* check for initialization errors */
      if (cryptolib_status == HASH_SUCCESS)
      {
        /* Add data to be hashed */
        cryptolib_status = SHA1_Append(&HASH_ctxt_st, pData, (int32_t)ulDataLen);

        if (cryptolib_status == HASH_SUCCESS)
        {
          /* retrieve */
          cryptolib_status = SHA1_Finish(&HASH_ctxt_st, Digest, &outputSize);
        }
      }
      break;
    }
    case CKM_SHA256_RSA_PKCS:
    case CKM_ECDSA_SHA256:
    {
      /* The HASH context (largest one) */
      SHA256ctx_stt HASH_ctxt_st;

      HASH_ctxt_st.mFlags = E_HASH_DEFAULT;
      HASH_ctxt_st.mTagSize = CRL_SHA256_SIZE;

      selected_hash = E_SHA256;

      cryptolib_status = SHA256_Init(&HASH_ctxt_st);

      /* check for initialization errors */
      if (cryptolib_status == HASH_SUCCESS)
      {
        /* Add data to be hashed */
        cryptolib_status = SHA256_Append(&HASH_ctxt_st, pData, (int32_t)ulDataLen);

        if (cryptolib_status == HASH_SUCCESS)
        {
          /* retrieve */
          cryptolib_status = SHA256_Finish(&HASH_ctxt_st, Digest, &outputSize);
        }
      }
      break;
    }
    case CKM_SHA384_RSA_PKCS:
      /* ERROR ==> not supported yet */
      cryptolib_status = RSA_ERR_UNSUPPORTED_HASH;
      break;

    case CKM_SHA512_RSA_PKCS:
      /* ERROR ==> not supported yet */
      cryptolib_status = RSA_ERR_UNSUPPORTED_HASH;
      break;
    case CKM_RSA_PKCS:
      /* hash_calculated = FALSE; */
      break;

    default:
      cryptolib_status = RSA_ERR_UNSUPPORTED_HASH;
      break;
  }

  /* if a HASH was requested, but not well supported, */
  /* Do not proceed the signature */
  if (cryptolib_status != HASH_SUCCESS)
  {
    /* A C_Sign operation consider a Single part operation. As the C_SignFinal */
    /*  will not be called, we can consider that the key usage is finished. */
    KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;

    return CKR_FUNCTION_FAILED;
  }

  /* Read the key value from the Key Handle                 */
  /* Key Handle is the index to one of static or nvm        */
  pkms_object = KMS_Objects_GetPointer(KMS_GETSESSSION(hSession).hKey);

  /* Check that hKey is valid */
  if (pkms_object != NULL)
  {
    /* A C_Sign operation consider a Single part operation. As the C_SignFinal */
    /*  will not be called, we can consider that the key usage is finished. */
    KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;


    /* PKCS11 spec: To Be Added ...
        The CKA_SIGN attribute of the signature key, which indicates whether the key supports signatures with
        appendix, MUST be CK_TRUE.
    */

    e_ret_status = KMS_Objects_SearchAttributes(CKA_PRIVATE_EXPONENT, pkms_object, &P_pKeyPrivExponentAttribute);

    if ((e_ret_status == CKR_OK) &&
        (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
        (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
    {
      PrivKey_st.mExponentSize = (int32_t)P_pKeyPrivExponentAttribute->size;

      if (P_pKeyPrivExponentAttribute->size > KMS_PRIV_EXP_MAX_SIZE)
      {
        return CKR_KEY_SIZE_RANGE;
      }

      /* Because au Indianess, and as edition of the text file is easier,
         we have to translate u32 to u8, thanks to KMS_Objects_TranslateKey. */

      /* Read value from the structure. Need to be translated from
         (uint32_t*) to (uint8_t *) */
      KMS_Objects_TranslateRsaAttributes(P_pKeyPrivExponentAttribute, mb_exponent);
      PrivKey_st.pmExponent = (uint8_t *)mb_exponent;

      e_ret_status = KMS_Objects_SearchAttributes(CKA_MODULUS, pkms_object, &P_pKeyModulusAttribute);

      if (e_ret_status == CKR_OK)
      {
        if (P_pKeyModulusAttribute->size > KMS_MODULUS_MAX_SIZE)
        {
          return CKR_KEY_SIZE_RANGE;
        }

        PrivKey_st.mModulusSize = (int32_t)P_pKeyModulusAttribute->size;

        /* Because au Indianess, and as edition of the text file is easier,
          we have to translate u32 to u8, thanks to KMS_Objects_TranslateKey. */

        /* Read value from the structure. Need to be translated from
          (uint32_t*) to (uint8_t *) */
        KMS_Objects_TranslateRsaAttributes(P_pKeyModulusAttribute, mb_modulus);

        PrivKey_st.pmModulus = (uint8_t *)mb_modulus;

        /* Initialize the membuf_st that must be passed to the RSA functions */
        mb_st.mSize = (int32_t)sizeof(preallocated_buffer);
        mb_st.mUsed = 0;
        mb_st.pmBuf = preallocated_buffer;

        /* Sign with RSA */
        cryptolib_status = RSA_PKCS1v15_Sign(&PrivKey_st, Digest, selected_hash, pSignature, &mb_st);

        if (cryptolib_status == RSA_SUCCESS)
        {
          *pulSignatureLen = (uint32_t)PrivKey_st.mModulusSize;
        }
      }
      else  /* Attribute CKA_MODULUS not found */
      {
        return CKR_MECHANISM_PARAM_INVALID;
      }

    }
    else  /* Attribute CKA_PRIVATE_EXPONENT not found */
    {
      return CKR_MECHANISM_PARAM_INVALID;
    }
  }

  if (cryptolib_status == AES_SUCCESS)
  {
    e_ret_status = CKR_OK;
  }
  else
  {
    e_ret_status = CKR_FUNCTION_FAILED;
  }
#else /* KMS_RSA */
  e_ret_status = CKR_FUNCTION_FAILED;
#endif /* KMS_RSA */

  return e_ret_status;
}


/**
  * @brief  This function is called upon @ref C_VerifyInit call
  * @note   Refer to @ref C_VerifyInit function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pMechanism verification mechanism
  * @param  hKey verification key
  * @retval Operation status
  */
CK_RV        KMS_VerifyInit(CK_SESSION_HANDLE hSession,
                            CK_MECHANISM_PTR  pMechanism,
                            CK_OBJECT_HANDLE  hKey)
{

  /* Check that we support the expected mechanism. */
#ifdef KMS_RSA
  if ((pMechanism->mechanism == CKM_SHA1_RSA_PKCS) ||
      (pMechanism->mechanism == CKM_SHA256_RSA_PKCS))
  {

    /* Check session handle */
    if (KMS_Check_Session_Handle(hSession) != CKR_OK)
    {
      return CKR_SESSION_HANDLE_INVALID;

    }


    /* Check that session not already used for another processing ! */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }
  }
#endif /* KMS_RSA */

#ifdef KMS_ECDSA
  if ((pMechanism->mechanism == CKM_ECDSA_SHA1) ||
      (pMechanism->mechanism == CKM_ECDSA_SHA256))
  {

    /* Check session handle */
    if (KMS_Check_Session_Handle(hSession) != CKR_OK)
    {
      return CKR_SESSION_HANDLE_INVALID;
    }


    /* Check that session not already used for another processing ! */
    if (KMS_GETSESSSION(hSession).hKey != KMS_HANDLE_KEY_NOT_KNOWN)
    {
      return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }
    else
    {
      KMS_GETSESSSION(hSession).hKey = hKey;
      KMS_GETSESSSION(hSession).Mechanism = pMechanism->mechanism;
    }
  }
#endif /* KMS_ECDSA */

  return CKR_OK;
}


/**
  * @brief  This function is called upon @ref C_Verify call
  * @note   Refer to @ref C_Verify function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pData data to verify
  * @param  ulDataLen data to verify length
  * @param  pSignature signature
  * @param  ulSignatureLen signature length
  * @retval Operation status
  */
CK_RV  KMS_Verify(CK_SESSION_HANDLE hSession,         /* the session's handle */
                  CK_BYTE_PTR       pData,           /* signed data */
                  CK_ULONG          ulDataLen,       /* length of signed data */
                  CK_BYTE_PTR       pSignature,      /* signature */
                  CK_ULONG          ulSignatureLen)  /* signature length */

{
  (void)ulSignatureLen;
  CK_RV    e_ret_status ;
  int32_t cryptolib_status = HASH_SUCCESS;    /* CryptoLib Error Status */
  hashType_et selected_hash = E_MD5;          /* Initialization at the first value of enum list to avoid warnings */   
  RSApubKey_stt PubKey_st; /* Structure that will contain the public key*/
  kms_obj_keyhead_t *pkms_object;
  uint8_t Digest[CRL_SHA512_SIZE]; /* Size of the biggest hash result */
  int32_t DigestSize;
  /* structure that will contain the preallocated buffer */
  membuf_stt mb_st;

  /* Check that we are not pointing to a session not allowed */
  if (KMS_Check_Session_Handle(hSession) != CKR_OK)
  {
    /* A C_Sign operation consider a Single part operation. As the C_SignFinal */
    /*  will not be called, we can consider that the key usage is finished. */
    KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;

    return CKR_SESSION_HANDLE_INVALID;
  }

  /* If a digest has to be computed */
  switch (KMS_GETSESSSION(hSession).Mechanism)
  {
    case CKM_MD5_RSA_PKCS:
      /* ERROR ==> not supported yet */
      cryptolib_status = RSA_ERR_UNSUPPORTED_HASH;
      break;
    case CKM_SHA1_RSA_PKCS:
    case CKM_ECDSA_SHA1:
    {
      /* The HASH context (largest one) */
      HASHctx_stt HASH_ctxt_st;

      HASH_ctxt_st.mFlags = E_HASH_DEFAULT;
      HASH_ctxt_st.mTagSize = CRL_SHA1_SIZE;

      selected_hash = E_SHA1;

      cryptolib_status = SHA1_Init(&HASH_ctxt_st);

      /* check for initialization errors */
      if (cryptolib_status == HASH_SUCCESS)
      {
        /* Add data to be hashed */
        cryptolib_status = SHA1_Append(&HASH_ctxt_st, pData, (int32_t)ulDataLen);

        if (cryptolib_status == HASH_SUCCESS)
        {
          /* retrieve */
          cryptolib_status = SHA1_Finish(&HASH_ctxt_st, Digest, &DigestSize);
        }
      }
      break;
    }
    case CKM_SHA256_RSA_PKCS:
    case CKM_ECDSA_SHA256:
    {
      /* The HASH context (largest one) */
      SHA256ctx_stt HASH_ctxt_st;

      HASH_ctxt_st.mFlags = E_HASH_DEFAULT;
      HASH_ctxt_st.mTagSize = CRL_SHA256_SIZE;

      selected_hash = E_SHA256;

      cryptolib_status = SHA256_Init(&HASH_ctxt_st);

      /* check for initialization errors */
      if (cryptolib_status == HASH_SUCCESS)
      {
        /* Add data to be hashed */
        cryptolib_status = SHA256_Append(&HASH_ctxt_st, pData, (int32_t)ulDataLen);

        if (cryptolib_status == HASH_SUCCESS)
        {
          /* retrieve */
          cryptolib_status = SHA256_Finish(&HASH_ctxt_st, Digest, &DigestSize);
        }
      }
      break;
    }
    case CKM_SHA384_RSA_PKCS:
      /* ERROR ==> not supported yet */
      cryptolib_status = RSA_ERR_UNSUPPORTED_HASH;
      break;

    case CKM_SHA512_RSA_PKCS:
      /* ERROR ==> not supported yet */
      cryptolib_status = RSA_ERR_UNSUPPORTED_HASH;
      break;
    case CKM_RSA_PKCS:
      /*   hash_calculated = FALSE; */
      break;

    default:
      cryptolib_status = RSA_ERR_UNSUPPORTED_HASH;
      break;
  }

  /* if a HASH was requested, but not well supported, */
  /* Do not proceed the signature */
  if (cryptolib_status != HASH_SUCCESS)
  {
    /* A C_Sign operation consider a Single part operation. As the C_SignFinal */
    /*  will not be called, we can consider that the key usage is finished. */
    KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;

    return CKR_FUNCTION_FAILED;
  }

  /* Read the key value from the Key Handle                 */
  /* Key Handle is the index to one of static or nvm        */
  pkms_object = KMS_Objects_GetPointer(KMS_GETSESSSION(hSession).hKey);

  /* Check that hKey is valid */
  if (pkms_object != NULL)
  {
    /* A C_Verify operation consider a Single part operation. As the C_VerifyFinal */
    /*  will not be called, we can consider that the key usage is finished. */
    KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;


    /* ATTRIBUTES FOR RSA AND ECDSA AREE FULLY DIFFERENT */
    if ((KMS_GETSESSSION(hSession).Mechanism == CKM_SHA1_RSA_PKCS) ||
        ((KMS_GETSESSSION(hSession).Mechanism == CKM_SHA256_RSA_PKCS)))
    {

#ifdef KMS_RSA
      kms_ref_t *P_pKeyPubExponentAttribute, *P_pKeyModulusAttribute;

      /* PKCS11 spec: To Be added ...
          The CKA_SIGN attribute of the signature key, which indicates whether the key supports signatures with
          appendix, MUST be CK_TRUE.
      */

      e_ret_status = KMS_Objects_SearchAttributes(CKA_PUBLIC_EXPONENT, pkms_object, &P_pKeyPubExponentAttribute);

      if ((e_ret_status == CKR_OK) &&
          (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
          (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
      {
        PubKey_st.mExponentSize = (int32_t)P_pKeyPubExponentAttribute->size;

        if (P_pKeyPubExponentAttribute->size > KMS_PUB_EXP_MAX_SIZE)
        {
          return CKR_KEY_SIZE_RANGE;
        }

        /* Because au Indianess, and as edition of the text file is easier,
          we have to translate u32 to u8, thanks to KMS_Objects_TranslateKey. */

        if (P_pKeyPubExponentAttribute->size > 3U)
        {
          /* Read value from the structure. For Public exponent, no need to translat */
          KMS_Objects_TranslateRsaAttributes(P_pKeyPubExponentAttribute, mb_exponent);
          PubKey_st.pmExponent = (uint8_t *)mb_exponent;
        }
        else
        {
          PubKey_st.pmExponent = (uint8_t *)P_pKeyPubExponentAttribute->data;
        }

        e_ret_status = KMS_Objects_SearchAttributes(CKA_MODULUS, pkms_object, &P_pKeyModulusAttribute);

        if (e_ret_status == CKR_OK)
        {
          if (P_pKeyModulusAttribute->size > KMS_MODULUS_MAX_SIZE)
          {
            return CKR_KEY_SIZE_RANGE;
          }

          PubKey_st.mModulusSize = (int32_t)P_pKeyModulusAttribute->size;

          /* Because au Indianess, and as edition of the text file is easier,
            we have to translate u32 to u8, thanks to KMS_Objects_TranslateKey. */

          /* Read value from the structure. Need to be translated from
            (uint32_t*) to (uint8_t *) */
          KMS_Objects_TranslateRsaAttributes(P_pKeyModulusAttribute, mb_modulus);

          PubKey_st.pmModulus = (uint8_t *)mb_modulus;

          /* Initialize the membuf_st that must be passed to the RSA functions */
          mb_st.mSize = (int32_t)sizeof(preallocated_buffer);
          mb_st.mUsed = 0;
          mb_st.pmBuf = preallocated_buffer;

          /* Sign with RSA */
          cryptolib_status = RSA_PKCS1v15_Verify(&PubKey_st, Digest, selected_hash, pSignature, &mb_st);

        }
        else  /* Attribute CKA_MODULUS not found */
        {
          return CKR_MECHANISM_PARAM_INVALID;
        }
      }
      else  /* Attribute CKA_PRIVATE_EXPONENT not found */
      {
        return CKR_MECHANISM_PARAM_INVALID;
      }
#endif /* KMS_RSA */

    }


    if ((KMS_GETSESSSION(hSession).Mechanism == CKM_ECDSA_SHA1) ||
        (KMS_GETSESSSION(hSession).Mechanism == CKM_ECDSA_SHA256))
    {


#ifdef KMS_ECDSA
      kms_ref_t *P_pEC_ParamAttribute;
      EC_stt  EC_st;
      /* structure to store the preallocated buffer for computation*/
      membuf_stt Crypto_Buffer;

      /* PKCS11 spec: To Be added ...
          The CKA_EC_PARAMS attribute, specify the Curve to use for the verification */


      /* CKA_EC_PARAMS is DER encoded OIDs for EC supported curves            */
      /*   secp192r1 = '06082a8648ce3d030101'x          {1 2 840 10045 3 1 1} */
      /*   secp256r1 = '06082a8648ce3d030107'x          {1 2 840 10045 3 1 7} */
      /*   secp384r1 = '06052b81040022'x                {1 3 132 0 34}        */

      e_ret_status = KMS_Objects_SearchAttributes(CKA_EC_PARAMS, pkms_object, &P_pEC_ParamAttribute);

      if ((e_ret_status == CKR_OK) &&
          (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
          (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
      {

        /* Load the Elliptic Curve definied in the Object by CKA_EC_PARAMS */
        e_ret_status = load_curve(P_pEC_ParamAttribute, &EC_st);
        if (e_ret_status == CKR_OK)
        {
          /* We prepare the memory buffer strucure */
          Crypto_Buffer.pmBuf =  preallocated_buffer;
          Crypto_Buffer.mUsed = 0;
          Crypto_Buffer.mSize = (int16_t)sizeof(preallocated_buffer);

          /* Init the Elliptic Curve, passing the required memory */
          /* Note: This is not a temporary buffer, the memory inside EC_stt_buf is linked to EC_st *
              As long as there's need to use EC_st the content of EC_stt_buf should not be altered */
          cryptolib_status = ECCinitEC(&EC_st, &Crypto_Buffer);

          if (cryptolib_status == ECC_SUCCESS)
          {
            /* EC is initialized, now prepare to read the public key. First, allocate a point */
            ECpoint_stt *PubKey = NULL;

            cryptolib_status = ECCinitPoint(&PubKey, &EC_st, &Crypto_Buffer);

            if (cryptolib_status == ECC_SUCCESS)
            {
              kms_ref_t *P_pEC_DER_x962_Point;
              uint8_t   pub_x[64];
              uint8_t   pub_y[64];
              ECDSAsignature_stt *sign = NULL;


              /* We read the Public Key value from Object */
              /* The expected format is : */
              e_ret_status = KMS_Objects_SearchAttributes(CKA_EC_POINT, pkms_object, &P_pEC_DER_x962_Point);

              if (e_ret_status == CKR_OK)
              {
                uint8_t   u8ParamAttrib[128];
                int32_t  pub_key_size; /* Size in bytes */

                /* Here, the curve is loaded in EC_st */
                pub_key_size = EC_st.mNsize;

                /* Read value from the structure. Need to be translated from
                    (uint32_t*) to (uint8_t *) */
                KMS_Objects_TranslateRsaAttributes(P_pEC_DER_x962_Point, u8ParamAttrib);


                /* We expect a DER format, with first byte= 04, then the X, then the Y */
                /* TBD: This is not the final version, as the value is suppose */
                /*      to be of form: x962 + DER */
                if (u8ParamAttrib[0] == 0x04U)
                {
                  /* ASN.1 encoding:
                   * (T,L,V): <1 byte Tage type> <1 or more bytes length><x data>
                   * 0x04: Type Octet String
                   * 1 byte length if inferior to 128 bytes
                   * more bytes length otherwise: first byte is 0x80 + number of bytes that will encode length
                   * ex: Length = 0x23 => 1 byte length: 0x23
                   *        Length = 0x89 => 2 bytes length: 0x81 0x89
                   *        Length = 0x123 => 3 bytes length: 0x82 0x01 0x23
                   */
                  uint8_t offset_for_data = 1;
                  if ((u8ParamAttrib[1] & 0x80U) == 0x80U)
                  {
                    offset_for_data = (u8ParamAttrib[1] & 0x7FU) + 1U;
                  }
                  /* Copy the Pub_x */
                  (void)memcpy(&pub_x[0], &u8ParamAttrib[1U + offset_for_data], (size_t)pub_key_size);

                  /* Copy the Pub_y */
                  (void)memcpy(&pub_y[0],
                               &u8ParamAttrib[1UL + (uint32_t)offset_for_data + (uint32_t)pub_key_size],
                               (size_t)pub_key_size);

                  /* Point is initialized, now import the public key */
                  (void)ECCsetPointCoordinate(PubKey, E_ECC_POINT_COORDINATE_X, pub_x, pub_key_size);
                  (void)ECCsetPointCoordinate(PubKey, E_ECC_POINT_COORDINATE_Y, pub_y, pub_key_size);

                  /* Try to validate the Public Key. */
                  cryptolib_status = ECCvalidatePubKey(PubKey, &EC_st, &Crypto_Buffer);
                  if (cryptolib_status == ECC_SUCCESS)
                  {
                    /* Public Key is validated, Initialize the signature object */
                    cryptolib_status = ECDSAinitSign(&sign, &EC_st, &Crypto_Buffer);

                    if (cryptolib_status == ECC_SUCCESS)
                    {
                      ECDSAverifyCtx_stt verctx;

                      /* PRepare the signature values to CryptoLib format */

                      (void)ECDSAsetSignature(sign, E_ECDSA_SIGNATURE_R_VALUE, pSignature, pub_key_size);
                      (void)ECDSAsetSignature(sign, E_ECDSA_SIGNATURE_S_VALUE, &pSignature[pub_key_size], pub_key_size);

                      /* Prepare the structure for the ECDSA signature verification */
                      verctx.pmEC = &EC_st;
                      verctx.pmPubKey = PubKey;

                      /* Verify it */
                      cryptolib_status = ECDSAverify(Digest, DigestSize, sign, &verctx, &Crypto_Buffer);

                      /* release ressource ...*/
                      (void)ECDSAfreeSign(&sign, &Crypto_Buffer);
                      (void)ECCfreePoint(&PubKey, &Crypto_Buffer);
                      (void)ECCfreeEC(&EC_st, &Crypto_Buffer);
                    }
                  }
                }
              }
            }
          }

        }
        else
        {
          return ECC_ERR_BAD_OPERATION;
        }
      }
    }
#endif /* KMS_ECDSA */

  }


  if (cryptolib_status == SIGNATURE_VALID)
  {
    e_ret_status = CKR_OK;
  }
  else
  {
    e_ret_status = CKR_FUNCTION_FAILED;
  }

  return e_ret_status;



}





/**
  * @}
  */

/**
  * @}
  */

#endif /* KMS_SIGN_VERIFY */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
