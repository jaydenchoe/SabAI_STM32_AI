/**
  ******************************************************************************
  * @file    kms_config.h
  * @author  MCD Application Team
  * @brief   This file contains configuration for Key Management Services (KMS)
  *          module functionalities.
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
#ifndef KMS_CONFIG_H
#define KMS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @defgroup KMS_Configuration Global configuration
  * @{
  */

/** @defgroup KMS_HW_Config Hardware
  * @{
  */

/**
  * @brief KMS_EXT_TOKEN_ENABLED Uncomment the below line to support External Token
  */
#define KMS_EXT_TOKEN_ENABLED


/**
  * @}
  */

/** @defgroup KMS_Storage_Config Storage
  * @{
  */

/**
  * @brief KMS_NVM_ENABLED Uncomment the below line to support Non Volatile
  *        Memory storage
  * @note  Mandatory to handle runtime objects addition through blob import or
  *        creation through specific KMS services
  */
#define KMS_NVM_ENABLED

/**
  * @brief KMS_NVM_DYNAMIC_ENABLED Uncomment the below line to support Non
  *        Volatile Memory storage
  * @note  Mandatory to handle runtime objects creation through specific KMS services
  * @note  Requires @ref KMS_NVM_ENABLED to be defined
  */
#define KMS_NVM_DYNAMIC_ENABLED

/**
  * @brief KMS_CHECK_PARAMS Uncomment the below line to not check the input
  *        parameters pointers
  * @note  When enclave is available, it is recommended to keep this control on
  */
#define KMS_CHECK_PARAMS


/**
  * @}
  */

/** @defgroup KMS_Config Services
  * @{
  */
/**
  * @brief KMS_NB_SESSIONS_MAX Defines the number of sessions KMS will be able
  *        to handle in parallel
  * @note  This value has an impact on memory footprint
  */
#define KMS_NB_SESSIONS_MAX                     10UL

/**
  * @}
  */

/** @defgroup KMS_Features_Config Features
  * @{
  */

/**
  * @brief KMS_AES_CBC Uncomment the below line to support AES CBC algorithm
  */
#define KMS_AES_CBC

/**
  * @brief KMS_AES_GCM Uncomment the below line to support AES GCM algorithm
  */
#define KMS_AES_GCM

/**
  * @brief KMS_AES_CMAC Uncomment the below line to support AES CMAC algorithm
  */
#define KMS_AES_CMAC

/**
  * @brief KMS_AES_CCM Uncomment the below line to support AES CCM algorithm
  */
#define KMS_AES_CCM

/**
  * @brief KMS_AES_CTR Uncomment the below line to support AES CTR algorithm
  */
#define KMS_AES_CTR

/**
  * @brief KMS_AES_ECB Uncomment the below line to support AES ECB algorithm
  */
#define KMS_AES_ECB

/**
  * @brief KMS_DIGEST Uncomment the below line to support digest services
  */
#define KMS_DIGEST

/**
  * @brief KMS_SIGN_VERIFY Uncomment the below line to support Sign and Verify
  *        services
  */
#define KMS_SIGN_VERIFY

/**
  * @brief KMS_RSA Uncomment the below line to support RSA algorithm for Sign
  *        and Verify services
  * @note  Requires @ref KMS_SIGN_VERIFY to be defined
  */
#define KMS_RSA

/**
  * @brief KMS_RSA_1024 Uncomment the below line to support RSA 1024 bits
  *        algorithm for Sign and Verify services
  * @note  Requires @ref KMS_RSA to be defined
  */
#define KMS_RSA_1024

/**
  * @brief KMS_RSA_2048 Uncomment the below line to support RSA 2048 bits
  *        algorithm for Sign and Verify services
  * @note  Requires @ref KMS_RSA to be defined
  */
#define KMS_RSA_2048


/**
  * @brief KMS_ECDSA Uncomment the below line to support ECDSA algorithm for
  *        Sign and Verify services
  * @note  Requires @ref KMS_SIGN_VERIFY to be defined
  */
#define KMS_ECDSA

/**
  * @brief KMS_ECDSA_SHA1 Uncomment the below line to support ECDSA with SHA1
  *        algorithm for Sign and Verify services
  * @note  Requires @ref KMS_ECDSA to be defined
  */
#define KMS_ECDSA_SHA1

/**
  * @brief KMS_ECDSA_SHA1 Uncomment the below line to support ECDSA with SHA256
  *        algorithm for Sign and Verify services
  * @note  Requires @ref KMS_ECDSA to be defined
  */
#define KMS_ECDSA_SHA256

/**
  * @brief KMS_DERIVE_KEY Uncomment the below line to support key derivation
  *        services
  * @note  Requires @ref KMS_NVM_DYNAMIC_ENABLED to be defined
  */
#define KMS_DERIVE_KEY

/**
  * @brief KMS_WRAP_KEY Uncomment the below line to support key wrapping
  *        services
  */
#define KMS_WRAP_KEY

/**
  * @brief KMS_GET_ATTRIBUTE_VALUE Uncomment the below line to support Get
  *        Attribute functionality
  */
#define KMS_GET_ATTRIBUTE_VALUE

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

