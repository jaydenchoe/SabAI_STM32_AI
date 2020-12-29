/**
  ******************************************************************************
  * @file    kms_objects.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
  *          module object manipulation services.
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
#include "kms_objects.h"

#include "kms_nvm_storage.h"            /* KMS storage services */
#include "kms_platf_objects.h"          /* KMS platform objects services */
#include "kms_low_level.h"              /* Flash access to read blob */
#include "kms_blob_metadata.h"          /* Blob header defintions */

#include "kms_digest.h"                 /* SHA services for Blob verification */
#include "kms_sign_verify.h"            /* Verify  services for Blob authentication */
#include "kms_aes.h"                    /* AES  services for Blob decryption */

#include <string.h>


/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_OBJECTS Blob Objects Management
  * @{
  */


/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup KMS_OBJECTS_Private_Defines Private Defines
  * @{
  */
#define KMS_BLOB_CHUNK_SIZE  512UL    /*!< Blob import working chunk size */
#define KMS_MAX_LOCK_KEY_HANDLE 10   /*!< Max number of lockable keys    */

/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static CK_OBJECT_HANDLE kms_locked_key_handles[KMS_MAX_LOCK_KEY_HANDLE];
static uint32_t         kms_locked_key_index=0;

/* Private function prototypes -----------------------------------------------*/
static kms_import_blob_state_t authenticate_blob_header(KMS_BlobRawHeaderTypeDef *pBlobHeader);
static kms_import_blob_state_t authenticate_blob(KMS_BlobRawHeaderTypeDef *pBlobHeader);
static kms_import_blob_state_t install_blob(KMS_BlobRawHeaderTypeDef *pBlobHeader);
static CK_RV  read_next_chunk(uint32_t session, uint8_t *p_source_address, uint32_t size, uint8_t *p_decrypted_chunk,
                              uint32_t *p_decrypted_size);

/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_OBJECTS_Exported_Functions Exported Functions
  * @{
  */

/* This function is the entry point to the objects contained either in the static definition or nvms area. */

/**
  * @brief  This function returns object pointer from key handle
  * @param  hKey key handle
  * @retval NULL if not found, Object pointer otherwise
  */
kms_obj_keyhead_t *KMS_Objects_GetPointer(CK_OBJECT_HANDLE hKey)
{
  uint32_t MinSlot;
  uint32_t MaxSlot;
  kms_obj_keyhead_t *pObject = NULL;

  /* Check that the key has not been Locked */
  if (KMS_CheckKeyIsNotLocked(hKey) == CKR_OK)
  {
    
    /* Read the available static slots from the platform */
    KMS_PlatfObjects_EmbeddedRange(&MinSlot, &MaxSlot);
    /* If hKey is in the range of the embedded keys */
    if ((hKey <= MaxSlot) && (hKey >= MinSlot))
    {
      pObject = KMS_PlatfObjects_EmbeddedObject(hKey - MinSlot);
    }

#ifdef KMS_NVM_ENABLED
    /* Read the available nvm slots from the platform */
    KMS_PlatfObjects_NvmStaticRange(&MinSlot, &MaxSlot);
    /* If hKey is in the range of nvm keys */
    if ((hKey <= MaxSlot) && (hKey >= MinSlot))
    {
      pObject = KMS_PlatfObjects_NvmStaticObject(hKey - MinSlot);
    }

#ifdef KMS_NVM_DYNAMIC_ENABLED

    /* Read the available nvm slots from the platform */
    KMS_PlatfObjects_NvmDynamicRange(&MinSlot, &MaxSlot);
    /* If hKey is in the range of nvm keys */
    if ((hKey <= MaxSlot) && (hKey >= MinSlot))
    {
      pObject = KMS_PlatfObjects_NvmDynamicObject(hKey - MinSlot);
    }
#endif  /* KMS_NVM_DYNAMIC_ENABLED */
#endif  /* KMS_NVM_ENABLED */
  }

  /* Double Check to avoid basic fault injection : Check that the key has not been Locked */
  if (KMS_CheckKeyIsNotLocked(hKey) == CKR_OK)
  {
    return pObject;
  }
  else
  {
    /* hKey not in embedded nor in nvm, or Locked */
    return NULL;
  }
}

/**
  * @brief  This function returns object range identification from key handle
  * @param  hKey key handle
  * @retval Value within @ref kms_obj_range_t
  */
kms_obj_range_t  KMS_Objects_GetRange(CK_OBJECT_HANDLE hKey)
{
  uint32_t MinSlot;
  uint32_t MaxSlot;

  /* Read the available static slots from the platform */
  KMS_PlatfObjects_EmbeddedRange(&MinSlot, &MaxSlot);
  /* If hKey is in the range of the embedded keys */
  if ((hKey <= MaxSlot) && (hKey >= MinSlot))
  {
    return (KMS_OBJECT_RANGE_EMBEDDED);
  }

#ifdef KMS_NVM_ENABLED
  /* Read the available nvm slots from the platform */
  KMS_PlatfObjects_NvmStaticRange(&MinSlot, &MaxSlot);
  /* If hKey is in the range of nvm keys */
  if ((hKey <= MaxSlot) && (hKey >= MinSlot))
  {
    return (KMS_OBJECT_RANGE_NVM_STATIC_ID);
  }

#ifdef KMS_NVM_DYNAMIC_ENABLED

  /* Read the available nvm slots from the platform */
  KMS_PlatfObjects_NvmDynamicRange(&MinSlot, &MaxSlot);
  /* If hKey is in the range of nvm keys */
  if ((hKey <= MaxSlot) && (hKey >= MinSlot))
  {
    return (KMS_OBJECT_RANGE_NVM_DYNAMIC_ID);
  }
#endif  /* KMS_NVM_DYNAMIC_ENABLED */
#endif  /* KMS_NVM_ENABLED */

#ifdef KMS_EXT_TOKEN_ENABLED
  /* Read the available external token slots from the platform */
  KMS_PlatfObjects_ExtTokenStaticRange(&MinSlot, &MaxSlot);
  /* If hKey is in the range of external token keys */
  if ((hKey <= MaxSlot) && (hKey >= MinSlot))
  {
    return (KMS_OBJECT_RANGE_EXT_TOKEN_STATIC_ID);
  }

  /* Read the available external token slots from the platform */
  KMS_PlatfObjects_ExtTokenDynamicRange(&MinSlot, &MaxSlot);
  /* If hKey is in the range of external token keys */
  if ((hKey <= MaxSlot) && (hKey >= MinSlot))
  {
    return (KMS_OBJECT_RANGE_EXT_TOKEN_DYNAMIC_ID);
  }

#endif  /* KMS_EXT_TOKEN_ENABLED */

  /* hKey not in known ranges */
  return KMS_OBJECT_RANGE_UNKNOWN;
}

/**
  * @brief  This function allow to Lock key usage.
  * @param  hKey key handle
  * @retval CK_RV return value
  */
CK_RV  KMS_LockKeyHandle(CK_OBJECT_HANDLE hKey)
{
  uint32_t  lock_table_index;
  
  /* if the table is full, then return */
  if( kms_locked_key_index >= KMS_MAX_LOCK_KEY_HANDLE )
      return CKR_CANT_LOCK;
  
  /* Check that the Handle is not already registered in the table */
  for (lock_table_index=0; lock_table_index<kms_locked_key_index; lock_table_index++)
  {
    /* If the hKey is already registered, then just exit */
    if(kms_locked_key_handles[lock_table_index] == hKey)
      return CKR_OK;
  }

  kms_locked_key_handles[lock_table_index] = hKey;
  lock_table_index++;

  return CKR_OK;
}

/**
  * @brief  This function allow to check if a key has not been locked.
  * @param  hKey key handle
  * @retval CK_RV return value: CKR_OK if not Locked, CKR_KEY_HANDLE_INVALID 
  *         if key not allowed.
  */
CK_RV  KMS_CheckKeyIsNotLocked(CK_OBJECT_HANDLE hKey)
{
  uint32_t  lock_table_index;
  
  /* Check that the Handle is not already registered in the table */
  for (lock_table_index=0; lock_table_index<kms_locked_key_index; lock_table_index++)
  {
    /* If the hKey is already registered, then just exit */
    if(kms_locked_key_handles[lock_table_index] == hKey)
      return CKR_KEY_HANDLE_INVALID;
  }
  
  return CKR_OK;
}

/**
  * @brief  This function search for an attribute in an blob
  * @param  searched_id attribute ID to search
  * @param  pkms_key_head blob object to search in
  * @param  pAttribute found attribute
  * @retval CKR_OK if attribute is found, CKR_ATTRIBUTE_TYPE_INVALID otherwise
  */
CK_RV  KMS_Objects_SearchAttributes(uint32_t searched_id, kms_obj_keyhead_t *pkms_key_head, kms_ref_t **pAttribute)
{
  kms_ref_t *pkms_blob_current = (kms_ref_t *)pkms_key_head->blobs;
  uint8_t  *pkms_blob;
  uint32_t blob_index;
  uint32_t CurrentAttributeSize;

  /* The blob containing the attributes is described by the kms_obj_keyhead_t */
  for (blob_index = 0; blob_index < pkms_key_head->blobs_count; blob_index++)
  {

    /* Parse the attributes from the blob to find the id */
    if (pkms_blob_current->id == searched_id)
    {
      *pAttribute = pkms_blob_current;

      /* ID is found */
      return CKR_OK;
    }
    CurrentAttributeSize = pkms_blob_current->size ;
    /* When size is not a multiple of 4, we have to consider 4 bytes  alignment */
    if ((CurrentAttributeSize % 4UL) != 0UL)
    {
      CurrentAttributeSize += 4UL - (CurrentAttributeSize % 4UL);
    }

    pkms_blob = &((uint8_t *)pkms_blob_current)[4UL + 4UL + CurrentAttributeSize];

    /* Point to the next Attibute */
    pkms_blob_current = (kms_ref_t *)pkms_blob;

  }
  return (CKR_ATTRIBUTE_TYPE_INVALID);
}

/**
  * @brief  This function translate key value
  * @note   Keys are stored on uint32_t buffer and translated into uint8_t one
  * @param  pAttribute attribute containing the key to translate
  * @param  vKeyValue translated key
  * @retval CKR_OK if success, CKR_DATA_LEN_RANGE otherwise
  */
CK_RV     KMS_Objects_TranslateKey(kms_ref_t *pAttribute, uint8_t *vKeyValue)
{
  uint32_t index_key;

  /* Set key size with value from attribute  */
  if ((pAttribute->size % 4UL) == 0UL)   /* check that we have a multiple of 4 bytes */
  {

    for (index_key = 0; index_key < (pAttribute->size / sizeof(uint32_t)); index_key++)
    {
      vKeyValue[(index_key * sizeof(uint32_t))]   = (uint8_t)(pAttribute->data[index_key] >> 24);
      vKeyValue[(index_key * sizeof(uint32_t)) + 1UL] = (uint8_t)(pAttribute->data[index_key] >> 16);
      vKeyValue[(index_key * sizeof(uint32_t)) + 2UL] = (uint8_t)(pAttribute->data[index_key] >> 8);
      vKeyValue[(index_key * sizeof(uint32_t)) + 3UL] = (uint8_t)(pAttribute->data[index_key]);
    }

    return CKR_OK;

  }

  return (CKR_DATA_LEN_RANGE);

}

/**
  * @brief  This function translate RSA key value
  * @note   Keys are stored on uint32_t buffer and translated into uint8_t one
  * @param  pAttribute attribute containing the key to translate
  * @param  vKeyValue translated key
  */
/* Read value from the structure. Need to be translated from
   (uint32_t*) to (uint8_t *) */
void     KMS_Objects_TranslateRsaAttributes(kms_ref_t *pAttribute, uint8_t *vKeyValue)
{
  uint32_t index_key;
  uint32_t reste;

  /* RSA attributes can be of 257, 129, 65 bytes ==> not a modulo 4 */

  for (index_key = 0; index_key < (pAttribute->size / sizeof(uint32_t)); index_key++)
  {
    vKeyValue[(index_key * sizeof(uint32_t))]   = (uint8_t)(pAttribute->data[index_key] >> 24);
    vKeyValue[(index_key * sizeof(uint32_t)) + 1UL] = (uint8_t)(pAttribute->data[index_key] >> 16);
    vKeyValue[(index_key * sizeof(uint32_t)) + 2UL] = (uint8_t)(pAttribute->data[index_key] >> 8);
    vKeyValue[(index_key * sizeof(uint32_t)) + 3UL] = (uint8_t)(pAttribute->data[index_key]);
  }

  /* Set key size with value from attribute  */
  reste = pAttribute->size % 4UL;
  if (reste != 0UL)     /* check that we have a multiple of 4 bytes */
  {
    if (reste == 1UL)
    {
      vKeyValue[(index_key * sizeof(uint32_t))]   = (uint8_t)(pAttribute->data[index_key]);
    }

    /*      vKeyValue[(index_key*sizeof(uint32_t))]   = (uint8_t)(pAttribute->data[index_key]>>24);*/
    if (reste == 2UL)
    {
      vKeyValue[(index_key * sizeof(uint32_t))]   = (uint8_t)(pAttribute->data[index_key] >> 8);
      vKeyValue[(index_key * sizeof(uint32_t)) + 1UL] = (uint8_t)(pAttribute->data[index_key]);
    }

    if (reste == 3UL)
    {
      vKeyValue[(index_key * sizeof(uint32_t))]   = (uint8_t)(pAttribute->data[index_key]);
      vKeyValue[(index_key * sizeof(uint32_t)) + 1UL] = (uint8_t)(pAttribute->data[index_key] >> 8);
      vKeyValue[(index_key * sizeof(uint32_t)) + 2UL] = (uint8_t)(pAttribute->data[index_key] >> 16);
    }

  }
}

/**
  * @brief  This function imports an encrypted blob into the NVM storage
  * @note   It ensure authentication, verification and decryption of the blob
  * @param  pData Encrypted blobs to import
  * @param  pImportBlobState blob importation result
  * @retval Operation status within @ref kms_import_blob_state_t
  */
CK_RV  KMS_Objects_ImportBlob(CK_BYTE_PTR pData, CK_ULONG_PTR pImportBlobState)
{
  kms_import_blob_state_t e_ret_status = KMS_IMPORT_BLOB_SESSION_INUSE;
  uint32_t session_index;

  /* First ensure there is no session in use prior to update keys with blob contents */
  session_index = 1;
  do
  {
    if (KMS_GETSESSSION(session_index).state != KMS_SESSION_NOT_USED)
    {
      break;
    }
    session_index++;
  } while (session_index <= KMS_NB_SESSIONS_MAX); /* Session index are going from 1 to KMS_NB_SESSIONS_MAX */

  /* The received Blob is protected. It contains a Header describing the content */

  if (session_index > KMS_NB_SESSIONS_MAX)
  {
    /* Blob header Authentication */
    e_ret_status = authenticate_blob_header((KMS_BlobRawHeaderTypeDef *)pData);
  }
  if (e_ret_status == KMS_IMPORT_BLOB_NO_ERROR)
  {
    /* Blob Authentication */
    e_ret_status = authenticate_blob((KMS_BlobRawHeaderTypeDef *)pData);
  }

  /* Key Install */
  /* Check that Blob Authentication is OK */
  if (e_ret_status == KMS_IMPORT_BLOB_NO_ERROR)
  {
    /* Read the Blob & Install it in NVM */
    e_ret_status = install_blob((KMS_BlobRawHeaderTypeDef *)pData);
  }

  *pImportBlobState = e_ret_status;

  return CKR_OK;

}

/**
  * @}
  */

/* Private function ----------------------------------------------------------*/

/** @addtogroup KMS_OBJECTS_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Authenticate KMS Blob header
  * @param  pBlobHeader the blob header
  * @retval KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR if error,
  *         KMS_IMPORT_BLOB_NO_ERROR otherwise.
  */
static kms_import_blob_state_t authenticate_blob_header(KMS_BlobRawHeaderTypeDef *pBlobHeader)
{
  CK_SESSION_HANDLE session;
  CK_MECHANISM smech;
  CK_ULONG ObjectIdIndex;

  /* Open session */
  if (KMS_OpenSession(0, CKF_SERIAL_SESSION, NULL, 0, &session) != CKR_OK)
  {
    return KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR;
  }

  /* Verify signature */
  ObjectIdIndex = KMS_PlatfObjects_GetBlobVerifyKey();
  smech.pParameter = NULL;
  smech.ulParameterLen = 0;
  smech.mechanism = CKM_ECDSA_SHA256;

  if (KMS_VerifyInit(session, &smech, ObjectIdIndex) != CKR_OK)
  {
    (void)KMS_CloseSession(session);
    return KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR;
  }
  if (KMS_Verify(session,
                 (CK_BYTE_PTR)pBlobHeader, (CK_ULONG)(&(pBlobHeader->HeaderMAC[0])) - (CK_ULONG)pBlobHeader,
                 (CK_BYTE_PTR) & (pBlobHeader->HeaderMAC[0]),
                 (CK_ULONG)KMS_BLOB_MAC_LEN) != CKR_OK)
  {
    (void)KMS_CloseSession(session);
    return KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR;
  }

  (void)KMS_CloseSession(session);

  return KMS_IMPORT_BLOB_NO_ERROR;
}

/**
  * @brief  Verify KMS Blob
  * @note   Decrypt is done to verify the Integrity of the Blob.
  * @param  pBlobHeader the blob header
  * @retval KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR if error,
  *         KMS_IMPORT_BLOB_NO_ERROR otherwise.
  */
static kms_import_blob_state_t authenticate_blob(KMS_BlobRawHeaderTypeDef *pBlobHeader)
{
  kms_import_blob_state_t e_se_err_status = KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR;
  CK_RV        e_ret_status;

  /*  chunk size is the maximum , the 1st block can be smaller */
  /*  the chunk is static to avoid  large stack */
  uint8_t fw_decrypted_chunk[KMS_BLOB_CHUNK_SIZE] __attribute__((aligned(8)));
  uint8_t fw_encrypted_chunk[KMS_BLOB_CHUNK_SIZE] __attribute__((aligned(8)));
  uint8_t *pfw_source_address = (uint8_t *)0xFFFFFFFFU;
  uint32_t fw_decrypted_total_size = 0;
  uint32_t size;
  uint32_t fw_decrypted_chunk_size;
  uint32_t fw_tag_len = 0;
  uint8_t fw_tag_output[KMS_BLOB_TAG_LEN];
  uint32_t pass_index;
  CK_SESSION_HANDLE aessession;
  CK_SESSION_HANDLE digsession;
  CK_ULONG ObjectIdIndex;
  CK_MECHANISM aesmech;
  CK_MECHANISM digmech;

  if ((pBlobHeader == NULL))
  {
    return KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR;
  }

  /* Open session */
  if (KMS_OpenSession(0, CKF_SERIAL_SESSION, NULL, 0, &aessession) != CKR_OK)
  {
    return KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR;
  }
  if (KMS_OpenSession(0, CKF_SERIAL_SESSION, NULL, 0, &digsession) != CKR_OK)
  {
    (void)KMS_CloseSession(aessession);
    return KMS_IMPORT_BLOB_CRYPTO_BLOB_AUTH_ERROR;
  }

  pass_index = 0;


  /* Decryption process*/
  ObjectIdIndex = KMS_PlatfObjects_GetBlobDecryptKey();
  aesmech.mechanism = CKM_AES_CBC;
  aesmech.pParameter = &(pBlobHeader->InitVector[0]);
  aesmech.ulParameterLen = KMS_BLOB_IV_LEN;

  e_ret_status = KMS_DecryptInit(aessession, &aesmech, ObjectIdIndex);
  if (e_ret_status == CKR_OK)
  {
    /* Digest process*/
    digmech.mechanism = CKM_SHA256;
    digmech.pParameter = NULL;
    digmech.ulParameterLen = 0;
    e_ret_status = KMS_DigestInit(digsession, &digmech);
  }

  if (e_ret_status == CKR_OK)
  {
    /* Decryption loop*/
    while ((fw_decrypted_total_size < (pBlobHeader->BlobSize)) && (e_ret_status == CKR_OK))
    {

      if (pass_index == 0UL)
      {
        pfw_source_address = (uint8_t *)(KMS_LL_FLASH_GetBlobDownloadAddress() + KMS_BLOB_IMG_OFFSET);
      }
      fw_decrypted_chunk_size = sizeof(fw_decrypted_chunk);
      if ((pBlobHeader->BlobSize - fw_decrypted_total_size) < fw_decrypted_chunk_size)
      {
        fw_decrypted_chunk_size = pBlobHeader->BlobSize - fw_decrypted_total_size;
      }
      size = fw_decrypted_chunk_size;

      /* Decrypt Append*/
      e_ret_status = KMS_LL_FLASH_Read(fw_encrypted_chunk, pfw_source_address, size);

      if (e_ret_status == CKR_OK)
      {
        if (size == 0UL)
        {
          pass_index += 1UL;
        }
        else
        {
          e_ret_status = KMS_DecryptUpdate(aessession,
                                           (CK_BYTE *)fw_encrypted_chunk,
                                           size,
                                           (CK_BYTE *)fw_decrypted_chunk,
                                           (CK_ULONG_PTR)&fw_decrypted_chunk_size);

          if ((e_ret_status == CKR_OK) && (fw_decrypted_chunk_size == size))
          {
            e_ret_status = KMS_DigestUpdate(digsession, fw_decrypted_chunk, fw_decrypted_chunk_size);
            if (e_ret_status == CKR_OK)
            {
              /* Update source pointer */
              pfw_source_address = &pfw_source_address[fw_decrypted_chunk_size];
              fw_decrypted_total_size += fw_decrypted_chunk_size;
              (void)memset(fw_decrypted_chunk, 0xff, fw_decrypted_chunk_size);
              pass_index += 1UL;
            }
          }
        }
      }
    }
  }

  if ((e_ret_status == CKR_OK))
  {
    /* Do the Finalization, check the authentication TAG*/
    fw_tag_len = KMS_BLOB_TAG_LEN; /* Section 5.2: Buffer handling compliance */
    e_ret_status =  KMS_DecryptFinal(aessession, fw_tag_output, (CK_ULONG_PTR)&fw_tag_len);

    if (e_ret_status == CKR_OK)
    {
      fw_tag_len = KMS_BLOB_TAG_LEN; /* Section 5.2: Buffer handling compliance */
      e_ret_status = KMS_DigestFinal(digsession, fw_tag_output, (CK_ULONG_PTR)&fw_tag_len);
      if (e_ret_status == CKR_OK)
      {
        if ((fw_tag_len == KMS_BLOB_TAG_LEN) && (memcmp(fw_tag_output, pBlobHeader->BlobTag, KMS_BLOB_TAG_LEN) == 0))
        {
          e_se_err_status = KMS_IMPORT_BLOB_NO_ERROR;
        }
      }
    }
  }

  (void)KMS_CloseSession(aessession);
  (void)KMS_CloseSession(digsession);
  return e_se_err_status;
}


/**
  * @brief  Read and decrypt a blob chunk
  * @note   Chunk size is @ref KMS_BLOB_CHUNK_SIZE
  * @param  session the current KMS session in use for blob decryption
  * @param  p_source_address the current blob decryption address
  * @param  size the size to decrypt
  * @param  p_decrypted_chunk the decrypted chunk
  * @param  p_decrypted_size the decrypted chunk size
  * @retval Operation status
  */
CK_RV  read_next_chunk(uint32_t session, uint8_t *p_source_address, uint32_t size, uint8_t *p_decrypted_chunk,
                       uint32_t *p_decrypted_size)
{
  uint8_t fw_encrypted_chunk[KMS_BLOB_CHUNK_SIZE] __attribute__((aligned(8)));
  CK_RV        e_ret_status;

  /* Read */
  e_ret_status = KMS_LL_FLASH_Read(fw_encrypted_chunk, p_source_address, size);

  if (e_ret_status == CKR_OK)
  {
    e_ret_status = KMS_DecryptUpdate(session,
                                     (CK_BYTE *)fw_encrypted_chunk,
                                     size,
                                     (CK_BYTE *)p_decrypted_chunk,
                                     (CK_ULONG_PTR)(uint32_t)&p_decrypted_size);
  }

  return (e_ret_status);
}

/**
  * @brief  Install KMS blob
  * @param  pBlobHeader the blob header
  * @retval Operation status
  */
static kms_import_blob_state_t install_blob(KMS_BlobRawHeaderTypeDef *pBlobHeader)
{
  kms_import_blob_state_t e_se_err_status = KMS_IMPORT_BLOB_CRYPTO_BLOB_FORM_ERROR;
  CK_RV        e_ret_status;
  CK_SESSION_HANDLE session;
  CK_ULONG ObjectIdIndex;
  CK_MECHANISM aesmech;

  /*  chunk size is the maximum , the 1st block can be smaller */
  /*  the chunk is static to avoid  large stack */
  uint8_t fw_decrypted_chunk[KMS_BLOB_CHUNK_SIZE] __attribute__((aligned(8)));
  uint8_t *pfw_source_address;
  uint32_t fw_decrypted_total_size = 0;
  uint32_t size;
  uint32_t fw_decrypted_chunk_size;
  uint32_t fw_tag_len = 0;
  uint8_t fw_tag_output[KMS_BLOB_TAG_LEN];
  nvms_error_t nvm_status = NVMS_NOERROR;
  uint32_t *pNextMagic;
  uint32_t bytes_to_next_magic = 0;
  uint32_t bytes_copied_in_kms = 0;
  uint32_t bytes_copied_from_decrypted_chunk = 0;
  uint32_t nvm_warning_occurred = 0;

  static biggest_kms_obj_keyhead_t  import_blob_object;

  if ((pBlobHeader == NULL))
  {
    return KMS_IMPORT_BLOB_CRYPTO_BLOB_FORM_ERROR;
  }


  /* The platform has to be initialized */
  KMS_PlatfObjects_Init();

  /* Open session */
  if (KMS_OpenSession(0, CKF_SERIAL_SESSION, NULL, 0, &session) != CKR_OK)
  {
    return KMS_IMPORT_BLOB_CRYPTO_BLOB_FORM_ERROR;
  }

  /* Decryption process*/
  ObjectIdIndex = KMS_PlatfObjects_GetBlobDecryptKey();
  aesmech.mechanism = CKM_AES_CBC;
  aesmech.pParameter = &(pBlobHeader->InitVector[0]);
  aesmech.ulParameterLen = KMS_BLOB_IV_LEN;

  e_ret_status = KMS_DecryptInit(session, &aesmech, ObjectIdIndex);

  if (e_ret_status == CKR_OK)
  {

    /* Initialize the NVM */
    KMS_PlatfObjects_NvmInitialize();

    pfw_source_address = (uint8_t *)(KMS_LL_FLASH_GetBlobDownloadAddress() + KMS_BLOB_IMG_OFFSET);
    fw_decrypted_chunk_size = sizeof(fw_decrypted_chunk);
    size = fw_decrypted_chunk_size;

    /* Read next chunk */
    e_ret_status = read_next_chunk(session, pfw_source_address, size, fw_decrypted_chunk, &fw_decrypted_chunk_size);
    pfw_source_address = &pfw_source_address[fw_decrypted_chunk_size];
    fw_decrypted_total_size += fw_decrypted_chunk_size;

    pNextMagic = (uint32_t *)fw_decrypted_chunk;

    /* Process the Blob to the end of the last decrypted chunk of the blob  */
    while (((fw_decrypted_total_size < (pBlobHeader->BlobSize))
            || (pNextMagic < (uint32_t *)(fw_decrypted_chunk + fw_decrypted_chunk_size)))
           && (e_ret_status == CKR_OK) && (nvm_status == NVMS_NOERROR))
    {

      /* Read Object by objects */
      /* Copy the object from Flash to Object structure */

      /* Copy header of object into buffer to NVM */
      if ((fw_decrypted_chunk_size - bytes_copied_from_decrypted_chunk) > sizeof(kms_obj_keyhead_no_blob_t))
      {
        /* Copy the header of the object */
        (void)memcpy(&import_blob_object, fw_decrypted_chunk + bytes_copied_from_decrypted_chunk,
                     sizeof(kms_obj_keyhead_no_blob_t));

        bytes_copied_in_kms = sizeof(kms_obj_keyhead_no_blob_t);
        bytes_copied_from_decrypted_chunk = bytes_copied_from_decrypted_chunk + sizeof(kms_obj_keyhead_no_blob_t);
        pNextMagic = &pNextMagic[sizeof(kms_obj_keyhead_no_blob_t) / 4UL];
        bytes_to_next_magic = bytes_to_next_magic + sizeof(kms_obj_keyhead_no_blob_t);

      }
      else if (fw_decrypted_total_size < (pBlobHeader->BlobSize))
      {
        /* When the end of the Chunk do not contain a full object header */
        /* Copy the beginning with decrypted latest bytes */
        /* Then read a chunk */
        /* To complete the header reading */

        /* Just copy the end of the decrypted buffer */
        (void)memcpy(&import_blob_object, fw_decrypted_chunk + bytes_copied_from_decrypted_chunk,
                     fw_decrypted_chunk_size - bytes_copied_from_decrypted_chunk);

        bytes_copied_in_kms = fw_decrypted_chunk_size - bytes_copied_from_decrypted_chunk;

        /* Read the next chunk */
        /* Read next chunk */
        /* If it is the last access with less than ckunk size to be read */
        if ((pBlobHeader->BlobSize - fw_decrypted_total_size) < fw_decrypted_chunk_size)
        {
          fw_decrypted_chunk_size = pBlobHeader->BlobSize - fw_decrypted_total_size;
        }
        size = fw_decrypted_chunk_size;

        e_ret_status = read_next_chunk(session, pfw_source_address, size, fw_decrypted_chunk, &fw_decrypted_chunk_size);
        pfw_source_address = &pfw_source_address[fw_decrypted_chunk_size];
        fw_decrypted_total_size += fw_decrypted_chunk_size;

        (void)memcpy((uint8_t *)&import_blob_object + bytes_copied_in_kms, fw_decrypted_chunk,
                     sizeof(kms_obj_keyhead_no_blob_t) - bytes_copied_in_kms);
        bytes_copied_from_decrypted_chunk = sizeof(kms_obj_keyhead_no_blob_t) - bytes_copied_in_kms ;
        bytes_copied_in_kms = sizeof(kms_obj_keyhead_no_blob_t);

        pNextMagic = (uint32_t *)fw_decrypted_chunk + bytes_copied_from_decrypted_chunk;
        bytes_to_next_magic = bytes_to_next_magic + bytes_copied_from_decrypted_chunk;

      }
      else
      {
        /* Nothing to do in this case */
      }

      /* Search for the next object */
      /* Or end of Blob */
      while (*pNextMagic != KMS_ABI_VERSION_CK_2_40)
      {
        /* If we reach the end of the chunk */
        if (pNextMagic == (uint32_t *)(fw_decrypted_chunk + fw_decrypted_chunk_size))
        {
          /* If last chunk decrypted but no next magic, then means no more magic. ==> End of blob reached */
          if (fw_decrypted_total_size == (pBlobHeader->BlobSize))
          {
            break;
          }

          /* Store the current decrypted chunk data */
          (void)memcpy((uint8_t *)&import_blob_object + bytes_copied_in_kms,
                       fw_decrypted_chunk + bytes_copied_from_decrypted_chunk,
                       fw_decrypted_chunk_size - bytes_copied_from_decrypted_chunk);
          bytes_copied_in_kms = bytes_copied_in_kms + (fw_decrypted_chunk_size - bytes_copied_from_decrypted_chunk);
          bytes_copied_from_decrypted_chunk = fw_decrypted_chunk_size;

          /* Read the next chunk */
          /* Read next chunk */

          /* If it is the last access with less than ckunk size to be read */
          if ((pBlobHeader->BlobSize - fw_decrypted_total_size) < fw_decrypted_chunk_size)
          {
            fw_decrypted_chunk_size = pBlobHeader->BlobSize - fw_decrypted_total_size;
          }
          size = fw_decrypted_chunk_size;

          e_ret_status = read_next_chunk(session,
                                         pfw_source_address,
                                         size,
                                         fw_decrypted_chunk,
                                         &fw_decrypted_chunk_size);
          pfw_source_address = &pfw_source_address[fw_decrypted_chunk_size];
          fw_decrypted_total_size += fw_decrypted_chunk_size;

          bytes_copied_from_decrypted_chunk = 0;
          bytes_to_next_magic = 0;
          pNextMagic = (uint32_t *)fw_decrypted_chunk;
        }
        else
        {
          pNextMagic++;
          bytes_to_next_magic += 4UL;
        }

      };

      /* We found the next object or detected end of blob */
      /* Copy data to buffer to NVM and call NVM to store object */

      /* Store the current decrypted chunk data */
      (void)memcpy((uint8_t *)&import_blob_object + bytes_copied_in_kms,
                   fw_decrypted_chunk + bytes_copied_from_decrypted_chunk,
                   bytes_to_next_magic - bytes_copied_from_decrypted_chunk);
      bytes_copied_in_kms = bytes_copied_in_kms + (bytes_to_next_magic - bytes_copied_from_decrypted_chunk);
      bytes_copied_from_decrypted_chunk += (bytes_to_next_magic - bytes_copied_from_decrypted_chunk);

      /* Store the object just finalized */
      nvm_status = KMS_PlatfObjects_NvmStoreObject(import_blob_object.object_id,
                                                   (uint8_t *)&import_blob_object,
                                                   import_blob_object.blobs_size + sizeof(kms_obj_keyhead_no_blob_t));

      if (nvm_status == NVMS_WARNING)
      {
        nvm_warning_occurred++;
        nvm_status = NVMS_NOERROR;
      }
    };

    /* Reload the NVM Object list */
    KMS_PlatfObjects_Init();

  }

  if ((e_ret_status == CKR_OK) && (nvm_status == NVMS_NOERROR))
  {
    /* Do the Finalization */
    fw_tag_len = KMS_BLOB_TAG_LEN; /* Section 5.2: Buffer handling compliance */
    e_ret_status =  KMS_DecryptFinal(session, fw_tag_output, (CK_ULONG_PTR)&fw_tag_len);

    if (e_ret_status == CKR_OK)
    {
      if (nvm_warning_occurred != 0UL)
      {
        e_se_err_status = KMS_IMPORT_BLOB_NVM_WARNING;
      }
      else
      {
        e_se_err_status = KMS_IMPORT_BLOB_FINALIZED;
      }
    }
  }

  if (nvm_status != NVMS_NOERROR)
  {
    e_se_err_status = KMS_IMPORT_BLOB_NVM_ERROR;
  }

  (void)KMS_CloseSession(session);
  return e_se_err_status;
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
