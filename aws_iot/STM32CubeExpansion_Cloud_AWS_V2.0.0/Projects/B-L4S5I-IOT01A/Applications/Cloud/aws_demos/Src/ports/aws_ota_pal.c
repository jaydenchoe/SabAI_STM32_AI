/*
 * Amazon FreeRTOS OTA PAL for STM32L4 V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*
  Platform abstraction layer for Amazon FreeRTOS OTA agent.
  This file contains the OTA support functions specific to the STM32L4xx target platform.

  TARGET_PLATFORM_STM32L4XX
*/
#include <stdio.h>
#include "flash.h"
#include "iot_crypto.h"
#include "aws_iot_ota_pal.h"
#include "aws_iot_ota_agent_internal.h"
#include "string.h"

#include "stm32l4xx_hal.h"      /* Flash programming of the FW*/
#include "se_def.h"
#include "sfu_fwimg_regions.h"
#ifdef ENABLE_IMAGE_STATE_HANDLING
#include "se_interface_application.h"
#endif /* ENABLE_IMAGE_STATE_HANDLING */

/**
  * @brief Specifies a structure containing values related to the management of multi-images in Flash.
  */
typedef struct
{
  uint32_t  MaxSizeInBytes;        /*!< The maximum allowed size for the FwImage in User Flash (in Bytes) */
  uint32_t  DownloadAddr;          /*!< The download address for the FwImage in UserFlash */
  uint32_t  ImageOffsetInBytes;    /*!< Image write starts at this offset */
  uint32_t  ExecutionAddr;         /*!< The execution address for the FwImage in UserFlash */
} SFU_FwImageFlashTypeDef;

#define USER_CONF_MAGIC            0x0123456789ABCDEFuLL
typedef struct {
  uint64_t magic;                  /*!< The USER_CONF_MAGIC magic word signals that the structure was once written to FLASH. */
  uint32_t ota_agent_state;
} iot_state_t;

#ifdef ENABLE_IMAGE_STATE_HANDLING
OTA_Err_t prvInitOTAImageState(void);
OTA_Err_t prvSetOTAImageState( OTA_ImageState_t eNewOTAState );
#else
static int setIoTDeviceState(iot_state_t *state);
static int getIoTDeviceState(const iot_state_t **state);
#endif /* ENABLE_IMAGE_STATE_HANDLING */

static uint32_t SFU_APP_InstallAtNextReset(uint32_t *fw_header);
static uint32_t SFU_APP_GetDownloadAreaInfo(SFU_FwImageFlashTypeDef *pArea);


/* Specify the OTA signature algorithm we support on this platform. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";


/* The static functions below (prvPAL_CheckFileSignature and prvPAL_ReadAndAssumeCertificate)
 * are optionally implemented. If these functions are implemented then please set the following macros in
 * aws_test_ota_config.h to 1:
 * otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
 * otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
 */

/**
 * @brief Verify the signature of the specified file.
 *
 * This function should be implemented if signature verification is not offloaded
 * to non-volatile memory io functions.
 *
 * This function is called from prvPAL_Close().
 *
 * @param[in] C OTA file context information.
 *
 * @return Below are the valid return values for this function.
 * kOTA_Err_None if the signature verification passes.
 * kOTA_Err_SignatureCheckFailed if the signature verification fails.
 * kOTA_Err_BadSignerCert if the if the signature verification certificate cannot be read.
 *
 */
static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C );

/**
 * @brief Read the specified signer certificate from the filesystem into a local buffer.
 *
 * The allocated memory returned becomes the property of the caller who is responsible for freeing it.
 *
 * This function is called from prvPAL_CheckFileSignature(). It should be implemented if signature
 * verification is not offloaded to non-volatile memory io function.
 *
 * @param[in] pucCertName The file path of the certificate file.
 * @param[out] ulSignerCertSize The size of the certificate file read.
 *
 * @return A pointer to the signer certificate in the file system. NULL if the certificate cannot be read.
 * This returned pointer is the responsibility of the caller; if the memory is allocated the caller must free it.
 */
static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );

/* Location of the update slot in Flash. */
SFU_FwImageFlashTypeDef fw_image_dwl_area;

/* State of the downloaded OTA image
 * The state of the currently running image is stored in flash with the firmware image and is
 * accessed via SE_APP_GetActiveFwState() and SE_APP_SetActiveFwState(). The state of the OTA image
 * being downloaded is stored in RAM in OTAImageState. Depending on the state of the currently running
 * image we get/set one of the two image states.
 */
iot_state_t OTAImageState;

static const char pcClientCertificatePem[] =
		"-----BEGIN CERTIFICATE-----\n"
		"MIICNjCCAbygAwIBAgIUEFCiZSz0U0NfD/kvxi8plpqaIzgwCgYIKoZIzj0EAwIw\n"
		"UjELMAkGA1UEBhMCWFgxFTATBgNVBAcMDERlZmF1bHQgQ2l0eTEcMBoGA1UECgwT\n"
		"RGVmYXVsdCBDb21wYW55IEx0ZDEOMAwGA1UEAwwFU1RNMzIwHhcNMTkwODAxMTM0\n"
		"MzAzWhcNMjIwNDI3MTM0MzAzWjBSMQswCQYDVQQGEwJYWDEVMBMGA1UEBwwMRGVm\n"
		"YXVsdCBDaXR5MRwwGgYDVQQKDBNEZWZhdWx0IENvbXBhbnkgTHRkMQ4wDAYDVQQD\n"
		"DAVTVE0zMjB2MBAGByqGSM49AgEGBSuBBAAiA2IABJ2bLnKMrhIv8dYwT5C37580\n"
		"xdvsqmVVZKTlRZUatELS2Njq/X+ekalP4W87BDRoQAxHIeHbJwpjTZO0yHz0FasT\n"
		"nOV4gQigVB4PG8DZU9SfoJ1m4Ri8Y+rdtbnGGVAd2KNTMFEwHQYDVR0OBBYEFNpZ\n"
		"KYz6S/zzKlyu3KWN07uInExyMB8GA1UdIwQYMBaAFNpZKYz6S/zzKlyu3KWN07uI\n"
		"nExyMA8GA1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwIDaAAwZQIxAMCm5HMx1REz\n"
		"oxYV0/jDB2VGrBaQ5dSj4rHeA+wpGgRZI8TSqDB+NWXimVXrJG5gQgIwZ9Ek8+YV\n"
		"kpFNZ102HE1pOqbI4UcVAiQejK5n2TSuxGbxrl29FYjrSGKrWa2Jt2Ws\n"
		"-----END CERTIFICATE-----\n";

#ifndef ENABLE_IMAGE_STATE_HANDLING
iot_state_t liot_state __attribute__( ( section( "UNINIT_FIXED_LOC" ) ) );

static int setIoTDeviceState(iot_state_t *state)
{
	DEFINE_OTA_METHOD_NAME( "setIoTState" );
	int ret = 0;
	state->magic = USER_CONF_MAGIC;

	if (FLASH_update((uint32_t)&liot_state, state, sizeof(iot_state_t)) != 0)
	{
		OTA_LOG_L1( "[%s] ERROR - Failed programming the IOT state into Flash.\r\n", OTA_METHOD_NAME );
		ret = -1;
	}

	return ret;
}

static int getIoTDeviceState(const iot_state_t **state)
{
	int ret = -1;

	if (state != NULL)
	{
		if (liot_state.magic == USER_CONF_MAGIC)
		{
			*state = &liot_state;
			ret = 0;
		}
		else
		{
			*state = NULL;
		}
	}

	return ret;
}
#endif


static inline BaseType_t prvContextValidate( OTA_FileContext_t * C )
{
    return( ( C != NULL ) &&
            ( C->pucFile != NULL ) ); /*lint !e9034 Comparison is correct for file pointer type. */
}

/* Used to set the high bit of Windows error codes for a negative return value. */
#define OTA_PAL_INT16_NEGATIVE_MASK    ( 1 << 15 )

/* Size of buffer used in file operations on this platform (Windows). */
#define OTA_PAL_WIN_BUF_SIZE ( ( size_t ) 4096UL )

/* Attempt to create a new receive file for the file chunks as they come in. */

OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CreateFileForRx" );

    OTA_Err_t eResult = kOTA_Err_RxFileCreateFailed;

    if( C != NULL )
    {
#ifdef ENABLE_IMAGE_STATE_HANDLING
        /* Initialize OTA image state */
        if(kOTA_Err_None != prvInitOTAImageState())
        {
            OTA_LOG_L1( "[%s] ERROR - Unable to initialize OTA image state.\r\n", OTA_METHOD_NAME );
            return kOTA_Err_Panic;
        }
#else
        iot_state_t state = {
            .magic = 0,
            .ota_agent_state = eOTA_ImageState_Unknown
        };
#endif /* ENABLE_IMAGE_STATE_HANDLING */
        SFU_APP_GetDownloadAreaInfo(&fw_image_dwl_area);

#ifndef ENABLE_IMAGE_STATE_HANDLING
        if ( 0 != setIoTDeviceState(&state) )
        {
            OTA_LOG_L1( "[%s] ERROR - Unable to write to image state file.\r\n", OTA_METHOD_NAME );
        }
        else
#endif /* ENABLE_IMAGE_STATE_HANDLING */
        {
            C->pucFile = (void*) &fw_image_dwl_area; // Aliased to C->iFileHandle by union
            FLASH_Erase_Size(fw_image_dwl_area.DownloadAddr, fw_image_dwl_area.MaxSizeInBytes);
            OTA_LOG_L1( "[%s] Receive file created at 0x%08x.\r\n", OTA_METHOD_NAME, C->pucFile );
            eResult = kOTA_Err_None;
        }
    }
    else
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
    }

    return eResult; /*lint !e480 !e481 Exiting function without calling fclose.
                     * Context file handle state is managed by this API. */
}


/* Abort receiving the specified OTA update by closing the file. */

OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_Abort" );

    /* Set default return status to uninitialized. */
    OTA_Err_t eResult = kOTA_Err_Uninitialized;
    int32_t lFileCloseResult = 0;

    if( NULL != C )
    {
        /* Close the OTA update file if it's open. */
        if( NULL != C->pucFile )
        {
            C->pucFile = NULL;

            if( 0 == lFileCloseResult )
            {
                OTA_LOG_L1( "[%s] OK\r\n", OTA_METHOD_NAME );
                eResult = kOTA_Err_None;
            }
            else /* Failed to close file. */
            {
                OTA_LOG_L1( "[%s] ERROR - Closing file failed.\r\n", OTA_METHOD_NAME );
                eResult = kOTA_Err_FileAbort;
            }
        }
        else
        {
            /* Nothing to do. No open file associated with this context. */
            eResult = kOTA_Err_None;
        }
    }
    else /* Context was not valid. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_FileAbort;
    }

    return eResult;
}

/* Write a block of data to the specified file. */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_WriteBlock" );

    int16_t rc = 0;

    if( prvContextValidate( C ) == pdTRUE )
    {
        SFU_FwImageFlashTypeDef * fwImage = ( SFU_FwImageFlashTypeDef * ) C->pucFile;
        vLoggingPrintf("W: Received offset=%08x size=%d\n", ulOffset, ulBlockSize);

        /* Guard against slot overflow */
        if( ulOffset + ulBlockSize > fwImage->MaxSizeInBytes)
        {
            OTA_LOG_L1( "[%s] ERROR - Flash slot overflow.\r\n", OTA_METHOD_NAME );
            rc |= OTA_PAL_INT16_NEGATIVE_MASK;
        }
        else
        {
            int ret;
            uint8_t *pucBuf = ( uint8_t * ) fwImage->DownloadAddr;

            if( ( (( uint32_t )( pucBuf + ulOffset ) % 8) != 0 ) || ( ( ulBlockSize % 8 ) != 0) )
            {
                /* STM32L4 constraint: The destination memory is not aligned on double-word boundaries. */
                ret = FLASH_update(( uint32_t ) pucBuf + ulOffset, ( uint32_t * ) pacData, ulBlockSize);
            }
            else
            {
                ret = FLASH_Write( ( uint32_t ) pucBuf + ulOffset, ( uint32_t * ) pacData, ulBlockSize);
            }

            if( ret == 0 )
            {
                rc = ulBlockSize;
            }
            else
            {
                OTA_LOG_L1( "[%s] ERROR - fwrite failed\r\n", OTA_METHOD_NAME );
                rc |= OTA_PAL_INT16_NEGATIVE_MASK;
            }
        }
    }
    else /* Invalid context or file pointer provided. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        rc |= OTA_PAL_INT16_NEGATIVE_MASK;
    }

    return ( int16_t ) rc;
}

/* Close the specified file. This shall authenticate the file if it is marked as secure. */

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

    OTA_Err_t eResult = kOTA_Err_None;

    if( prvContextValidate( C ) == pdTRUE )
    {
        if( C->pxSignature != NULL )
        {
            /* Verify the file signature, close the file and return the signature verification result. */
            eResult = prvPAL_CheckFileSignature( C );
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR - NULL OTA Signature structure.\r\n", OTA_METHOD_NAME );
            eResult = kOTA_Err_SignatureCheckFailed;
        }

        /* Close the file. */
        C->pucFile = NULL;

        if( eResult == kOTA_Err_None )
        {
            OTA_LOG_L1( "[%s] %s signature verification passed.\r\n", OTA_METHOD_NAME, cOTA_JSON_FileSignatureKey );
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR - Failed to pass %s signature verification: %d.\r\n", OTA_METHOD_NAME,
                        cOTA_JSON_FileSignatureKey, eResult );

            /* If we fail to verify the file signature that means the image is not valid. We need to set the image state to aborted. */
            prvPAL_SetPlatformImageState( eOTA_ImageState_Aborted );
        }
    }
    else /* Invalid OTA Context. */
    {
        /* FIXME: Invalid error code for a null file context and file handle. */
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_FileClose;
    }

    return eResult;
}


/* Verify the signature of the specified file. */

static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CheckFileSignature" );

    OTA_Err_t eResult = kOTA_Err_None;
    uint32_t ulBytesRead;
    uint32_t ulSignerCertSize;
    uint8_t * pucBuf, * pucSignerCert;
    void * pvSigVerifyContext;

    if( prvContextValidate( C ) == pdTRUE )
    {
        /* Verify the signature. */
        if( pdFALSE == CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA, cryptoHASH_ALGORITHM_SHA256 ) )
        {
            eResult = kOTA_Err_SignatureCheckFailed;
        }
        else
        {
            OTA_LOG_L1( "[%s] Started %s signature verification, file: %s\r\n", OTA_METHOD_NAME,
                        cOTA_JSON_FileSignatureKey, ( const char * ) C->pucCertFilepath );
            pucSignerCert = prvPAL_ReadAndAssumeCertificate( ( const uint8_t * const ) C->pucCertFilepath, &ulSignerCertSize );

            if( pucSignerCert != NULL )
            {
                SFU_FwImageFlashTypeDef * fwImage = ( SFU_FwImageFlashTypeDef * ) C->pucFile;
                pucBuf = ( uint8_t * ) fwImage->DownloadAddr;
                ulBytesRead = C->ulFileSize;
                /* Include the file chunk in the signature validation. Zero size is OK. */
                CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, pucBuf, ulBytesRead );

                if( pdFALSE == CRYPTO_SignatureVerificationFinal( pvSigVerifyContext,
                    ( char * ) pucSignerCert,
                    ( size_t ) ulSignerCertSize,
                    C->pxSignature->ucData,
                    C->pxSignature->usSize ) ) /*lint !e732 !e9034 Allow comparison in this context. */
                {
                  eResult = kOTA_Err_SignatureCheckFailed;
                }
                pvSigVerifyContext = NULL;  /* The context has been freed by CRYPTO_SignatureVerificationFinal(). */
            }
            else
            {
                eResult = kOTA_Err_BadSignerCert;
            }
        }
    }
    else
    {
        /* FIXME: Invalid error code for a NULL file context. */
        OTA_LOG_L1( "[%s] ERROR - Invalid OTA file context.\r\n", OTA_METHOD_NAME );
        /* Invalid OTA context or file pointer. */
        eResult = kOTA_Err_NullFilePtr;
    }

    return eResult;
}


/* Read the specified signer certificate from the filesystem into a local buffer. The allocated
 * memory becomes the property of the caller who is responsible for freeing it.
 */

static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    //DEFINE_OTA_METHOD_NAME( "prvPAL_ReadAndAssumeCertificate" );

    /* The crypto code requires the terminating zero to be part of the length so add 1. */
    *ulSignerCertSize = strlen(pcClientCertificatePem) + 1;

    return (uint8_t*) pcClientCertificatePem;
}


OTA_Err_t prvPAL_ResetDevice( void )
{
    NVIC_SystemReset();
    /* return kOTA_Err_None; with IAR, returning after NVIC_SystemReset() produces a cc warning*/
}


OTA_Err_t prvPAL_ActivateNewImage( void )
{
    uint32_t  fw_header_input[(SE_FW_HEADER_TOT_LEN + 3) / 4];
    /* Read header in slot 1 */
    memcpy((void *) fw_header_input, (void *) fw_image_dwl_area.DownloadAddr, sizeof(fw_header_input));

  /* Ask for installation at next reset */
  (void) SFU_APP_InstallAtNextReset(fw_header_input);

  return prvPAL_ResetDevice();
}


/*
 * Set the final state of the last transferred (final) OTA file (or bundle).
 * On Windows, the state of the OTA image is stored in PlaformImageState.txt.
 */

#ifndef ENABLE_IMAGE_STATE_HANDLING
OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );

    OTA_Err_t eResult = kOTA_Err_None;

    if( eState != eOTA_ImageState_Unknown && eState <= eOTA_LastImageState )
    {
        iot_state_t newState;
        const iot_state_t *pCurState = NULL;
        getIoTDeviceState(&pCurState);

        if( eState == eOTA_ImageState_Accepted )
        {
            if( pCurState->ota_agent_state != eOTA_ImageState_Testing )
            {
                eResult = kOTA_Err_CommitFailed;
            }
            else
            {
                newState.ota_agent_state = eState;
            }
        }
        else if( eState == eOTA_ImageState_Testing )
        {
            if( pCurState->ota_agent_state != eOTA_ImageState_Unknown )
            {
                eResult = kOTA_Err_CommitFailed;
            }
            else
            {
                newState.ota_agent_state = eState;
            }
        }
        else if( eState == eOTA_ImageState_Rejected )
        {
            newState.ota_agent_state = eState; // else kOTA_Err_RejectFailed
        }
        else if( eState == eOTA_ImageState_Aborted )
        {
            newState.ota_agent_state = eState;  // else kOTA_Err_AbortFailed
        }
        else
        {
            eResult = kOTA_Err_BadImageState;
        }

        if ( ( eResult == kOTA_Err_None ) && ( setIoTDeviceState(&newState) != 0) )
        {
            OTA_LOG_L1( "[%s] ERROR - Unable to write to image state file.\r\n", OTA_METHOD_NAME );
            eResult = kOTA_Err_Panic;
        }
    }
    else /* Image state invalid. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid image state provided.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_BadImageState;
    }

    return eResult;
}

#else

OTA_Err_t prvInitOTAImageState(void)
{
      OTAImageState.magic = 0;
      OTAImageState.ota_agent_state = eOTA_ImageState_Unknown;
      return kOTA_Err_None;
}

OTA_Err_t prvSetOTAImageState( OTA_ImageState_t eNewOTAState )
{
  OTA_Err_t result = kOTA_Err_BadImageState;
  /* Can't set image state to unknown or out of range */
  if((eNewOTAState<=0)||(eNewOTAState>eOTA_LastImageState))
  {
    result = kOTA_Err_BadImageState;
  }
  else
  {
    switch(eNewOTAState)
    {
      case eOTA_ImageState_Accepted:
        /* can only set Accepted if the image is running (in self test mode) */
        result = kOTA_Err_CommitFailed;
        break;
      default:
        OTAImageState.ota_agent_state = eNewOTAState;
        result = kOTA_Err_None;
    }
  }
  return result;
}



OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );
    SE_FwStateTypeDef eCurrentImageState = FWIMG_STATE_UNKNOWN;
    OTA_Err_t eResult = kOTA_Err_Uninitialized;

    SE_ErrorStatus e_se_estatus = SE_ERROR;
    SE_StatusTypeDef eSE_Status = SE_OK;

    /* Read the state of the currently running image */
    e_se_estatus = SE_APP_GetActiveFwState(&eSE_Status, &eCurrentImageState);
    if(SE_SUCCESS != e_se_estatus)
    {
      /* Error getting image state */
      return kOTA_Err_BadImageState;
    }

    OTA_LOG_L1( "[%s] Current state (SE_FwStateTypeDef) %d\r\n", OTA_METHOD_NAME, eCurrentImageState );
    OTA_LOG_L1( "[%s] Request to change state to (OTA_ImageState_t) %d\r\n", OTA_METHOD_NAME, eState );

    /* if the current image is in SELFTEST mode we can update its state, otherwise
     * we act on the state of the OTA image being received
     */
    if( FWIMG_STATE_SELFTEST == eCurrentImageState)
    {
      /* This is an image launched in self test mode! - we can update either to valid or invalid */
      switch(eState)
      {
        /* the image was accepted after self tests succeeded */
        case eOTA_ImageState_Accepted:
        {
          SE_FwStateTypeDef eNewImageState = FWIMG_STATE_VALID;
          e_se_estatus = SE_APP_SetActiveFwState(&eSE_Status, &eNewImageState);
          if(SE_SUCCESS == e_se_estatus)
          {
            /* Commit was successful */
            eResult = kOTA_Err_None;
            eCurrentImageState = eNewImageState;
            OTA_LOG_L1( "[%s] New current state (SE_FwStateTypeDef) %d\r\n", OTA_METHOD_NAME, eCurrentImageState );
            /* Erase backup image as it is no longer needed */
            SFU_APP_GetDownloadAreaInfo(&fw_image_dwl_area);
            FLASH_Erase_Size(fw_image_dwl_area.DownloadAddr, fw_image_dwl_area.MaxSizeInBytes);
          }
          else
          {
            /* Image is valid but commit failed */
            eResult = kOTA_Err_CommitFailed;
          }
          break;
        }
        /* the image is already in testing state */
        case eOTA_ImageState_Testing:
        {
          eResult = kOTA_Err_None;
          break;
        }
        /* Image was rejected - set as INVALID */
        case eOTA_ImageState_Rejected:
        /* all other image states will set to invalid */
        default:
        {
          SE_FwStateTypeDef eNewImageState = FWIMG_STATE_INVALID;
          e_se_estatus = SE_APP_SetActiveFwState(&eSE_Status, &eNewImageState);
          if(SE_SUCCESS == e_se_estatus)
          {
            eResult = kOTA_Err_None;
            eCurrentImageState = eNewImageState;
            OTA_LOG_L1( "[%s] New current state (SE_FwStateTypeDef) %d\r\n", OTA_METHOD_NAME, eCurrentImageState );
          }
          else
          {
            eResult = kOTA_Err_RejectFailed;
          }
          break;
        }
      }
    }
    else
    {
      /* Image is not in self-test mode - we update the OTA image state */
      eResult = prvSetOTAImageState( eState );
    }
    return eResult;
}

#endif /* ENABLE_IMAGE_STATE_HANDLING */

#ifndef ENABLE_IMAGE_STATE_HANDLING
/* Get the state of the currently running image.
 *
 * We read this at OTA_Init time so we can tell if the MCU image is in self
 * test mode. If it is, we expect a successful connection to the OTA services
 * within a reasonable amount of time. If we don't satisfy that requirement,
 * we assume there is something wrong with the firmware and reset the device,
 * causing it to rollback to the previous code.
 */
OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    //DEFINE_OTA_METHOD_NAME( "prvPAL_GetPlatformImageState" );

    OTA_PAL_ImageState_t ePalState = eOTA_PAL_ImageState_Unknown;
    iot_state_t const * pState = NULL;

    if (getIoTDeviceState(&pState) == 0)
    {
        switch (pState->ota_agent_state)
			  {
				    case eOTA_ImageState_Testing:
					      ePalState = eOTA_PAL_ImageState_PendingCommit;
					      break;
				    case eOTA_ImageState_Accepted:
					      ePalState = eOTA_PAL_ImageState_Valid;
					      break;
				    case eOTA_ImageState_Rejected:
				    case eOTA_ImageState_Aborted:
				    default:
					      ePalState = eOTA_PAL_ImageState_Invalid;
					      break;
			  }
		}
    else
    {
        /* If no image state exists, assume a factory image. */
		    ePalState = eOTA_PAL_ImageState_Valid;
    }

    return ePalState; /*lint !e64 !e480 !e481 I/O calls and return type are used per design. */
}
#else

/* Read the state of the OTA image */
OTA_ImageState_t prvGetOTAImageState( void )
{
    return (OTA_ImageState_t) OTAImageState.ota_agent_state;
}

/* Get the state of the currently running image.
 *
 * We read this at OTA_Init time so we can tell if the MCU image is in self
 * test mode. If it is, we expect a successful connection to the OTA services
 * within a reasonable amount of time. If we don't satisfy that requirement,
 * we assume there is something wrong with the firmware and reset the device,
 * causing it to rollback to the previous code.
 */
OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
  DEFINE_OTA_METHOD_NAME( "prvPAL_GetPlatformImageState" );

  OTA_PAL_ImageState_t ePalState = eOTA_PAL_ImageState_Unknown;
  OTA_ImageState_t eOTAImageState = eOTA_ImageState_Unknown;
  SE_FwStateTypeDef eCurrentImageState = FWIMG_STATE_UNKNOWN;

  SE_ErrorStatus e_se_estatus = SE_ERROR;
  SE_StatusTypeDef eSE_Status = SE_OK;

  /* Get state of running image */
  e_se_estatus = SE_APP_GetActiveFwState(&eSE_Status, &eCurrentImageState);
  /* Get state of OTA image */
  eOTAImageState = prvGetOTAImageState();
  /* If the CurrentImageState is SELFTEST, we return it, otherwise we return the OTA image state */
  if(SE_SUCCESS == e_se_estatus)
  {
    if(eCurrentImageState == FWIMG_STATE_SELFTEST)
    {
      ePalState = eOTA_PAL_ImageState_PendingCommit;
    }
    else
    {
      switch(eOTAImageState)
      {
        case eOTA_ImageState_Unknown:
          ePalState = eOTA_PAL_ImageState_Unknown;
          break;
        case eOTA_ImageState_Testing:
          ePalState = eOTA_PAL_ImageState_PendingCommit;
          break;
        case eOTA_ImageState_Accepted:
          ePalState = eOTA_PAL_ImageState_Valid;
          break;
        case eOTA_ImageState_Rejected:
          ePalState = eOTA_PAL_ImageState_Invalid;
          break;
        case eOTA_ImageState_Aborted:
          ePalState = eOTA_PAL_ImageState_Invalid;
          break;
        default:
          ePalState = eOTA_PAL_ImageState_Unknown;
      }
    }
  }
  OTA_LOG_L3( "[%s] Got state from Current Image (SE_FwStateTypeDef) %d\r\n", OTA_METHOD_NAME, eCurrentImageState );
  OTA_LOG_L3( "[%s] Got state from OTA Image (OTA_PAL_ImageState_t) %d\r\n", OTA_METHOD_NAME, eOTAImageState );
  OTA_LOG_L3( "[%s] Returning state (OTA_PAL_ImageState_t) %d\r\n", OTA_METHOD_NAME, ePalState );
  return ePalState;
}
#endif /* ENABLE_IMAGE_STATE_HANDLING */


static uint32_t SFU_APP_GetDownloadAreaInfo(SFU_FwImageFlashTypeDef *pArea)
{
  uint32_t ret = 0;

  if (pArea != NULL)
  {
    pArea->DownloadAddr = SFU_IMG_SLOT_DWL_REGION_BEGIN_VALUE;
    pArea->MaxSizeInBytes = (uint32_t)SFU_IMG_SLOT_DWL_REGION_SIZE;
    pArea->ImageOffsetInBytes = SFU_IMG_IMAGE_OFFSET;
  }
  else
  {
    ret = -1;
  }

  return ret;
}

static uint32_t SFU_APP_InstallAtNextReset(uint32_t *fw_header)
{
	uint32_t ret = HAL_OK;
	uint8_t zero_buffer[SFU_IMG_IMAGE_OFFSET - SE_FW_HEADER_TOT_LEN];

	if (fw_header == NULL)
	{
		return -1;
	}

	memset(zero_buffer, 0, SFU_IMG_IMAGE_OFFSET - SE_FW_HEADER_TOT_LEN);
    ret = FLASH_Erase_Size((uint32_t) SFU_IMG_SWAP_REGION_BEGIN_VALUE, SFU_IMG_IMAGE_OFFSET);
    if (ret == HAL_OK)
    {
      ret = FLASH_Write((uint32_t)SFU_IMG_SWAP_REGION_BEGIN_VALUE, fw_header, SE_FW_HEADER_TOT_LEN);
    }
    if (ret == HAL_OK)
    {
      ret = FLASH_Write((uint32_t )(SFU_IMG_SWAP_REGION_BEGIN_VALUE + SE_FW_HEADER_TOT_LEN), (void *)zero_buffer, SFU_IMG_IMAGE_OFFSET - SE_FW_HEADER_TOT_LEN);
    }

    return (ret == HAL_OK) ? 0 : -1;
}

