/**
  ******************************************************************************
  * @file    stsafea110.h
  * @author  SMD/AME application teams
  * @version V3.1.0
  * @brief   STSAFE_A110 device specific include file.
  *          Specifies device specific defines, macros, types.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * STSAFE DRIVER SOFTWARE LICENSE AGREEMENT (SLA0088)
  *
  * BY INSTALLING, COPYING, DOWNLOADING, ACCESSING OR OTHERWISE USING THIS SOFTWARE
  * OR ANY PART THEREOF (AND THE RELATED DOCUMENTATION) FROM STMICROELECTRONICS
  * INTERNATIONAL N.V, SWISS BRANCH AND/OR ITS AFFILIATED COMPANIES (STMICROELECTRONICS),
  * THE RECIPIENT, ON BEHALF OF HIMSELF OR HERSELF, OR ON BEHALF OF ANY ENTITY BY WHICH
  * SUCH RECIPIENT IS EMPLOYED AND/OR ENGAGED AGREES TO BE BOUND BY THIS SOFTWARE LICENSE
  * AGREEMENT.
  *
  * Under STMicroelectronics’ intellectual property rights, the redistribution,
  * reproduction and use in source and binary forms of the software or any part thereof,
  * with or without modification, are permitted provided that the following conditions
  * are met:
  * 1.  Redistribution of source code (modified or not) must retain any copyright notice,
  *     this list of conditions and the disclaimer set forth below as items 10 and 11.
  * 2.  Redistributions in binary form, except as embedded into a microcontroller or
  *     microprocessor device or a software update for such device, must reproduce any
  *     copyright notice provided with the binary code, this list of conditions, and the
  *     disclaimer set forth below as items 10 and 11, in documentation and/or other
  *     materials provided with the distribution.
  * 3.  Neither the name of STMicroelectronics nor the names of other contributors to this
  *     software may be used to endorse or promote products derived from this software or
  *     part thereof without specific written permission.
  * 4.  This software or any part thereof, including modifications and/or derivative works
  *     of this software, must be used and execute solely and exclusively in combination
  *     with a secure microcontroller device from STSAFE family manufactured by or for
  *     STMicroelectronics.
  * 5.  No use, reproduction or redistribution of this software partially or totally may be
  *     done in any manner that would subject this software to any Open Source Terms.
  *     “Open Source Terms” shall mean any open source license which requires as part of
  *     distribution of software that the source code of such software is distributed
  *     therewith or otherwise made available, or open source license that substantially
  *     complies with the Open Source definition specified at www.opensource.org and any
  *     other comparable open source license such as for example GNU General Public
  *     License(GPL), Eclipse Public License (EPL), Apache Software License, BSD license
  *     or MIT license.
  * 6.  STMicroelectronics has no obligation to provide any maintenance, support or
  *     updates for the software.
  * 7.  The software is and will remain the exclusive property of STMicroelectronics and
  *     its licensors. The recipient will not take any action that jeopardizes
  *     STMicroelectronics and its licensors' proprietary rights or acquire any rights
  *     in the software, except the limited rights specified hereunder.
  * 8.  The recipient shall comply with all applicable laws and regulations affecting the
  *     use of the software or any part thereof including any applicable export control
  *     law or regulation.
  * 9.  Redistribution and use of this software or any part thereof other than as  permitted
  *     under this license is void and will automatically terminate your rights under this
  *     license.
  * 10. THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" AND ANY
  *     EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  *     WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
  *     OF THIRD PARTY INTELLECTUAL PROPERTY RIGHTS, WHICH ARE DISCLAIMED TO THE FULLEST
  *     EXTENT PERMITTED BY LAW. IN NO EVENT SHALL STMICROELECTRONICS OR CONTRIBUTORS BE
  *     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  *     DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  *     THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  *     NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  *     ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  * 11. EXCEPT AS EXPRESSLY PERMITTED HEREUNDER, NO LICENSE OR OTHER RIGHTS, WHETHER EXPRESS
  *     OR IMPLIED, ARE GRANTED UNDER ANY PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF
  *     STMICROELECTRONICS OR ANY THIRD PARTY.
  ******************************************************************************
  */

#ifndef STSAFEA110_H
#define STSAFEA110_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stsafea_conf.h"
#include "stsafea_types.h"

/** @addtogroup STSAFE_A110_DEVICE_DEF_MACRO_TYPES
  * @{
  */


/** @addtogroup DEVICE_STSAFE_A110
  * @{
  */


/* Exported types ------------------------------------------------------------*/
/** @defgroup STSAFE_A110_Exported_Types STSAFE-A110's exported types
  * @{
  */
/*!
 * \enum StSafeA_RndSubject_t
 * \brief Random subject enumeration
 * \details Specifies the random number usage
 */
typedef enum
{
  STSAFEA_EPHEMERAL_RND             = 0x0U,
} StSafeA_RndSubject_t;

/*!
 * \struct StSafeA_SigfoxKeySlotBuffer_t
 * \brief Sigfox key slot structure type definition
 * \details Contains Sigfox key slot information.
 */
typedef struct
{
  uint16_t Length;
  uint8_t  DeviceIdentifierPresence;
  uint8_t  NetworkAuthenticationKeyPresence;
  uint8_t  PortabilityAuthorizationCodePresence;
  uint8_t  EncryptionKeyPresence;
  uint8_t  RolloverCounter;
  uint16_t SequenceCounter;
  uint8_t  DeviceIdentifier[4];
  uint8_t  PortabilityAuthorizationCode[8];
  uint8_t  EncryptionFlag;
  uint8_t  EncryptionFlagUpdateAccessCondition;
} StSafeA_SigfoxKeySlotBuffer_t;

/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/
/** @defgroup STSAFE_A110_Exported_Defines STSAFE-A110's exported defines
  * @{
  */


/** @defgroup STSAFE_A110_SIGNATURE_SESSION STSAFE-A110's signature session deactivation
  * @brief STSAFE-A110 signature session deactivation
  * @{
  */
#define USE_SIGNATURE_SESSION                   0U

/**
  * @}
  */


/** @defgroup STSAFE_A110_Commands  STSAFE-A110's exported types
  * @brief STSAFE-A110 Commands
  * @{
  */
#define STSAFEA_CMD_ECHO                      (0x00U)      /*!< Echo: Returns as a response the data that it received as command data. */
#define STSAFEA_CMD_RESET                     (0x01U)      /*!< Reset: Interrupts any on-going session. */
#define STSAFEA_CMD_GENERATE_RANDOM           (0x02U)      /*!< Generate Random: Returns the requested number of random bytes.*/
#define STSAFEA_CMD_START_SESSION             (0x03U)      /*!< Start Session: Starts a signature session. It must be used in combination with the Get Signature command. */
#define STSAFEA_CMD_DECREMENT                 (0x04U)      /*!< Decrement: Decrements the one-way counter in a counter zone. When the counter reaches zero, the command is refused.*/
#define STSAFEA_CMD_READ                      (0x05U)      /*!< Read: Used to read data from a data partition zone. This command can also be used to change the read access conditions of the zone to a more stricter value. */
#define STSAFEA_CMD_UPDATE                    (0x06U)      /*!< Update: Used to update data in a zone. This command can also be used to change the update access conditions of the zone to a more stricter value. */
#define STSAFEA_CMD_DELETE_KEY                (0x0CU)      /*!< Delete: Used to delete keys. Only Delete Password feature is supported through the available APIs. */
#define STSAFEA_CMD_HIBERNATE                 (0x0DU)      /*!< Hibernate: Sets the product in very-low-power consumption mode. The device restart is equivalent to a restart after a reset or Power On Reset. */
#define STSAFEA_CMD_WRAP_LOCAL_ENVELOPE       (0x0EU)      /*!< Wrap Local Envelope: This command is used to wrap data (typically working keys that are entirely managed by the local host) with a local key envelope using an AES key wrap algorithm. */
#define STSAFEA_CMD_UNWRAP_LOCAL_ENVELOPE     (0x0FU)      /*!< Unwrap Local Envelope: This command is used to unwrap a local envelope with a local envelope key. */
#define STSAFEA_CMD_PUT_ATTRIBUTE             (0x10U)      /*!< Put Attribute: Used to put attributes in the STSAFE_A110 chips like Keys, a password, the host’s public key or I2C parameters. */
#define STSAFEA_CMD_GENERATE_KEY              (0x11U)      /*!< Generate Key: This command is used to generate key pairs (asymmetric cryptography) or local envelope keys (symmetric cryptography). */
#define STSAFEA_CMD_QUERY                     (0x14U)      /*!< Query: Used to check how the chip is configured. */
#define STSAFEA_CMD_GENERATE_SIGNATURE        (0x16U)      /*!< Generate Signature: This command generates a digital signature over a message digest generated by the host. It is typically used in the IoT device authentication use case. */
#define STSAFEA_CMD_VERIFY_SIGNATURE          (0x17U)      /*!< Verify Signature: This command serves message authentication and entity authentication purposes. */
#define STSAFEA_CMD_ESTABLISH_KEY             (0x18U)      /*!< Establish Key: This command can be used to establish a shared secret between two hosts by using asymmetric cryptography. */
#define STSAFEA_CMD_VERIFY_PASSWORD           (0x1AU)      /*!< Verify Password: This command performs password verification and remembers the outcome for future authorization of Put Attribute commands. */
/**
  * @}
  */


/** @defgroup STSAFE_A110_Commands_Wait_Time STSAFE-A110's waiting time to get command's response
  * @brief STSAFE-A110 Commands Waiting Time in ms between command and response
  * @{
  */
#define STSAFEA_MS_WAIT_TIME_CMD_ECHO                     (  5U)      /*!< Echo: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_RESET                    ( 30U)      /*!< Reset: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_GENERATE_RANDOM          ( 15U)      /*!< generate Random: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_START_SESSION            ( 25U)      /*!< Start Session: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_DECREMENT                ( 15U)      /*!< Decrement: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_READ                     (  5U)      /*!< Read: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_UPDATE                   (  5U)      /*!< Update: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_HIBERNATE                ( 50U)      /*!< Hibernate: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_WRAP_LOCAL_ENVELOPE      (200U)      /*!< Wrap Local Envelope: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_UNWRAP_LOCAL_ENVELOPE    (400U)      /*!< Unwrap Local Envelope: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_PUT_ATTRIBUTE            ( 15U)      /*!< Put Attribute: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_GENERATE_KEY             ( 15U)      /*!< Generate Key: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_DELETE_KEY               ( 15U)      /*!< Delete Key: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_QUERY                    (  5U)      /*!< Query: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_GET_SIGNATURE            (450U)      /*!< Get Signature: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_GENERATE_SIGNATURE_256   ( 80U)      /*!< Generate Signature 256 bits  Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_GENERATE_SIGNATURE_384   (140U)      /*!< Generate Signature 384 bits: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_VERIFY_MSG_SIGNATURE     (145U)      /*!< Verify Message Signature: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_ESTABLISH_KEY            (200U)      /*!< Establish Key: Minimum delay in ms before response. */
#define STSAFEA_MS_WAIT_TIME_CMD_VERIFY_PASSWORD          ( 15U)      /*!< Verify Password: Minimum delay in ms before response. */
/**
  * @}
  */


/** @defgroup STSAFE_A110_Commands_Tag STSAFE-A110's command tags
  * @brief STSAFE-A110 Commands Tag
  * @{
  */
#define STSAFEA_TAG_ADMIN_BASE_KEY_SLOT                 (0x01U)      /*!< Admin base key slot tag. */
#define STSAFEA_TAG_LOCAL_ENVELOPE_KEY_TABLE            (0x07U)      /*!< Local envelope key table tag . */
#define STSAFEA_TAG_LIFE_CYCLE_STATE                    (0x08U)      /*!< Life cycle state tag. */
#define STSAFEA_TAG_PASSWORD_SLOT                       (0x09U)      /*!< Password slot tag. */
#define STSAFEA_TAG_I2C_PARAMETER                       (0x0AU)      /*!< I2C parameters tag. */
#define STSAFEA_TAG_PRODUCT_DATA                        (0x11U)      /*!< Product data tag. */
#define STSAFEA_TAG_DATA_PARTITION_CONFIGURATION        (0x12U)      /*!< Data partition configuration tag. */
#define STSAFEA_TAG_PRIVATE_KEY_SLOT                    (0x13U)      /*!< Private key slot tag. */
#define STSAFEA_TAG_PRIVATE_KEY_TABLE                   (0x14U)      /*!< Private key table tag. */
#define STSAFEA_TAG_HOST_KEY_SLOT                       (0x17U)      /*!< Host key slot tag. */
#define STSAFEA_TAG_SIGFOX_KEY_SLOT                     (0x1FU)      /*!< Sigfox key slot tag. */
#define STSAFEA_TAG_COMMAND_AUTHORIZATION_CONFIGURATION (0x24U)
#define STSAFEA_TAG_COMMAND_ACCESS_CONDITIONS           (0x29U)
#define STSAFEA_TAG_COMMAND_HOST_ENCRYPTION_FLAGS       (0x2AU)
/**
  * @}
  */


/** @defgroup STSAFE_A110_Key_Slot_Number STSAFE-A110's slot number
  * @{
  */
#define STSAFEA_KEY_SLOT_0                     (0x00U) /*!< Key Slot number 0 */
#define STSAFEA_KEY_SLOT_1                     (0x01U) /*!< Key Slot number 1 */
#define STSAFEA_KEY_SLOT_EPHEMERAL             (0xFFU) /*!< Ephemeral Key Slot number */
/**
  * @}
  */


/**
  * @}
  */


/* Exported macros -----------------------------------------------------------*/
/** @defgroup STSAFE_A110_Exported_Macros STSAFE-A110's exported macros
  * @{
  */
/******************************** IS STSAFE TAG *******************************/
#define IS_STSAFEA_TAG(TAG) (((TAG) == STSAFEA_TAG_ADMIN_BASE_KEY_SLOT)                 || \
                             ((TAG) == STSAFEA_TAG_LOCAL_ENVELOPE_KEY_TABLE)            || \
                             ((TAG) == STSAFEA_TAG_LIFE_CYCLE_STATE)                    || \
                             ((TAG) == STSAFEA_TAG_PASSWORD_SLOT)                       || \
                             ((TAG) == STSAFEA_TAG_I2C_PARAMETER)                       || \
                             ((TAG) == STSAFEA_TAG_LTC_PARAM)                           || \
                             ((TAG) == STSAFEA_TAG_PRODUCT_DATA)                        || \
                             ((TAG) == STSAFEA_TAG_DATA_PARTITION_CONFIGURATION)        || \
                             ((TAG) == STSAFEA_TAG_PRIVATE_KEY_SLOT)                    || \
                             ((TAG) == STSAFEA_TAG_PRIVATE_KEY_TABLE)                   || \
                             ((TAG) == STSAFEA_TAG_HOST_KEY_SLOT)                       || \
                             ((TAG) == STSAFEA_TAG_ADMIN_BASE_KEY_SLOT_TKM_CAPA)        || \
                             ((TAG) == STSAFEA_TAG_ADMIN_BASE_KEY_SLOT_TKM_ID)          || \
                             ((TAG) == STSAFEA_TAG_ADMIN_BASE_KEY_SLOT_LIMITS)          || \
                             ((TAG) == STSAFEA_TAG_COMMAND_AUTHORIZATION_CONFIGURATION) || \
                             ((TAG) == STSAFEA_TAG_APPTKMID)                            || \
                             ((TAG) == STSAFEA_TAG_COMMAND_ACCESS_CONDITIONS)           || \
                             ((TAG) == STSAFEA_TAG_COMMAND_HOST_ENCRYPTION_FLAGS))

/******************************* STSAFE KEY SLOTS *****************************/
#define IS_STSAFEA_KEY_SLOT(SLOT_NUM) (((SLOT_NUM) == STSAFEA_KEY_SLOT_0) || \
                                       ((SLOT_NUM) == STSAFEA_KEY_SLOT_1) || \
                                       ((SLOT_NUM) == STSAFEA_KEY_SLOT_EPHEMERAL))
/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup STSAFE_A110_CORE_Exported_Functions
  * @{
  */
/** @addtogroup CORE_Exported_Functions_Group1 Administrative Commands APIs
  *  @brief    STSAFE-A APIs related to administrative commands
  *
@verbatim
 ===============================================================================
              ##### Administrative Commands  #####
 ===============================================================================
[..]
    (+) Command Authorization Configuration Query
@endverbatim
  * @{
  */
StSafeA_ResponseCode_t StSafeA_CommandAuthorizationConfigurationQuery(
  StSafeA_Handle_t *pStSafeA,
  uint8_t InCmdAuthRecordNum,
  StSafeA_CommandAuthorizationConfigurationBuffer_t *pOutCmdAuthConfig,
  uint8_t InMAC);
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



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STSAFEA110_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
