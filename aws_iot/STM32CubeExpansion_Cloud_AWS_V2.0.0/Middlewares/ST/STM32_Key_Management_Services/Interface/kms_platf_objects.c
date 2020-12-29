/**
  ******************************************************************************
  * @file    kms_platf_objects.c
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module platform objects management
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
#include "kms_platf_objects.h"
#include "kms_nvm_storage.h"            /* KMS storage services */
#define KMS_PLATF_OBJECTS_C
#include "kms_platf_objects_config.h"   /* KMS embedded objects definitions */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_PLATF Platform Objects
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @addtogroup KMS_PLATF_Exported_Functions Exported Functions
  * @{
  */
/**
  * @brief  NVM initialization status
  */
static uint32_t kms_platf_nvm_initialisation_done = 0UL;

#ifdef KMS_NVM_ENABLED
/**
  * @brief  NVM static objects access variables
  * @note   This "cache" table is used to speed up access to NVM static objects
  */
static kms_obj_keyhead_t *KMS_PlatfObjects_NvmStaticList[KMS_INDEX_MAX_NVM_STATIC_OBJECTS -
                                                         KMS_INDEX_MIN_NVM_STATIC_OBJECTS];
#ifdef KMS_NVM_DYNAMIC_ENABLED
/**
  * @brief  NVM dynamic objects access variables
  * @note   This "cache" table is used to speed up access to NVM dynamic objects
  */
static kms_obj_keyhead_t *KMS_PlatfObjects_NvmDynamicList[KMS_INDEX_MAX_NVM_DYNAMIC_OBJECTS -
                                                          KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS];
#endif /* KMS_NVM_DYNAMIC_ENABLED */
#endif /* KMS_NVM_ENABLED */
/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_PLATF_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Returns range of embedded objects
  * @param  pMin Embedded objects min ID
  * @param  pMax Embedded objects max ID
  */
void             KMS_PlatfObjects_EmbeddedRange(uint32_t *pMin, uint32_t *pMax)
{

  *pMin = KMS_INDEX_MIN_EMBEDDED_OBJECTS;
  *pMax = KMS_INDEX_MAX_EMBEDDED_OBJECTS;

  return ;
}

/**
  * @brief  Returns embedded object corresponding to given key handle
  * @param  hKey key handle
  * @retval corresponding object
  */
kms_obj_keyhead_t *KMS_PlatfObjects_EmbeddedObject(uint32_t hKey)
{
  return (kms_obj_keyhead_t *)(uint32_t)KMS_PlatfObjects_EmbeddedList[hKey];
}

#ifdef KMS_NVM_ENABLED
/**
  * @brief  Returns range of NVM static objects
  * @param  pMin NVM static objects min ID
  * @param  pMax NVM static objects max ID
  */
void             KMS_PlatfObjects_NvmStaticRange(uint32_t *pMin, uint32_t *pMax)
{

  *pMin = KMS_INDEX_MIN_NVM_STATIC_OBJECTS;
  *pMax = KMS_INDEX_MAX_NVM_STATIC_OBJECTS;

  return ;
}

/**
  * @brief  Returns NVM static object corresponding to given key handle
  * @param  hKey key handle
  * @retval corresponding object
  */
kms_obj_keyhead_t *KMS_PlatfObjects_NvmStaticObject(uint32_t hKey)
{
  return KMS_PlatfObjects_NvmStaticList[hKey];
}

#ifdef KMS_NVM_DYNAMIC_ENABLED
/**
  * @brief  Returns range of NVM dynamic objects
  * @param  pMin NVM dynamic objects min ID
  * @param  pMax NVM dynamic objects max ID
  */
void             KMS_PlatfObjects_NvmDynamicRange(uint32_t *pMin, uint32_t *pMax)
{

  *pMin = KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS;
  *pMax = KMS_INDEX_MAX_NVM_DYNAMIC_OBJECTS;

  return ;
}

/**
  * @brief  Returns NVM dynamic object corresponding to given key handle
  * @param  hKey key handle
  * @retval corresponding object
  */
kms_obj_keyhead_t *KMS_PlatfObjects_NvmDynamicObject(uint32_t hKey)
{
  return KMS_PlatfObjects_NvmDynamicList[hKey];
}

/**
  * @brief  Returns an available NVM dynamic ID
  * @param  pObjId NVm dynamic ID
  * @retval CKR_DEVICE_MEMORY if none available, CKR_OK otherwise
  */
CK_RV KMS_PlatfObjects_AllocateNvmDynamicObjectId(CK_OBJECT_HANDLE_PTR pObjId)
{
  CK_OBJECT_HANDLE Index;

  /* Find a Free place in nvm dynamic table */
  for (Index = 0; Index < (KMS_INDEX_MAX_NVM_DYNAMIC_OBJECTS - KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS); Index++)
  {
    if (KMS_PlatfObjects_NvmDynamicList[Index] == NULL)
    {
      *pObjId = Index + KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS;
      return (CKR_OK);
    }
  }

  /* No place found in Dynamicl List */
  return CKR_DEVICE_MEMORY;
}

/**
  * @brief  Update @ref KMS_PlatfObjects_NvmDynamicList with NVM contents
  */
void KMS_PlatfObjects_NvmDynamicObjectList(void)
{
  nvms_error_t  nvms_rv;
  uint32_t      i;
  size_t nvms_DataSize;
  kms_obj_keyhead_t *nvms_pData;

  /* Load the KMS_PlatfObjects_NvmDynamicList[], used to store buffer to NVM. */
  /* This should save processiong time. */
  for (i = KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS; i < KMS_INDEX_MAX_NVM_DYNAMIC_OBJECTS; i++)
  {
    /* Read values from NVM */
    nvms_rv = NVMS_GetData(i, &nvms_DataSize, (uint8_t **)&nvms_pData);

    if ((nvms_DataSize != 0UL) && (nvms_rv == NVMS_NOERROR))
    {
      KMS_PlatfObjects_NvmDynamicList[i - KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS] = nvms_pData;
    }
    else
    {
      KMS_PlatfObjects_NvmDynamicList[i - KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS] = NULL;
    }

  }
}
#endif  /* KMS_NVM_DYNAMIC_ENABLED */
#endif  /* KMS_NVM_ENABLED */

#ifdef KMS_EXT_TOKEN_ENABLED
/**
  * @brief  Returns range of External token static objects
  * @param  pMin External token static objects min ID
  * @param  pMax External token static objects max ID
  */
void             KMS_PlatfObjects_ExtTokenStaticRange(uint32_t *pMin, uint32_t *pMax)
{

  *pMin = KMS_INDEX_MIN_EXT_TOKEN_STATIC_OBJECTS;
  *pMax = KMS_INDEX_MAX_EXT_TOKEN_STATIC_OBJECTS;

  return ;
}
/**
  * @brief  Returns range of External token dynamic objects
  * @param  pMin External token dynamic objects min ID
  * @param  pMax External token dynamic objects max ID
  */
void             KMS_PlatfObjects_ExtTokenDynamicRange(uint32_t *pMin, uint32_t *pMax)
{

  *pMin = KMS_INDEX_MIN_EXT_TOKEN_DYNAMIC_OBJECTS;
  *pMax = KMS_INDEX_MAX_EXT_TOKEN_DYNAMIC_OBJECTS;

  return ;
}
#endif /* KMS_EXT_TOKEN_ENABLED */


/**
  * @brief  Initialize platform objects
  * @note   Initialize NVM storage and fill "cache" buffers
  */
void   KMS_PlatfObjects_Init(void)
{

#ifdef KMS_NVM_ENABLED
  nvms_error_t  nvms_rv;
  uint32_t      i;
  size_t nvms_DataSize;
  kms_obj_keyhead_t *nvms_pData;

  /* The NVMS_Init should be done only once */
  KMS_PlatfObjects_NvmInitialize();

  /* Load the KMS_PlatfObjects_NvmStaticList[], used to store buffer to NVM. */
  /* This should save processiong time. */
  for (i = KMS_INDEX_MIN_NVM_STATIC_OBJECTS; i < KMS_INDEX_MAX_NVM_STATIC_OBJECTS; i++)
  {
    /* Read values from NVM */
    nvms_rv = NVMS_GetData(i, &nvms_DataSize, (uint8_t **)&nvms_pData);

    if ((nvms_DataSize != 0UL) && (nvms_rv == NVMS_NOERROR))
    {
      KMS_PlatfObjects_NvmStaticList[i - KMS_INDEX_MIN_NVM_STATIC_OBJECTS] = nvms_pData;
    }
    else
    {
      KMS_PlatfObjects_NvmStaticList[i - KMS_INDEX_MIN_NVM_STATIC_OBJECTS] = NULL;
    }

  }

#ifdef KMS_NVM_DYNAMIC_ENABLED
  KMS_PlatfObjects_NvmDynamicObjectList();
#endif /* KMS_NVM_DYNAMIC_ENABLED */

#endif /* KMS_NVM_ENABLED */


}

/**
  * @brief  De-Initialize platform objects
  */
void   KMS_PlatfObjects_Finalize(void)
{

}

/**
  * @brief  Initialize the NVM storage
  */
void   KMS_PlatfObjects_NvmInitialize(void)
{
#ifdef KMS_NVM_ENABLED
  /* The NVMS_Init should be done only once */
  if (kms_platf_nvm_initialisation_done == 0UL)
  {
    /* Initialize the NVMS */
    (void)NVMS_Init();
    kms_platf_nvm_initialisation_done = 1UL;
  }
#endif /* KMS_NVM_ENABLED */
}

/**
  * @brief  De-Initialize the NVM storage
  */
void   KMS_PlatfObjects_NvmFinalize(void)
{
#ifdef KMS_NVM_ENABLED

  /* Finalize the NVMS */
  NVMS_Deinit();

  /* We must re-allow the call to NVMS_Init() */
  kms_platf_nvm_initialisation_done = 0UL;

#endif /* KMS_NVM_ENABLED */
}

/**
  * @brief  Store object in NVM storage
  * @note   Either static or dynamic objects
  * @param  ObjectId Object ID
  * @param  pObjectToAdd Object to add
  * @param  ObjectSize Object size
  * @retval See @ref nvms_error_t
  */
nvms_error_t   KMS_PlatfObjects_NvmStoreObject(uint32_t ObjectId, uint8_t *pObjectToAdd,  uint32_t ObjectSize)
{
  nvms_error_t  rv;

#ifdef KMS_NVM_ENABLED

  /* It's a NVM STATIC object */
  if ((ObjectId >= KMS_INDEX_MIN_NVM_STATIC_OBJECTS) && (ObjectId <= KMS_INDEX_MAX_NVM_STATIC_OBJECTS))
  {
    rv = NVMS_WriteData(ObjectId, ObjectSize, (const uint8_t *)pObjectToAdd);
  }
  else
  {
#ifdef KMS_NVM_DYNAMIC_ENABLED
    /* It's a NVM DYNAMIC object */
    if ((ObjectId >= KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS) && (ObjectId <= KMS_INDEX_MAX_NVM_DYNAMIC_OBJECTS))
    {
      rv = NVMS_WriteData(ObjectId, ObjectSize, (const uint8_t *)pObjectToAdd);
    }
    else
    {
      rv = NVMS_SLOT_INVALID;
    }
#else /* KMS_NVM_DYNAMIC_ENABLED */
    rv = NVMS_SLOT_INVALID;
#endif /* KMS_NVM_DYNAMIC_ENABLED */
  }
#else /* KMS_NVM_ENABLED */
  rv = NVMS_NOTINIT;
#endif /* KMS_NVM_ENABLED */
  return (rv);
}


/**
  * @brief  Remove object from NVM storage
  * @note   Only dynamic objects
  * @param  ObjectId Object ID
  * @retval See @ref nvms_error_t
  */
nvms_error_t   KMS_PlatfObjects_NvmRemoveObject(uint32_t ObjectId)
{
  nvms_error_t rv = NVMS_DATA_NOT_FOUND;

  /* Check that the ObjectID is in dynamic range */
  if ((ObjectId >= KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS) && (ObjectId <= KMS_INDEX_MAX_NVM_DYNAMIC_OBJECTS))
  {

    rv = NVMS_EraseData(ObjectId);


  }
  return (rv);

}

/**
  * @brief  Returns Blob import verification key handle
  * @retval Key handle
  */
CK_ULONG        KMS_PlatfObjects_GetBlobVerifyKey(void)
{
  return (CK_ULONG)KMS_INDEX_BLOBIMPORT_VERIFY;
}

/**
  * @brief  Returns Blob import decryption key handle
  * @retval Key handle
  */
CK_ULONG        KMS_PlatfObjects_GetBlobDecryptKey(void)
{
  return (CK_ULONG)KMS_INDEX_BLOBIMPORT_DECRYPT;
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
