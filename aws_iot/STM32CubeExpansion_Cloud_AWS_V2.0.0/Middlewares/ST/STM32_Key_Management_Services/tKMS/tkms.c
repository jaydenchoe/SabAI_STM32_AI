/**
  ******************************************************************************
  * @file    tkms.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
  *          module of the PKCS11 APIs when securely enclaved into Secure Engine
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

/* Ensure code is kept even not used, whatever compiler */
#if   defined (__CC_ARM)
#define __root __attribute__((used))
#elif defined (__GNUC__)
#define __root __attribute__((used))
#elif defined (__ICCARM__)
#define __root __root
#endif /* __CC_ARM / __GNUC__ / __ICCARM__ */

/* Place code in a specific section*/
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".SE_IF_Code"
#elif defined(__CC_ARM)
#pragma arm section code = ".SE_IF_Code"
#endif /* __ICCARM__ / __CC_ARM */

/* Includes ------------------------------------------------------------------*/
#include "kms.h"
#include "se_interface_kms.h"


/** @addtogroup tKMS User interface to Key Management Services (tKMS)
  * @{
  */

/** @addtogroup KMS_TKMS tKMS
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup KMS_TKMS_Private_Variables Private Variables
  * @{
  */
static uint32_t TKMS_TokenInit = 0UL;    /*!< Token initialization state, used to avoid multiple init calls */

/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_TKMS_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief tKMS initialization function
  */
__root CK_RV C_Initialize(CK_VOID_PTR pInitArgs)
{

  /* If initialisation already done */
  if (TKMS_TokenInit == 1UL)
  {
    return CKR_CRYPTOKI_ALREADY_INITIALIZED;
  }
  else
  {
    TKMS_TokenInit = 1UL;
    return SE_KMS_Initialize(pInitArgs);
  }
}

/**
  * @brief tKMS finalization function
  */
__root CK_RV C_Finalize(CK_VOID_PTR pReserved)
{
  TKMS_TokenInit = 0UL;
  return SE_KMS_Finalize(pReserved);
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

/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_function_attributes =
#elif defined(__CC_ARM)
#pragma arm section code
#endif /* __ICCARM__ / __CC_ARM */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
