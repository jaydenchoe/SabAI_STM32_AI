/**
  ******************************************************************************
  * @file    kms_blob_headers.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services module
  *          Blob objects
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KMS_BLOB_HEADERS_H
#define KMS_BLOB_HEADERS_H

/* Includes ------------------------------------------------------------------*/
#include "pkcs11.h"



#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_OBJECTS Blob Objects Management
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @addtogroup KMS_OBJECTS_Exported_Constants Exported Constants
  * @{
  */
/* define adapted to identify Objects in Blobs    */
/*  upper value = 0xB10B on both Version & CONFIG */
/**
  * @brief KMS ABI version
  * @note  Starts with 0xB10B for "Blob"
  * @note  Match with <a href=http://docs.oasis-open.org/pkcs11/pkcs11-base/v2.40/os/pkcs11-base-v2.40-os.html>
  *        PKCS #11 Cryptographic Token Interface Base Specification Version 2.40</a>
  */
#define KMS_ABI_VERSION_CK_2_40           0xB10B0240U
/**
  * @brief KMS Blob objects structure version
  * @note  Starts with 0xB10B for "Blob"
  */
#define KMS_ABI_CONFIG_KEYHEAD            0xB10B0003U

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup KMS_OBJECTS_Exported_Types Exported Types
  * @{
  */
/* These structures are used even for static & dynamic object definition */

/**
  * @brief KMS Blob header structure definition
  * @note  Not used to store objects cause contains no data but to ease access
  *        to variable length blob objecs
  */
typedef struct
{
  uint32_t version;               /*!< ABI version in use: must be @ref KMS_ABI_VERSION_CK_2_40 */
  uint32_t configuration;         /*!< Blob version in use: must be @ref KMS_ABI_CONFIG_KEYHEAD */
  uint32_t blobs_size;            /*!< Blob size */
  uint32_t blobs_count;           /*!< Blob count */
  uint32_t class;                 /*!< Blob class */
  uint32_t type;                  /*!< Blob type */
  uint32_t object_id;             /*!< Blob object ID */
} kms_obj_keyhead_no_blob_t;

/**
  * @brief KMS Blob object structure definition
  * @note  Can contains up to 128/4 = 32 blobs data element
  */
typedef struct
{
  uint32_t version;               /*!< ABI version in use: must be @ref KMS_ABI_VERSION_CK_2_40 */
  uint32_t configuration;         /*!< Blob version in use: must be @ref KMS_ABI_CONFIG_KEYHEAD */
  uint32_t blobs_size;            /*!< Blob size */
  uint32_t blobs_count;           /*!< Blob count */
  uint32_t class;                 /*!< Blob class */
  uint32_t type;                  /*!< Blob type */
  uint32_t object_id;             /*!< Blob object ID */
  uint32_t blobs[128 / 4];        /*!< Blob object data elements */
} kms_obj_keyhead_t;

/**
  * @brief KMS Blob object structure definition
  * @note  Can contains up to 256/4 = 64 blobs data element
  */
typedef struct
{
  uint32_t version;               /*!< ABI version in use: must be @ref KMS_ABI_VERSION_CK_2_40 */
  uint32_t configuration;         /*!< Blob version in use: must be @ref KMS_ABI_CONFIG_KEYHEAD */
  uint32_t blobs_size;            /*!< Blob size */
  uint32_t blobs_count;           /*!< Blob count */
  uint32_t class;                 /*!< Blob class */
  uint32_t type;                  /*!< Blob type */
  uint32_t object_id;             /*!< Blob object ID */
  uint32_t blobs[256 / 4];        /*!< Blob object data elements */
} kms_obj_keyhead_256_t;

#if !defined(KMS_SIGN_VERIFY)
/**
  * @brief KMS Blob object biggest structure definition
  * @note  Can contains up to 256/4 = 64 blobs data element
  */
typedef kms_obj_keyhead_256_t biggest_kms_obj_keyhead_t;

#else /* KMS_SIGN_VERIFY */
/**
  * @brief KMS Blob object structure definition
  * @note  Can contains up to 512/4 = 128 blobs data element
  */
typedef struct
{
  uint32_t version;               /*!< ABI version in use: must be @ref KMS_ABI_VERSION_CK_2_40 */
  uint32_t configuration;         /*!< Blob version in use: must be @ref KMS_ABI_CONFIG_KEYHEAD */
  uint32_t blobs_size;            /*!< Blob size */
  uint32_t blobs_count;           /*!< Blob count */
  uint32_t class;                 /*!< Blob class */
  uint32_t type;                  /*!< Blob type */
  uint32_t object_id;             /*!< Blob object ID */
  uint32_t blobs[512 / 4];        /*!< Blob object data elements */
} kms_obj_keyhead_512_t;

/**
  * @brief KMS Blob object structure definition
  * @note  Can contains up to 768/4 = 192 blobs data element
  */
typedef struct
{
  uint32_t version;               /*!< ABI version in use: must be @ref KMS_ABI_VERSION_CK_2_40 */
  uint32_t configuration;         /*!< Blob version in use: must be @ref KMS_ABI_CONFIG_KEYHEAD */
  uint32_t blobs_size;            /*!< Blob size */
  uint32_t blobs_count;           /*!< Blob count */
  uint32_t class;                 /*!< Blob class */
  uint32_t type;                  /*!< Blob type */
  uint32_t object_id;             /*!< Blob object ID */
  uint32_t blobs[768 / 4];        /*!< Blob object data elements */
} kms_obj_keyhead_768_t;

/**
  * @brief KMS Blob object structure definition
  * @note  Can contains up to 1024/256 = 32 blobs data element
  */
typedef struct
{
  uint32_t version;               /*!< ABI version in use: must be @ref KMS_ABI_VERSION_CK_2_40 */
  uint32_t configuration;         /*!< Blob version in use: must be @ref KMS_ABI_CONFIG_KEYHEAD */
  uint32_t blobs_size;            /*!< Blob size */
  uint32_t blobs_count;           /*!< Blob count */
  uint32_t class;                 /*!< Blob class */
  uint32_t type;                  /*!< Blob type */
  uint32_t object_id;             /*!< Blob object ID */
  uint32_t blobs[1024 / 4];       /*!< Blob object data elements */
} kms_obj_keyhead_1024_t;

/**
  * @brief KMS Blob object structure definition
  * @note  Can contains up to 1536/4 = 384 blobs data element
  */
typedef struct
{
  uint32_t version;               /*!< ABI version in use: must be @ref KMS_ABI_VERSION_CK_2_40 */
  uint32_t configuration;         /*!< Blob version in use: must be @ref KMS_ABI_CONFIG_KEYHEAD */
  uint32_t blobs_size;            /*!< Blob size */
  uint32_t blobs_count;           /*!< Blob count */
  uint32_t class;                 /*!< Blob class */
  uint32_t type;                  /*!< Blob type */
  uint32_t object_id;             /*!< Blob object ID */
  uint32_t blobs[1536 / 4];       /*!< Blob object data elements */
} kms_obj_keyhead_1536_t;

/**
  * @brief KMS Blob object biggest structure definition
  * @note  Can contains up to 1536/4 = 384 blobs data element
  */
typedef kms_obj_keyhead_1536_t biggest_kms_obj_keyhead_t;
#endif /* KMS_SIGN_VERIFY */

/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* KMS_BLOB_HEADERS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
