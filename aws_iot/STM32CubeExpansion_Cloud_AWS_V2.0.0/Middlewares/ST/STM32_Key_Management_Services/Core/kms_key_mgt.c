/**
  ******************************************************************************
  * @file    kms_key_mgt.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
  *          module key handling functionalities.
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
#include "kms_key_mgt.h"
#include "kms_aes.h"                    /* Encryption & key storage service for key derivation */
#include "kms_objects.h"                /* KMS objects services */
#include "kms_nvm_storage.h"            /* KMS storage services */
#include "kms_platf_objects.h"          /* KMS platform objects services */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_KEY Key handling services
  * @{
  */


/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/** @addtogroup KMS_KEY_Private_Functions Private Functions
  * @{
  */

/* Private function prototypes -----------------------------------------------*/
static CK_RV  fill_in_aes_object_instance(uint8_t *pKeyBuffer, uint32_t KeyLen,
                                          CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulAttributeCount,
                                          biggest_kms_obj_keyhead_t *pObjectToCreate);

/* Private function ----------------------------------------------------------*/

/**
  * @brief  This function helps create object to store newly create AES keys
  * @param  pKeyBuffer key buffer
  * @param  KeyLen key length
  * @param  pTemplate template containing attributes to copy into object
  * @param  ulAttributeCount number of attributes in the template
  * @param  pObjectToCreate object to be created
  * @retval Operation status
  */
static CK_RV  fill_in_aes_object_instance(uint8_t *pKeyBuffer, uint32_t KeyLen,
                                          CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulAttributeCount,
                                          biggest_kms_obj_keyhead_t *pObjectToCreate)
{
  uint32_t      KeyValue_int_aligned;
  uint32_t      Index;
  uint32_t      AttributesToCopy;
  uint32_t      attributesSize;
  CK_RV         e_ret_status = CKR_FUNCTION_FAILED;
  CK_ATTRIBUTE_PTR pTemp = pTemplate;

  /* Check that template values fit into object to fill */
  attributesSize = (5UL * sizeof(CK_ULONG)) + KeyLen;
  for (AttributesToCopy = 0; AttributesToCopy < ulAttributeCount; AttributesToCopy++)
  {
    attributesSize += pTemplate[AttributesToCopy].ulValueLen + (2UL * sizeof(CK_ULONG)) /* type & ulValueLen */;
  }
  if (attributesSize <= (sizeof(biggest_kms_obj_keyhead_t) - sizeof(kms_obj_keyhead_no_blob_t)))
  {
    e_ret_status = CKR_OK;
  }

  if (e_ret_status == CKR_OK)
  {
    pObjectToCreate->version = KMS_ABI_VERSION_CK_2_40;
    pObjectToCreate->configuration = KMS_ABI_CONFIG_KEYHEAD;
    pObjectToCreate->class  = CKO_SECRET_KEY;
    pObjectToCreate->type  = CKK_AES;
    pObjectToCreate->blobs[0] = CKA_KEY_TYPE;
    pObjectToCreate->blobs[1] = sizeof(CK_KEY_TYPE);
    pObjectToCreate->blobs[2] = CKK_AES;
    pObjectToCreate->blobs[3] = CKA_VALUE;
    pObjectToCreate->blobs[4] = KeyLen;

    /* Fill the object with the KeyValue passed */
    for (Index = 0; Index < (KeyLen / 4UL); Index++)
    {
      /* We take care of the endianess */
      KeyValue_int_aligned = (uint32_t)pKeyBuffer[Index + 0UL] + ((uint32_t)pKeyBuffer[Index + 1UL] << 8) + \
                             ((uint32_t)pKeyBuffer[Index + 2UL] << 16) + ((uint32_t)pKeyBuffer[Index + 3UL] << 24);
      pObjectToCreate->blobs[5UL + Index] = KeyValue_int_aligned;
    }

    /* Add Attributes coming from the Template */
    Index = Index + 5UL;
    for (AttributesToCopy = 0; (AttributesToCopy < ulAttributeCount) && (e_ret_status == CKR_OK); AttributesToCopy++)
    {
      /* Ensure data to copy fits space left in object */
      if ((((Index + 2UL/* pTemplate->type & pTemplate->ulValueLen */)*sizeof(CK_ULONG)) + pTemp->ulValueLen)
          < ((sizeof(biggest_kms_obj_keyhead_t) - sizeof(kms_obj_keyhead_no_blob_t))))
      {
        /* We re-align the attributes, with the logical order ==> size before value */
        /* To allow the parsing. */
        pObjectToCreate->blobs[Index] = pTemp->type;
        Index++;
        pObjectToCreate->blobs[Index] = pTemp->ulValueLen;
        Index++;
        (void)memcpy((uint8_t *) & (pObjectToCreate->blobs[Index]), pTemp->pValue, pTemp->ulValueLen);
        Index += pTemp->ulValueLen / sizeof(uint32_t);
        Index += ((pTemp->ulValueLen % sizeof(uint32_t)) != 0UL) ? 1UL : 0UL;

        pTemp++;
      }
      else
      {
        e_ret_status = CKR_FUNCTION_FAILED;
      }
    }

    /* The blob size consider blob table length in bytes */
    pObjectToCreate->blobs_size  = (Index * sizeof(uint32_t));

    /* 2 Attributes for the Key description + attributes coming from the Template. */
    pObjectToCreate->blobs_count = 2UL + ulAttributeCount;
  }
  return e_ret_status;

}


/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_KEY_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This function is called upon @ref C_DeriveKey call
  * @note   Refer to @ref C_DeriveKey function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pMechanism key derivation mechanism
  * @param  hBaseKey base key for derivation
  * @param  pTemplate new key template
  * @param  ulAttributeCount template length
  * @param  phKey new key handle
  * @retval Operation status
  */
CK_RV          KMS_DeriveKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                             CK_OBJECT_HANDLE hBaseKey, CK_ATTRIBUTE_PTR  pTemplate,
                             CK_ULONG  ulAttributeCount, CK_OBJECT_HANDLE_PTR  phKey)
{

  CK_RV e_ret_status;
  CK_ULONG EncryptedLen;
  kms_obj_keyhead_t *pkms_object;
  kms_ref_t *P_pKeyAttribute;
  uint8_t *pKeyBuffer;
  CK_MECHANISM aes_ebc_mechanism = { CKM_AES_ECB, NULL, 0 };

  static biggest_kms_obj_keyhead_t  for_dynamic_object_creation;

  /* Check that we support the expected mechanism. */
  if (pMechanism->mechanism != CKM_AES_ECB_ENCRYPT_DATA)
  {
    return CKR_MECHANISM_INVALID;
  }

  /* We reuse the AES-EncryptInit function */
  /* The Encryption mechanism do not expect any param, we use one the local definition */

  e_ret_status = KMS_EncryptInit(hSession, &aes_ebc_mechanism, hBaseKey);

  if (e_ret_status == CKR_OK)
  {

    /* The Key */
    /* Read the key value from the Key Handle                 */
    /* Key Handle is the index to one of static or nvm        */
    pkms_object = KMS_Objects_GetPointer(hBaseKey);

    /* Check that hKey is valid */
    if (pkms_object != NULL)
    {

      /* Search for the Key Value to use */
      e_ret_status = KMS_Objects_SearchAttributes(CKA_VALUE, pkms_object, &P_pKeyAttribute);

      if ((e_ret_status == CKR_OK) &&
          (pkms_object->version == KMS_ABI_VERSION_CK_2_40) &&
          (pkms_object->configuration == KMS_ABI_CONFIG_KEYHEAD))
      {
        kms_ref_t  *pDeriveAttribute;

        /* As stated in PKCS11 spec:                                                   */
        /* The CKA_DERIVE attribute has the value CK_TRUE if and only if it is         */
        /*   possible to derive other keys from the key.                               */
        /* Check that the object allows to DERIVE a KEY, checking ATTRIBUTE CKA_DERIVE */

        e_ret_status = KMS_Objects_SearchAttributes(CKA_DERIVE, pkms_object, &pDeriveAttribute);

        if (e_ret_status == CKR_OK)
        {
          if (*pDeriveAttribute->data != CK_TRUE)
          {
            /* Key derivation not permitted for the selected object. */
            return (CKR_ACTION_PROHIBITED);
          }
        }

        /* Set key size with value from attribute  */
        if ((P_pKeyAttribute->size == 16UL) ||    /* 128 bits */
            (P_pKeyAttribute->size == 24UL) ||     /* 192 bits */
            (P_pKeyAttribute->size == 32UL))       /* 256 bits */
        {
          /* Allocate a Key buffer */
          pKeyBuffer = (uint8_t *)KMS_AllocKey(P_pKeyAttribute->size);
          if (pKeyBuffer == NULL)
          {
            return CKR_DEVICE_MEMORY;
          }

        }
        else
        {
          /* Unsupported key size */
          return (CKR_ATTRIBUTE_VALUE_INVALID);
        }

        /* Derivation is done based on the value passed in the MEchanism */
        if ((pMechanism->pParameter != NULL) &&
            (pMechanism->ulParameterLen != 0UL))
        {

          /* We calculate the derived KEY from the Encrypting the pMechanism->pParameter */
          /* with the hBaseKey */
          e_ret_status = KMS_Encrypt(hSession, pMechanism->pParameter,
                                     pMechanism->ulParameterLen, pKeyBuffer, &EncryptedLen);

          if (e_ret_status == CKR_OK)
          {
            /* At this stage, we can create an object to store the derived key, */
            /* The object should embed the created Key, and the Template part. */

            e_ret_status = fill_in_aes_object_instance(pKeyBuffer,
                                                       EncryptedLen,
                                                       pTemplate,
                                                       ulAttributeCount,
                                                       &for_dynamic_object_creation);

            /* Free the buffer */
            KMS_FreeKey(pKeyBuffer);
            pKeyBuffer = NULL;

            if (e_ret_status == CKR_OK)
            {

              /* Check that the pointer to return the KeyHandle is valid */
              if (phKey != NULL)
              {
                CK_OBJECT_HANDLE  ObjectId = 0;
                nvms_error_t nvm_status;

                /* Create a new ObjectID */
                e_ret_status = KMS_PlatfObjects_AllocateNvmDynamicObjectId(&ObjectId);

                /* Return the ObjectId Handle */
                *phKey = ObjectId;

                if (e_ret_status != CKR_OK)
                {
                  return e_ret_status;
                }

                /* Fill in the object_id in the object header */
                for_dynamic_object_creation.object_id = ObjectId;

                /* Record the object in NVM */
                nvm_status = KMS_PlatfObjects_NvmStoreObject(ObjectId,
                                                             (uint8_t *)&for_dynamic_object_creation,
                                                             for_dynamic_object_creation.blobs_size
                                                             + sizeof(kms_obj_keyhead_no_blob_t));

                /* A Garbage generate a WARNING ==> Not an error */
                if ((nvm_status == NVMS_NOERROR) || (nvm_status == NVMS_WARNING))
                {
                  e_ret_status = CKR_OK;
                }
                else
                {
                  /* error = NVMS_KEY_INVALID */
                  if (nvm_status == NVMS_SLOT_INVALID)
                  {
                    e_ret_status = CKR_SLOT_ID_INVALID;
                  }
                  else
                    /* error cases NVMS_NOTINIT, NVMS_FLASH_FAILURE */
                    /* NVMS_OUT_OF_MEM, NVMS_INTERNAL */
                  {
                    e_ret_status = CKR_DEVICE_MEMORY ;
                  }
                }

                /* To take the latest entry (the derived key) into account in the table, */
                /* we have to refresh the list. */
                KMS_PlatfObjects_NvmDynamicObjectList();

              }
              else
              {
                e_ret_status = CKR_ARGUMENTS_BAD;
              }
            }
          }
        }

        /* Free the buffer */
        if (pKeyBuffer != NULL)
        {
          KMS_FreeKey(pKeyBuffer);
        }
      }
    }
  }

  /* No more crypto to manage with this key */
  KMS_GETSESSSION(hSession).hKey = KMS_HANDLE_KEY_NOT_KNOWN;


  return e_ret_status;
}


#ifdef KMS_WRAP_KEY

/**
  * @brief  This function is called upon @ref C_WrapKey call
  * @note   Refer to @ref C_WrapKey function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pMechanism wrapping mechanism
  * @param  hWrappingKey wrapping key handle
  * @param  hKey key to be wrapped handle
  * @param  pWrappedKey wrapped key
  * @param  pulWrappedKeyLen wrapped key length
  * @retval Operation status
  */
CK_RV KMS_WrapKey(CK_SESSION_HANDLE hSession,     CK_MECHANISM_PTR  pMechanism,
                  CK_OBJECT_HANDLE  hWrappingKey, CK_OBJECT_HANDLE  hKey,
                  CK_BYTE_PTR       pWrappedKey,  CK_ULONG_PTR      pulWrappedKeyLen)
{
  CK_RV e_ret_status = CKR_FUNCTION_FAILED;

  return e_ret_status;

}
#endif /* KMS_WRAP_KEY */

#ifdef KMS_GET_ATTRIBUTE_VALUE
/**
  * @brief  This function is called upon @ref C_GetAttributeValue call
  * @note   Refer to @ref C_GetAttributeValue function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  hObject object handle
  * @param  pTemplate template containing attributes to retrieve
  * @param  ulCount number of attributes in the template
  * @retval Operation status
  */
CK_RV KMS_GetAttributeValue(CK_SESSION_HANDLE hSession,  CK_OBJECT_HANDLE  hObject,
                            CK_ATTRIBUTE_PTR  pTemplate, CK_ULONG          ulCount)
{

  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  kms_obj_keyhead_t *pkms_object;
  CK_ATTRIBUTE_PTR pTemp = pTemplate;

  (void)(hSession);

  /* GetAttribute() is only authorised for objects with attribut EXTRACTABLE = TRUE */
  /* Verify that the object is extractable, reading the attributes */

  /* Read the key value from the Key Handle                 */
  /* Key Handle is the index to one of static or nvm        */
  pkms_object = KMS_Objects_GetPointer(hObject);

  /* Check that hObject is valid */
  if (pkms_object != NULL)
  {
    kms_ref_t  *pDeriveAttribute;

    /* Check the CKA_EXTRACTABLE attribute = CK_TRUE      */
    e_ret_status = KMS_Objects_SearchAttributes(CKA_EXTRACTABLE, pkms_object, &pDeriveAttribute);

    /* If attribute not found or object not destroyable */
    if ((e_ret_status != CKR_OK) || (*pDeriveAttribute->data != CK_TRUE))
    {
      /* Object cannot be removed. */
      return (CKR_ATTRIBUTE_SENSITIVE);
    }

    /* Object is extractable */
    /* Let's loop the Attributes passed in the Template, to extract the values of the matching Types */

    /*
    Description (extract from PKCS11 OASIS spec v2.40 :
    For each (type, pValue, ulValueLen) triple in the template, C_GetAttributeValue performs the following
    algorithm:
        1. If the specified attribute (i.e., the attribute specified by the type field) for the object cannot be
    revealed because the object is sensitive or unextractable, then the ulValueLen field in that triple is modified to
    hold the value CK_UNAVAILABLE_INFORMATION.
        2. Otherwise, if the specified value for the object is invalid (the object does not possess such an
    attribute), then the ulValueLen field in that triple is modified to hold the value
    CK_UNAVAILABLE_INFORMATION.
        3. Otherwise, if the pValue field has the value NULL_PTR, then the ulValueLen field is modified to hold
    the exact length of the specified attribute for the object.
        4. Otherwise, if the length specified in ulValueLen is large enough to hold the value of the specified
    attribute for the object, then that attribute is copied into the buffer located at pValue, and the
    ulValueLen field is modified to hold the exact length of the attribute.
        5. Otherwise, the ulValueLen field is modified to hold the value CK_UNAVAILABLE_INFORMATION.

    If case 1 applies to any of the requested attributes, then the call should return the value
    CKR_ATTRIBUTE_SENSITIVE. If case 2 applies to any of the requested attributes, then the call should
    return the value CKR_ATTRIBUTE_TYPE_INVALID. If case 5 applies to any of the requested attributes,
    then the call should return the value CKR_BUFFER_TOO_SMALL. As usual, if more than one of these
    error codes is applicable, Cryptoki may return any of them. Only if none of them applies to any of the
    requested attributes will CKR_OK be returned.
    */
    {
      kms_ref_t  *FoundAttribute;
      uint32_t Index;
      uint32_t IndexData;

      for (Index = 0; Index < ulCount; Index++)
      {
        /* Search for the type of attribute from the Template */
        e_ret_status = KMS_Objects_SearchAttributes(pTemp->type, pkms_object, &FoundAttribute);

        if (e_ret_status == CKR_OK)
        {

          /* When we manage a Value, we translate from u32 to u8 ...*/
          if (pTemp->type == CKA_VALUE)
          {
            uint8_t *pValue = pTemp->pValue;

            /* case '4' = copy the value */
            if (pTemp->ulValueLen >= FoundAttribute->size)
            {
              for (IndexData = 0; IndexData < (FoundAttribute->size / 4UL); IndexData++)
              {
                /* Copy the data from u32 to u8 */
                pValue[IndexData * 4UL]         = (uint8_t)((FoundAttribute->data[IndexData] >> 24) & 0x000000FFUL);
                pValue[(IndexData * 4UL) + 1UL] = (uint8_t)((FoundAttribute->data[IndexData] >> 16) & 0x000000FFUL);
                pValue[(IndexData * 4UL) + 2UL] = (uint8_t)((FoundAttribute->data[IndexData] >> 8) & 0x000000FFUL);
                pValue[(IndexData * 4UL) + 3UL] = (uint8_t)(FoundAttribute->data[IndexData] & 0x000000FFUL);
              }
            }
            else
            {
              pTemp->ulValueLen = CKR_BUFFER_TOO_SMALL;
            }
          }
          else
          {
            if (pTemp->ulValueLen <= 4UL)
            {
              pTemp->pValue = FoundAttribute->data;
            }

          }
        }
        else
        {
          pTemp->ulValueLen = CK_UNAVAILABLE_INFORMATION;
        }

        pTemp++;
      }
    }

  }

  return e_ret_status;
}
#endif /* KMS_GET_ATTRIBUTE_VALUE */

#ifdef KMS_UNITARY_TEST
/* This function is to test the GetAttribute function. */
CK_RV    KMS_UnitaryTest_GetAttribute(CK_ULONG_PTR plNbPassed, CK_ULONG_PTR plNbFailed)
{
  uint8_t  ReadValue[32];
  uint32_t NbPassed = 0;
  uint32_t NbFailed = 0;
  uint32_t Index;
  CK_ATTRIBUTE Template;
  CK_RV    rv;
  /* The CKA_VALUE of ObjectID = 0 */
  static const uint8_t expected_value1[16] = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, \
                                              0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08
                                             };

  /* The CKA_VALUE of ObjectID = 5 */
  static const uint8_t expected_value2[32] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
                                               0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                                               0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
                                               0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
                                             };

  /* Fill in the template with attribute to retrieve */
  Template.type       = CKA_VALUE;
  Template.pValue     = ReadValue;
  Template.ulValueLen = 32;

  /* ObjectID 0, value = */
  /* CKA_VALUE,   16, 0xfeffe992, 0x8665731c, 0x6d6a8f94, 0x67308308, */

  rv = KMS_GetAttributeValue(0,  0, &Template, 1);

  if (rv == CKR_OK)
  {
    NbPassed++;
  }
  else
  {
    NbFailed ++;
  }

  /* At this stage, ReadValue should contains the CKA_VALUE of ObjectID = 0 */
  for (Index = 0; Index < 16UL; Index++)
  {
    if (expected_value1[Index] != ReadValue[Index])
    {
      NbFailed ++;
      break;
    }
  }
  /* If the 16 values are egals, consider that the test is passed */
  if (Index == 16UL)
  {
    NbPassed++;
  }


  /* ObjectID 5, CKA_EXTRACTABLE = TRUE */
  /* CKA_VALUE,  32, 0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781, */
  /*                 0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x0914dff4, */

  rv = KMS_GetAttributeValue(0,  5, &Template, 1);

  if (rv == CKR_OK)
  {
    NbPassed++;
  }
  else
  {
    NbFailed ++;
  }

  /* At this stage, ReadValue should contains the CKA_VALUE of ObjectID = 5 */
  for (Index = 0; Index < 32UL; Index++)
  {
    if (expected_value2[Index] != ReadValue[Index])
    {
      NbFailed ++;
      break;
    }
  }
  /* If the 16 values are egals, consider that the test is passed */
  if (Index == 32UL)
  {
    NbPassed++;
  }

  /* ObjectID 1 ==> no EXTRACTABLE attribute */
  /* HEre, we check that the GetAttribute will not be done for non EXTRACTABLE objects */
  rv = KMS_GetAttributeValue(0,  1, &Template, 1);

  /* Here we expect to have an error */
  if (rv == CKR_ATTRIBUTE_SENSITIVE)
  {
    NbPassed++;
  }
  else
  {
    NbFailed ++;
  }

  /* ObjectID 4 ==> EXTRACTABLE = FALSE */
  /* HEre, we check that the GetAttribute will not be done for non EXTRACTABLE objects */
  rv = KMS_GetAttributeValue(0,  4, &Template, 1);

  /* Here we expect to have an error */
  if (rv == CKR_ATTRIBUTE_SENSITIVE)
  {
    NbPassed++;
  }
  else
  {
    NbFailed ++;
  }



  *plNbPassed = NbPassed;
  *plNbFailed = NbFailed;


  return CKR_OK;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
