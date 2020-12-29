/**
  ******************************************************************************
  * @file    wrap_config.h
  * @author  MCD Application Team
  * @brief   File to choose which functions
             are to be used with the wrapper
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
#ifndef  WRAP_CONFIG_H
#define WRAP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define TIMEOUT_VALUE           0xFF
#define WRAP_AES                1       /*!< Use of AES's algorithms*/
#define WRAP_AES_CBC            1       /*!< Use of AES CBC*/
#define WRAP_AES_GCM            1       /*!< Use of AES GCM*/
#define WRAP_AES_CCM            1       /*!< Use of AES CCM*/
#define WRAP_AES_CMAC           1       /*!< Use of AES CMAC*/
#define WRAP_AES_EBC            1       /*!< Use of AES EBC*/
#define WRAP_AES_CTR            1       /*!< Use of AES CTR*/

#define WRAP_HASH               1       /*!< Use of Digest's algorithms*/
#define WRAP_SHA_1              1       /*!< Use of SHA1*/
#define WRAP_SHA_256            1       /*!< Use of SHA1*/

#define WRAP_ECDSA              1       /*!< Use of ECDSA*/
#define WRAP_RSA                1       /*!< Use of RSA*/

#define T_VALUE_CCM             4

#if defined(WRAP_AES_CBC) && !defined(WRAP_AES)
#error "CBC is currently supported only by AES, but AES has not be included, please include AES or remove CBC"
#endif /* WRAP_AES_CBC && !defined(WRAP_AES) */

#if defined(WRAP_AES_GCM) && !defined(WRAP_AES)
#error "GCM is currently supported only by AES, but AES has not be included, please include AES or remove GCM"
#endif /* WRAP_AES_GCM && !defined(WRAP_AES) */

#if defined(WRAP_AES_CMAC) && !defined(WRAP_AES)
#error "CMAC is currently supported only by AES, but AES has not be included, please include AES or remove CMAC"
#endif /* WRAP_AES_CMAC && !defined(WRAP_AES) */

#if defined(WRAP_AES_CCM) && !defined(WRAP_AES)
#error "CCM is currently supported only by AES, but AES has not be included, please include AES or remove CCM"
#endif /* WRAP_AES_CCM && !defined(WRAP_AES) */

#if defined(WRAP_AES_ECB) && !defined(WRAP_AES)
#error "ECB is currently supported only by AES, but AES has not be included, please include AES or remove ECB"
#endif /* WRAP_AES_ECB && !defined(WRAP_AES) */

#if defined(WRAP_AES_CTR) && !defined(WRAP_AES)
#error "CTR is currently supported only by AES, but AES has not be included, please include AES or remove CTR"
#endif /* WRAP_AES_CTR && !defined(WRAP_AES) */

#if defined(WRAP_SHA_1) && !defined(WRAP_HASH)
#error "HASH has not be included, please include HASH or remove SHA_1"
#endif /* WRAP_SHA_1 && !defined(WRAP_HASH) */

#if defined(WRAP_SHA_256) && !defined(WRAP_HASH)
#error "HASH has not be included, please include HASH or remove SHA_256"
#endif /* WRAP_SHA_256 && !defined(WRAP_HASH) */

/* Exported functions ------------------------------------------------------- */
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*WRAP_CONFIG_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
