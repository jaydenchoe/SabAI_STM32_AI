/**
  ******************************************************************************
  * @file    kms_dyn_obj.c
  * @author  MCD Application Team
  * @brief   This file contains implementation for Key Management Services (KMS)
  *          module dynamic object management functionalities.
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
#include "kms_dyn_obj.h"
#include "kms_objects.h"                /* KMS object services */
#include "kms_nvm_storage.h"            /* KMS object storage services */
#include "kms_platf_objects.h"          /* KMS platf objects services */


/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_DYNOBJ Objects services
  * @{
  */


/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


/** @addtogroup KMS_DYNOBJ_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This function is called upon @ref C_CreateObject call
  * @note   Refer to @ref C_CreateObject function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  pTemplate object template
  * @param  ulCount attributes count in the template
  * @param  phObject object to be created
  * @retval Operation status
  */
CK_RV  KMS_CreateObject(CK_SESSION_HANDLE hSession,
                        CK_ATTRIBUTE_PTR pTemplate,
                        CK_ULONG ulCount,
                        CK_OBJECT_HANDLE_PTR phObject)
{
  (void)(hSession);
  (void)(pTemplate);
  (void)(ulCount);
  (void)(phObject);

  return CKR_FUNCTION_FAILED;
}

/**
  * @brief  This function is called upon @ref C_DestroyObject call
  * @note   Refer to @ref C_DestroyObject function description
  *         for more details on the APIs, parameters and possible returned values
  * @param  hSession session handle
  * @param  hObject object handle
  * @retval Operation status
  */
CK_RV  KMS_DestroyObject(CK_SESSION_HANDLE hSession,
                         CK_OBJECT_HANDLE hObject)
{
  CK_RV e_ret_status = CKR_FUNCTION_FAILED;
  kms_obj_keyhead_t *pkms_object;
  nvms_error_t rv_nvm;
  kms_ref_t  *pDeriveAttribute;

  (void)(hSession);
  /* Verify that the object is removable, reading the attributes */

  /* Read the key value from the Key Handle                 */
  /* Key Handle is the index to one of static or nvm        */
  pkms_object = KMS_Objects_GetPointer(hObject);

  /* Check that hKey is valid */
  if (pkms_object != NULL)
  {
    /* Check the CKA_DESTROYABLE attribute = CK_TRUE      */
    e_ret_status = KMS_Objects_SearchAttributes(CKA_DESTROYABLE, pkms_object, &pDeriveAttribute);

    /* If attribute not found or object not destroyable */
    if ((e_ret_status != CKR_OK) || (*(pDeriveAttribute->data) != CK_TRUE))
    {
      /* Object cannot be removed. */
      return (CKR_ACTION_PROHIBITED);
    }

    /* Object is removable */
    rv_nvm =   KMS_PlatfObjects_NvmRemoveObject(hObject);
    if (rv_nvm == NVMS_NOERROR)
    {
      KMS_PlatfObjects_NvmDynamicObjectList();
      e_ret_status = CKR_OK;
    }
  }

  return e_ret_status;
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
