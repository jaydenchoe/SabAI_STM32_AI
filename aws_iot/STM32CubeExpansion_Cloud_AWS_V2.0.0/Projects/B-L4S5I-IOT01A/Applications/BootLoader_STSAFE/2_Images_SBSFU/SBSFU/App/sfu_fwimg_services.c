/**
  ******************************************************************************
  * @file    sfu_fwimg_services.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage the Firmware Images.
  *          This file contains the services the bootloader (sfu_boot) can use to deal
  *          with the FW images handling.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#define SFU_FWIMG_SERVICES_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#include "sfu_low_level_flash.h"
#include "sfu_low_level_security.h"
#include "sfu_low_level.h"
#include "sfu_fwimg_services.h"
#include "sfu_fwimg_internal.h"
#include "sfu_trace.h"
#ifdef ENABLE_IMAGE_STATE_HANDLING
#include "se_def_metadata.h"
#include "se_interface_bootloader.h"
#include "sfu_scheme_x509_core.h"
#endif /* ENABLE_IMAGE_STATE_HANDLING */

#ifdef ENABLE_IMAGE_STATE_HANDLING
static SFU_ErrorStatus SFU_IMG_FindBackupStart(uint8_t** pHdr);
#endif /* ENABLE_IMAGE_STATE_HANDLING */

/** @addtogroup SFU Secure Boot / Secure Firmware Update
  * @{
  */

/** @addtogroup SFU_CORE SBSFU Application
  * @{
  */

/** @addtogroup SFU_IMG SFU Firmware Image
  * @{
  */
  uint8_t gRollbackDone = 0; /* indicates if a rollback has been performed : used in sfu_boot.c */

/** @defgroup SFU_IMG_SERVICES SFU Firmware Image Services
  * @brief FW handling services the bootloader can call.
  * @{
  */

/** @defgroup SFU_IMG_Exported_Functions FW Images Handling Services
  * @brief Services the bootloader can call to handle the FW images.
  * @{
  */

/** @defgroup SFU_IMG_Initialization_Functions FW Images Handling Init. Functions
  * @brief FWIMG initialization functions.
  * @{
  */

/**
  * @brief FW Image Handling (FWIMG) initialization function.
  *        Checks the validity of the settings related to image handling (slots size and alignments...).
  * @note  The system initialization must have been performed before calling this function (flash driver ready to be
  *        used...etc...).
  *        Must be called first (and once) before calling the other Image handling services.
  * @param  None.
  * @retval SFU_IMG_InitStatusTypeDef SFU_IMG_INIT_OK if successful, an error code otherwise.
  */
SFU_IMG_InitStatusTypeDef SFU_IMG_InitImageHandling(void)
{
  /*
   * At the moment there is nothing more to do than initializing the fwimg_core part.
   */
  return (SFU_IMG_CoreInit());
}

/**
  * @brief FW Image Handling (FWIMG) de-init function.
  *        Terminates the images handling service.
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_ShutdownImageHandling(void)
{
  /*
   * At the moment there is nothing more to do than shutting down the fwimg_core part.
   */
  return (SFU_IMG_CoreDeInit());
}

/**
  * @}
  */

/** @defgroup SFU_IMG_Service_Functions FW Images Handling Services Functions
  * @brief FWIMG services functions.
  * @{
  */

/** @defgroup SFU_IMG_Service_Functions_NewImg New image installation services
  * @brief New image installation functions.
  * @{
  */

/**
  * @brief Checks if there is a pending firmware installation.
  *        3 situations can occur:
  *        A. Pending firmware installation: a firmware is ready to be installed but the installation has never been
  *           triggered so far.
  *        B. Pending firmware installation resume: a firmware installation has been interrupted and a resume procedure
  *           is required to finalize installation.
  *        C. No firmware installation pending
  * @note  The anti-rollback check is not taken into account at this stage.
  *        But, if the firmware to be installed already carries some metadata (VALID tag) a newly downloaded firmware
  *        should not have then the installation is not considered as pending.
  * @note This function populates the FWIMG module variables: fw_image_header_to_test, fw_header_to_test
  * @param  None.
  * @retval SFU_IMG_ImgInstallStateTypeDef Pending Installation status (pending install, pending resume install, no
  *         pending action)
  */
SFU_IMG_ImgInstallStateTypeDef SFU_IMG_CheckPendingInstallation(void)
{
  SFU_IMG_ImgInstallStateTypeDef e_ret_state = SFU_IMG_NO_FWUPDATE;
  /*
   * The order of the checks is important:
   *
   * A. Check if a FW update has been interrupted first.
   *    This check is based on the content of the trailer area and the validity of the update image.
   *
   * B. Check if a Firmware image is waiting to be installed.
   */
  if (SFU_SUCCESS == SFU_IMG_CheckTrailerValid())
  {
    /* A Firmware Update has been stopped */
    e_ret_state = SFU_IMG_FWUPDATE_STOPPED;
  }
  else if (SFU_SUCCESS == SFU_IMG_FirmwareToInstall())
  {
    /*
     * A Firmware is available for installation:
     * fw_image_header_to_test and fw_header_to_test have been populated
     */
    e_ret_state = SFU_IMG_FWIMAGE_TO_INSTALL;
  }
  else
  {
    /* No interrupted FW update and no pending update */
    e_ret_state = SFU_IMG_NO_FWUPDATE;
  }

  return e_ret_state;
}

/**
  * @brief Verifies the validity of the metadata associated to a candidate image.
  *        The anti-rollback check is performed.
  *        Errors can be memorized in the bootinfo area but no action is taken in this procedure.
  * @note It is up to the caller to make sure the conditions to call this primitive are met (no check performed before
  *       running the procedure):
  *       SFU_IMG_CheckPendingInstallation should be called first.
  * @note If this check fails the candidate Firmware is altered in FLASH to invalidate it.
  * @note For the local loader (@ref SECBOOT_USE_LOCAL_LOADER), the candidate metadata is verified by:
  *       @ref SFU_APP_VerifyFwHeader.
  *       But, SFU_IMG_CheckCandidateMetadata will be called when the installation of the new image is triggered.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_CheckCandidateMetadata(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_FLASH_StatusTypeDef flash_if_status;
  int32_t curVer;

  /*
   * ##0 - FW Header Authentication
   *       An authentication check should be performed.
   *
   *       In this example, we bypass this check because SFU_IMG_CheckPendingInstallation() has been called first.
   *       Therefore we know the header is fine.
   *
   *       As a consequence: fw_image_header_to_test must have been populated first (by
   *       SFU_IMG_CheckPendingInstallation)
   */
  SFU_ErrorStatus authCheck = SFU_SUCCESS;

  if (SFU_SUCCESS == authCheck)
  {
    /*
     * ##1 - Check version is allowed
     *
     *      SFU_IMG_GetActiveFwVersion() returns -1 if there is no active firmware but can return a value if a bricked
     *      FW is present with a valid header.
     */

    curVer = SFU_IMG_GetActiveFwVersion();

    if (SFU_IMG_CheckFwVersion(curVer, (int32_t)fw_image_header_to_test.FwVersion) != SFU_SUCCESS)
    {
      /* The installation is forbidden */
#if defined(SFU_VERBOSE_DEBUG_MODE)
      /* a FW with a lower version cannot be installed */
      TRACE("\r\n          Anti-rollback: candidate version(%d) rejected | current version(%d) , min.version(%d) !",
            fw_image_header_to_test.FwVersion, curVer, SFU_FW_VERSION_START_NUM);
#endif /* SFU_VERBOSE_DEBUG_MODE */

      /* Memorize this error as this will be handled as a critical failure */
      (void)SFU_BOOT_SetLastExecError(SFU_EXCPT_INCORRECT_VERSION);

      /* We would enter an infinite loop of installation attempts if we do not clean-up the swap sector */
      e_ret_status = SFU_LL_FLASH_Erase_Size(&flash_if_status, (void *)SFU_IMG_SWAP_REGION_BEGIN, SFU_IMG_IMAGE_OFFSET);
      StatusFWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);

      /* leave now to handle the critical failure in the appropriate FSM state */
      e_ret_status = SFU_ERROR;
    }
    else
    {
      /* the anti-rollback check succeeds: the version is fine */
      e_ret_status = SFU_SUCCESS;
    }
  }
  else
  {
    /* authentication failure: in this example, this code is unreachable */
    e_ret_status = SFU_ERROR; /* unreachable */
  }

  /* Return the result */
  return (e_ret_status);
}

/**
  * @brief Installs a new firmware, performs the post-installation checks and sets the metadata to tag this firmware as
  *        valid if the checks are successful.
  *        The anti-rollback check is performed and errors are memorized in the bootinfo area, but no action is taken
  *        in this procedure.
  *        Cryptographic operations are used (if enabled): the firmware is decrypted and its authenticity is checked
  *        afterwards if the cryptographic scheme allows this (signature check for instance).
  *        The detailed errors are memorized in bootinfo area to be processed as critical errors if needed.
  *        This function modifies the FLASH content.
  *        If this procedure is interrupted during image preparation stage (e.g.: switch off) it cannot be resumed.
  *        If it is interrupted during installation stage, it can be resumed.
  * @note  It is up to the caller to make sure the conditions to call this primitive are met (no check performed before
  *        running the procedure): SFU_IMG_CheckPendingInstallation, SFU_IMG_CheckCandidateMetadata should be called
  *        first.
  *        fw_image_header_to_test to be populated before calling this function.
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_TriggerImageInstallation(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /*
   * In this example, the preparation stage consists in decrypting the candidate firmware image.
   *
   */
  e_ret_status = SFU_IMG_PrepareCandidateImageForInstall();

  if (SFU_SUCCESS == e_ret_status)
  {
    /*
     * In this example, installing the new firmware image consists in swapping the 2 FW images.
     */
    e_ret_status = SFU_IMG_InstallNewVersion();
  }
  else
  {
    /* Erase downloaded FW in case of authentication/integrity error */
    (void) SFU_IMG_EraseDownloadedImg();
  }

  /* return the installation result */
  return (e_ret_status);
}

/**
  * @}
  */

/** @defgroup SFU_IMG_Service_Functions_Reinstall Image re-installation services
  * @brief Image re-installation functions.
  * @{
  */

/**
  * @brief Resume installation of new valid firmware after installation failure.
  *        If this procedure is interrupted before its completion (e.g.: switch off) it can be resumed.
  *        This function is dedicated (only) to the resume of installation when a new firmware installation is stopped
  *        before its completion.
  *        This function does not handle explicitly the metadata tagging the firmware as valid: these metadata must
  *        have been preserved during the installation procedure.
  *        This function modifies the FLASH content.
  * @note  It is up to the caller to make sure the conditions to call this primitive are met (no check performed before
  *        running the procedure): need to make sure an installation has been interrupted.
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_TriggerResumeInstallation(void)
{
  /*
   * Nothing more to do than calling the core procedure
   * The core procedure is kept because we do not want code dealing with the internals (like trailers) in this file.
   */
  return (SFU_IMG_Resume());
}

/**
  * @}
  */

/** @defgroup SFU_IMG_Service_Functions_CurrentImage Active Firmware services (current image)
  * @brief Active Firmware Image functions.
  * @{
  */

/**
  * @brief This function makes sure the current active firmware will not be considered as valid any more.
  *        This function alters the FLASH content.
  * @note It is up to the caller to make sure the conditions to call this primitive are met (no check performed before
  *       running the procedure).
  * @param  None.
  * @retval SFU_SUCCESS if successful,SFU_ERROR error otherwise.
  */
SFU_ErrorStatus SFU_IMG_InvalidateCurrentFirmware(void)
{
  SFU_FLASH_StatusTypeDef x_flash_info;
  SFU_ErrorStatus e_ret_status;

  /* Reload Watchdog */
  SFU_LL_SECU_IWDG_Refresh();

  /* erase Slot0 except Header (under SECoreBin protection) for the anti-rollback check during next Fw update */
  e_ret_status = SFU_LL_FLASH_CleanUp(&x_flash_info, SFU_IMG_SLOT_0_REGION_BEGIN + SFU_IMG_IMAGE_OFFSET,
                                      SFU_IMG_SLOT_0_REGION_SIZE - SFU_IMG_IMAGE_OFFSET);

  return (e_ret_status);
}

/**
  * @brief Verifies the validity of the active firmware image metadata.
  * @note This function relies on cryptographic procedures and it is up to the caller to make sure the required
  *       elements have been configured.
  * @note This function populates the FWIMG module variable: fw_image_header_validated
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveImgMetadata(void)
{
  /*
   * The active FW slot is slot #0.
   * If the metadata is valid then 'fw_image_header_validated' is filled with the metadata.
   */
  return (SFU_IMG_GetFWInfoMAC(&fw_image_header_validated, 0));
}

/**
  * @brief Verifies the validity of the active firmware image.
  * @note This function relies on cryptographic procedures and it is up to the caller to make sure the required
  *       elements have been configured.
  *       Typically, SFU_IMG_VerifyActiveImgMetadata() must have been called first to populate
  *       fw_image_header_validated.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveImg(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SE_StatusTypeDef e_se_status = SE_KO;

  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   * slot #0 is the active FW image
   */
  e_ret_status = SFU_IMG_VerifyFwSignature(&e_se_status, &fw_image_header_validated, 0U, SE_FW_IMAGE_COMPLETE);
#if defined(SFU_VERBOSE_DEBUG_MODE)
  if (SFU_ERROR == e_ret_status)
  {
    /* We do not memorize any specific error, the FSM state is already providing the info */
    TRACE("\r\n=         SFU_IMG_VerifyActiveImg failure with se_status=%d!", e_se_status);
  }
#endif /* SFU_VERBOSE_DEBUG_MODE */

  return (e_ret_status);
}

/**
  * @brief Verifies if no malicious code beyond Fw Image in Slot #0.
  * @note SFU_IMG_VerifyActiveImgMetadata() must have been called first to populate fw_image_header_validated.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveSlot(void)
{
  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   * slot #0 is the active FW image
   */

#if defined(SFU_BYPASS_ACTIVE_SLOT_VERIF)
	/* Bypass the check */
	return(SFU_SUCCESS);
#else
  return (SFU_IMG_VerifySlot(SFU_IMG_SLOT_0_REGION_BEGIN, SFU_IMG_SLOT_0_REGION_SIZE,
                             fw_image_header_validated.FwSize));
#endif /* SFU_BYPASS_ACTIVE_SLOT_VERIF */
}

/**
  * @brief Verifies if no malicious code in Slot #0.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyEmptyActiveSlot(void)
{
  return (SFU_IMG_VerifySlot(SFU_IMG_SLOT_0_REGION_BEGIN, SFU_IMG_SLOT_0_REGION_SIZE, 0U));
}

/**
  * @brief  Control slot#0 FW tag
  * @note   This control will be done twice for security reasons (first control done in VerifyTagScatter)
  * @param  None
  * @retval SFU_SUCCESS if successful,SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_ControlActiveImgTag(void)
{
  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   * slot #0 is the active FW image
   */
  return (SFU_IMG_ControlFwTag(fw_image_header_validated.FwTag));
}

/**
  * @brief Launches the user application.
  *        The caller must be prepared to never get the hand back after calling this function.
  *        If a problem occurs, it must be memorized in the bootinfo area.
  *        If the caller gets the hand back then this situation must be handled as a critical error.
  * @note It is up to the caller to make sure the conditions to call this primitive are met
  *       (typically: no security check performed before launching the firmware).
  * @note This function only handles the "system" aspects.
  *       It is up to the caller to manage any security related action (enable ITs, disengage MPU, clean RAM...).
  *       Nevertheless, cleaning-up the stack and heap used by SB_SFU is part of the system actions handled by this
  *       function (as this needs to be done just before jumping into the user application).
  * @param  None.
  * @retval SFU_ErrorStatus Does not return if successful, returns SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_LaunchActiveImg(void)
{
  uint32_t jump_address ;
  typedef void (*Function_Pointer)(void);
  Function_Pointer  p_jump_to_function;

#if defined(SFU_MPU_PROTECT_ENABLE)
#if defined(SFU_MPU_USERAPP_ACTIVATION)
  SFU_LL_SECU_SetProtectionMPU_UserApp();
#else
  HAL_MPU_Disable();
#endif /* SFU_MPU_USERAPP_ACTIVATION */
#endif /* SFU_MPU_PROTECT_ENABLE */

  jump_address = *(__IO uint32_t *)(((uint32_t)SFU_IMG_SLOT_0_REGION_BEGIN + SFU_IMG_IMAGE_OFFSET + 4));
  /* Jump to user application */
  p_jump_to_function = (Function_Pointer) jump_address;
  /* Initialize user application's Stack Pointer */
  __set_MSP(*(__IO uint32_t *)(SFU_IMG_SLOT_0_REGION_BEGIN + SFU_IMG_IMAGE_OFFSET));

  /* Destroy the Volatile data and CSTACK in SRAM used by Secure Boot in order to prevent any access to sensitive data
     from the UserApp.
     If FWall is used and a Secure Engine CStack context switch is implemented, these data are the data
     outside the protected area, so not so sensitive, but still in this way we add increase the level of security.
  */
  SFU_LL_SB_SRAM_Erase();
  /* JUMP into User App */
  p_jump_to_function();


  /* The point below should NOT be reached */
  return (SFU_ERROR);
}

/**
  * @brief Get the version of the active FW (present in slot #0)
  * @note It is up to the caller to make sure the slot #0 contains a valid active FW.
  * @note In the current implementation the header is checked (authentication) and no version is returned if this check
  *       fails.
  * @param  None.
  * @retval the FW version if it succeeds (coded on uint16_t), -1 otherwise
  */
int32_t SFU_IMG_GetActiveFwVersion(void)
{
  SE_FwRawHeaderTypeDef fw_image_header;
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  int32_t version = -1;

  /*  check the header in slot #0 */
  e_ret_status = SFU_IMG_GetFWInfoMAC(&fw_image_header, 0U);
  if (e_ret_status == SFU_SUCCESS)
  {
    /* retrieve the version from the header without any further check */
    version = (int32_t)fw_image_header.FwVersion;
  }

  return (version);
}

/**
  * @brief Indicates if a valid active firmware image is installed.
  *        It performs the same checks as SFU_IMG_VerifyActiveImgMetadata and SFU_IMG_VerifyActiveImg
  *        but it also verifies if the Firmware image is internally tagged as valid.
  * @note This function modifies the FWIMG variables 'fw_image_header_validated' and 'fw_header_validated'
  * @param  None.
  * @retval SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_HasValidActiveFirmware(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Refresh Watchdog */
  SFU_LL_SECU_IWDG_Refresh();

  /* check the installed header (metadata) */
  e_ret_status = SFU_IMG_VerifyActiveImgMetadata();

  /* Refresh Watchdog */
  SFU_LL_SECU_IWDG_Refresh();

  if (e_ret_status == SFU_SUCCESS)
  {
    /* check the installed FW image itself */
    e_ret_status = SFU_IMG_VerifyActiveImg();
  }

  /* Refresh Watchdog */
  SFU_LL_SECU_IWDG_Refresh();

  if (e_ret_status == SFU_SUCCESS)
  {
    /* check that this FW has been tagged as valid by the bootloader  */
    e_ret_status = SFU_IMG_CheckSlot0FwValid();
  }

  return e_ret_status;
}

/**
  * @brief  Validate the active FW image in slot #0 by installing the header and the VALID tags
  * @param  address of the header to be installed in slot #0
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_Validation(uint8_t *pHeader)
{
  return (SFU_IMG_WriteHeaderValidated(pHeader));
}

/**
  * @brief  Erase downloaded firmware in case of decryption/authentication/integrity error
  * @param  None
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_EraseDownloadedImg(void)
{
  SFU_FLASH_StatusTypeDef flash_if_status;
  SFU_ErrorStatus e_ret_status_dwl = SFU_SUCCESS;
  SFU_ErrorStatus e_ret_status_swap = SFU_SUCCESS;

  e_ret_status_dwl = SFU_LL_FLASH_Erase_Size(&flash_if_status, SFU_IMG_SLOT_DWL_REGION_BEGIN,
                                             SFU_IMG_SLOT_DWL_REGION_SIZE);
  StatusFWIMG(e_ret_status_dwl == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);

  e_ret_status_swap = SFU_LL_FLASH_Erase_Size(&flash_if_status, SFU_IMG_SWAP_REGION_BEGIN, SFU_IMG_SWAP_REGION_SIZE);
  StatusFWIMG(e_ret_status_swap == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);

  return (((e_ret_status_swap != SFU_SUCCESS) || (e_ret_status_dwl != SFU_SUCCESS)) ?  SFU_ERROR : SFU_SUCCESS);
}

/**
  * @brief Verifies if the slot #1 is erased.
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyDownloadSlot(void)
{
  return (SFU_IMG_VerifySlot(SFU_IMG_SLOT_DWL_REGION_BEGIN, SFU_IMG_SLOT_DWL_REGION_SIZE, 0));
}

#ifdef ENABLE_IMAGE_STATE_HANDLING
/**
  * @brief Checks and updates the state of the current firmware
  * @param None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful and image state trasition is OK, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_CheckImageState()
{
  SFU_ErrorStatus e_ret_status_sfu = SFU_ERROR;
  SE_ErrorStatus e_ret_status_se = SE_ERROR;
  SE_FwStateTypeDef eImageState = FWIMG_STATE_UNKNOWN;
  SE_StatusTypeDef eSE_Status;

  /* Read Image state
   * if image is new, set to self test, return success
   * if image is selftest, set to invalid, return error
   * if image is valid return success
   * if image is invalid or unknown return error
   */
  e_ret_status_se = SE_SFU_IMG_GetActiveFwState(&eSE_Status, &eImageState);
  if(e_ret_status_se == SE_SUCCESS)
  {
    TRACE("\r\n=         SFU_IMG_CheckImageState Image State = %d", eImageState);
    switch(eImageState)
    {
      case FWIMG_STATE_NEW:
      {
        /* An OTA image arrives on the device in the NEW state
         * The bootloader switches the new image to SELFTEST state so
         * that when the image boots it can detect that it should carry out the
         * self test routine
         */
        SE_FwStateTypeDef eTmpImageState = FWIMG_STATE_SELFTEST;
        TRACE("\r\n=         SFU_IMG_CheckImageState Setting Image State to %d", eTmpImageState);
        e_ret_status_se = SE_SFU_IMG_SetActiveFwState(&eSE_Status, &eTmpImageState);
        if(e_ret_status_se == SE_SUCCESS)
        {
          TRACE("\r\n=         OK");
          e_ret_status_sfu = SFU_SUCCESS;
        }
        else
        {
          TRACE("\r\n=         FAILED (se return status) 0x%08x, (se status) 0x%08x", e_ret_status_se, eSE_Status);
          e_ret_status_sfu = SFU_ERROR;
        }
        break;
      }
      case FWIMG_STATE_SELFTEST:
      {
        /* Bootloader should never find an image in SELFTEST state - bootloader
         * assumes self test was not able to complete and marks image as INVALID
         */
        SE_FwStateTypeDef eTmpImageState = FWIMG_STATE_INVALID;
        TRACE("\r\n=         SFU_IMG_CheckImageState Setting Image State to %d", eTmpImageState);
        e_ret_status_se = SE_SFU_IMG_SetActiveFwState(&eSE_Status, &eTmpImageState);
        if(e_ret_status_se == SE_SUCCESS)
        {
          TRACE("\r\n=         OK");
        }
        else
        {
          TRACE("\r\n=         FAILED (se return status) 0x%08x, (se status) 0x%08x", e_ret_status_se, eSE_Status);
        }
        e_ret_status_sfu = SFU_ERROR;
        SFU_IMG_RollbackFWUpdate();
        break;
      }
      case FWIMG_STATE_VALID:
      {
        e_ret_status_sfu = SFU_SUCCESS;
        break;
      }
      default:
      {
        /* image may be in INVALID state or UNKNOWN state
         * Image marked as invalid failed selftest so we need to attempt a rollback to last good image
         * Image in UNKNOWN state shouldn't be allowed to boot so attempt a rollback to last good image
         */
        e_ret_status_sfu = SFU_ERROR;
        SFU_IMG_RollbackFWUpdate();
        break;
      }
    }
  }
  else
  {
    e_ret_status_sfu = SFU_ERROR;
  }
  return e_ret_status_sfu;
}

/**
  * @brief Searches Slot 1 for an SBSFU header which is copied into the slot during a firmware update
  * @param uint8_t** pHdr Pointer to pointer to an sbsfu header, will point to the header if one is found
  * @retval SFU_ErrorStatus SFU_SUCCESS if a header is found in slot 1, error code otherwise
  */
static SFU_ErrorStatus SFU_IMG_FindBackupStart(uint8_t** pHdr)
{
  int32_t result = 1;
  uint8_t Buffer[4] = {0};
  /* This is not clean coding, the last 2 * "swap region size" bytes of the slot
     are reserved for trailer stuff */
  *pHdr = (uint8_t*) ((REGION_SLOT_1_END + 1) - 2 * SFU_IMG_SWAP_REGION_SIZE);
  SFU_ErrorStatus sfu_result = SFU_SUCCESS;

  while((result!=0) && ((uint32_t)*pHdr>=REGION_SLOT_1_START) && (sfu_result==SFU_SUCCESS))
  {
    TRACE("\n\rCheck for header at 0x%08x", *pHdr);
    sfu_result =  SFU_LL_FLASH_Read(Buffer, *pHdr, sizeof(Buffer));
    if(sfu_result==SFU_SUCCESS)
    {
      /* read from flash OK */
      result = memcmp(Buffer, SBMAGIC, sizeof(SBMAGIC)-1);
      *pHdr -= SFU_IMG_SWAP_REGION_SIZE;
    }
  }
  if((result==0) && (sfu_result == SFU_SUCCESS))
  {
    *pHdr += SFU_IMG_SWAP_REGION_SIZE;
    TRACE("\n\rHeader found at 0x%08x", *pHdr);
    return SFU_SUCCESS;
  }
  else
  {
    TRACE("\n\rHeader not found");
    *pHdr = NULL;
    return SFU_ERROR;
  }
}

/**
  * @brief Executes a rollback to the original firmware if a firmware update fails
  * @param none
  * @retval SFU_ErrorStatus SFU_SUCCESS if the rollback is successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_RollbackFWUpdate(void)
{
  const int32_t PAGE_SIZE = ( 4*1024 );
  SFU_ErrorStatus e_sfu_result = SFU_SUCCESS;
  SE_FwRawHeaderTypeDef* pHdr = NULL;
  SE_FwRawHeaderTypeDef* pTempHdr = NULL;
  uint8_t Buffer[4*1024];
  uint8_t ZeroBuffer[32] = {0x00};
  uint8_t isHeader = 0;

  gRollbackDone = 0;

  /* Verify slot 1 header is from fw that triggered the update */
  /* Backed-up header is in flash Slot1 at pHdr address */
  /* Load current header (start of Slot 0, normally behind the firewall) */
  e_sfu_result =  SFU_LL_FLASH_Read((void *)Buffer, (const void *) SFU_IMG_SLOT_0_REGION_BEGIN, sizeof(SE_FwRawHeaderTypeDef));
  if(SFU_SUCCESS != e_sfu_result)
  {
    return SFU_ERROR;
  }

  /* if fingerprint is 0x00 we cannot do rollback */
  if(0==memcmp(((SE_FwRawHeaderTypeDef*) Buffer)->UpdateSourceFingerprint, ZeroBuffer, sizeof(ZeroBuffer)))
  {
    return SFU_ERROR;
  }

  /* Find Old Image Header */
  e_sfu_result = SFU_IMG_FindBackupStart((uint8_t**) &pHdr);
  if(SFU_SUCCESS != e_sfu_result)
  {
    return SFU_ERROR;
  }
  else
  {
    /* Keep a record of the header location */
    pTempHdr = pHdr;
  }

  /* Current header should contain fingerprint of backed-up header */
  e_sfu_result = SFU_IMG_CheckUpdateSourceFingerprint((SE_FwRawHeaderTypeDef*) Buffer, (SE_FwRawHeaderTypeDef*) pHdr);
  memset(Buffer, 0x00, sizeof(Buffer));
  if(SFU_SUCCESS != e_sfu_result)
  {
    TRACE("\n\rRollback Failed, backup fingerprint does not match original");
    return SFU_ERROR;
  }
  else
  {
    TRACE("\n\rStarting Rollback, Fingerprint OK");
  }

  uint32_t PageOffset = ((sizeof(SE_FwRawHeaderTypeDef) + pHdr->FwSize) / PAGE_SIZE) * PAGE_SIZE;

  /* Total size is size of header struct + VALID tags (3*32) bytes + size of the firmware image
   */
  uint32_t BytesRemaining = sizeof(SE_FwRawHeaderTypeDef) + 3*32 + pHdr->FwSize;
  TRACE("\n\rRollback ");

  while(BytesRemaining>0)
  {
    uint32_t BytesToCopy = BytesRemaining%PAGE_SIZE;
    if(BytesToCopy==0)
    {
      /* copy whole page */
      BytesToCopy = PAGE_SIZE;
    }
    else
    {
      /* copy partial page */
      memset(Buffer, 0x00, sizeof(Buffer));
    }
#if defined(SFU_VERBOSE_DEBUG_MODE)
    TRACE("\n\rReading %d bytes from 0x%08x", BytesToCopy, (((uint8_t*) pHdr)+PageOffset));
#endif /* SFU_VERBOSE_DEBUG_MODE */
    e_sfu_result = SFU_LL_FLASH_Read((void*) Buffer, (void*)(((uint8_t*) pHdr)+PageOffset), BytesToCopy);
    if(e_sfu_result == SFU_SUCCESS)
    {
      SFU_FLASH_StatusTypeDef xFlashStatus;
#if defined(SFU_VERBOSE_DEBUG_MODE)
      TRACE("\n\rErasing %d bytes at 0x%08x", PAGE_SIZE, (REGION_SLOT_0_START+PageOffset));
#endif /* SFU_VERBOSE_DEBUG_MODE */
      e_sfu_result = SFU_LL_FLASH_Erase_Size(&xFlashStatus, (void*)(REGION_SLOT_0_START+PageOffset), PAGE_SIZE);
      /* Ensure there is no UpdateSourceFingerprint in original firmware header
       * This thwarts injection of a new UpdateSourceFingerprint while the header is in slot 1
       * and stops someone attempting multiple rollbacks by writing fake fingerprint in original header
       */
      if((void*)(((uint8_t*) pHdr)+PageOffset) == pTempHdr)
      {
        pTempHdr = (SE_FwRawHeaderTypeDef*) Buffer;
        memset(pTempHdr->UpdateSourceFingerprint, 0x00, sizeof(pTempHdr->UpdateSourceFingerprint));
        isHeader = 1;
      }
    }
    if(e_sfu_result == SFU_SUCCESS)
    {
      SFU_FLASH_StatusTypeDef xFlashStatus;
#if defined(SFU_VERBOSE_DEBUG_MODE)
      TRACE("\n\rWriting %d bytes at 0x%08x", PAGE_SIZE, (REGION_SLOT_0_START+PageOffset));
#endif /* SFU_VERBOSE_DEBUG_MODE */
      if (1 == isHeader)
      {
    	  /* Split the header in 2 write operations otherwise we write beyond the FWALL protection with a non-secure operation */
    	  e_sfu_result = SFU_LL_FLASH_Write(&xFlashStatus, (void*)(REGION_SLOT_0_START+PageOffset), (void*)Buffer, PAGE_SIZE/2);
        if (SFU_SUCCESS == e_sfu_result)
        {
          e_sfu_result = SFU_LL_FLASH_Write(&xFlashStatus, (void*)(REGION_SLOT_0_START+PageOffset+PAGE_SIZE/2), (void*)(Buffer+(PAGE_SIZE/2)), PAGE_SIZE/2);
        }
      }
      else
      {
    	  e_sfu_result = SFU_LL_FLASH_Write(&xFlashStatus, (void*)(REGION_SLOT_0_START+PageOffset), (void*)Buffer, BytesToCopy);
      }
    }
    if(e_sfu_result == SFU_SUCCESS)
    {
      BytesRemaining-=BytesToCopy;
      PageOffset-=PAGE_SIZE;
    }
    else
    {
      return SFU_ERROR;
    }
	TRACE(".");
  }
  TRACE("\n\rDone");
  /* Rollback is complete, now erase downloaded image to prevent an attempt to
   * reinstall
   */
  if(e_sfu_result == SFU_SUCCESS)
  {
    gRollbackDone = 1;
    e_sfu_result = SFU_IMG_EraseDownloadedImg();
  }
  return e_sfu_result;
}


/**
  * @brief Gets fingerprint of header of old firmware and stores it in the header of new firmware
  * @param SE_FwRawHeaderTypeDef* pNewFwHdr Pointer to header of new firmware (updated fw)
  * @param SE_FwRawHeaderTypeDef* pOldFwHdr Pointer to header of old firmware (original fw)
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_SetUpdateSourceFingerprint(SE_FwRawHeaderTypeDef* pNewFwHdr, SE_FwRawHeaderTypeDef* pOldFwHdr)
{
  SFU_ErrorStatus retval = SFU_SUCCESS;
  uint8_t HashBuffer[32];

  /* The fingerprint of the header of the original fw is stored in the header of the new fw
   */
  if((NULL == pNewFwHdr) || (NULL == pOldFwHdr))
  {
    return SFU_ERROR;
  }
  /* check these look like headers */
  if(memcmp(&(pOldFwHdr->SFUMagic), SBMAGIC, strlen(SBMAGIC)) || memcmp(&(pNewFwHdr->SFUMagic), SBMAGIC, strlen(SBMAGIC)))
  {
    return SFU_ERROR;
  }
  /* hash source header */
  retval = SFU_SCHEME_X509_CORE_HashContiguous((uint8_t*) pOldFwHdr, sizeof(SE_FwRawHeaderTypeDef), HashBuffer, sizeof(HashBuffer));

  if(SFU_SUCCESS == retval)
  {
    /* We store the digest of the source header inside the new image header to lock any rollback to
	 * this firmware image which is last known good and initiated the update
     */
    memcpy(&(pNewFwHdr->UpdateSourceFingerprint), HashBuffer, sizeof(HashBuffer));
  }

  return retval;
}

/**
  * @brief Check the fingerprint stored in current firmware header matches the fingerpring of the backed up firmware header
  * @param SE_FwRawHeaderTypeDef* pNewFwHdr Pointer to header of new firmware (updated fw)
  * @param SE_FwRawHeaderTypeDef* pOldFwHdr Pointer to header of backed-up firmware (original fw?)
  * @retval SFU_ErrorStatus SFU_SUCCESS if match is successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_CheckUpdateSourceFingerprint(SE_FwRawHeaderTypeDef* pNewFwHdr, SE_FwRawHeaderTypeDef* pOldFwHdr)
{
  SFU_ErrorStatus retval = SFU_SUCCESS;
  uint8_t HashBuffer[32] = {0};
  /* The fingerprint of the header of the original fw is stored in the header of the new fw
   */
  if((NULL == pNewFwHdr) || (NULL == pOldFwHdr))
  {
    TRACE("\n\rSFU_IMG_CheckUpdateSourceFingerprint: Failed, NULL pointer pNewFwHdr=0x%08x, pOldFwHdr=0x%08x", pNewFwHdr, pOldFwHdr);
    return SFU_ERROR;
  }
  /* check that these look like headers */
  if(memcmp(&(pOldFwHdr->SFUMagic), SBMAGIC, strlen(SBMAGIC)) || memcmp(&(pNewFwHdr->SFUMagic), SBMAGIC, strlen(SBMAGIC)))
  {
    TRACE("\n\rSFU_IMG_CheckUpdateSourceFingerprint: Failed, Not a Header: pNewFwHdr=0x%08x, pOldFwHdr=0x%08x", pNewFwHdr->SFUMagic, pOldFwHdr->SFUMagic);
    return SFU_ERROR;
  }
  /* Check if the fingerprint is 0x00 */
  /* A 0x00 fingerprint is not allowed for rollback */
  if(0 == memcmp(&(pNewFwHdr->UpdateSourceFingerprint), HashBuffer, sizeof(HashBuffer)))
  {
    TRACE("\n\rSFU_IMG_CheckUpdateSourceFingerprint: Failed, No UpdateSourceFingerprint");
    return SFU_ERROR;
  }

  /* hash source header */
  retval = SFU_SCHEME_X509_CORE_HashContiguous((uint8_t*) pOldFwHdr, sizeof(SE_FwRawHeaderTypeDef), HashBuffer, sizeof(HashBuffer));

  if(SFU_SUCCESS == retval)
  {
    /* check the fingerprint held in the new header matches the old header fingerprint */
    // if(memcmp(&(pNewFwHdr->UpdateSourceFingerprint), &(pOldFwHdr->HeaderMAC), sizeof(pNewFwHdr->UpdateSourceFingerprint)))
    if(0 != memcmp(&(pNewFwHdr->UpdateSourceFingerprint), HashBuffer, sizeof(HashBuffer)))
    {
      TRACE("\n\rSFU_IMG_CheckUpdateSourceFingerprint: Failed, UpdateSourceFingerprint does not match header hash");
      retval = SFU_ERROR;
    }
  }
  else
  {
    TRACE("\n\rSFU_IMG_CheckUpdateSourceFingerprint: Failed, SFU_SCHEME_X509_CORE_HashContiguous returned error: 0x%x", retval);
  }
  return retval;
}

#endif /* ENABLE_IMAGE_STATE_HANDLING */
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

/**
  * @}
  */

#undef SFU_FWIMG_SERVICES_C

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
