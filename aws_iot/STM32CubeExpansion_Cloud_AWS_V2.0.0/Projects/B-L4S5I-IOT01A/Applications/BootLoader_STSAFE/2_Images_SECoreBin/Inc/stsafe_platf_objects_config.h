/**
  ******************************************************************************
  * @file    stsafe_platf_objects_config.h
  * @author  SMD/AME application teams
  * @brief   This file contains definitions of STSAFE Platform Objects
  *          configuration.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STSAFE_PLATF_OBJECTS_CONFIG_H
#define STSAFE_PLATF_OBJECTS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "pkcs11.h"

/* Exported macros -----------------------------------------------------------*/

#define __CK_BBOOL(__CK_BBOOL__)                       (((__CK_BBOOL__) == CK_TRUE) ? &CkTrue :    \
                                                        ((__CK_BBOOL__) == CK_FALSE)  ? &CkFalse : 0)

#define __CK_OBJECT_CLASS(__CK_OBJECT_CLASS__)         (((__CK_OBJECT_CLASS__) == CKO_PRIVATE_KEY) ? &CkoPrivateKey :\
                                                        ((__CK_OBJECT_CLASS__) == CKO_PUBLIC_KEY) ? &CkoPublicKey : \
                                                        ((__CK_OBJECT_CLASS__) == CKO_CERTIFICATE) ? &CkoCertificate :\
                                                        ((__CK_OBJECT_CLASS__) == CKO_SECRET_KEY) ? &CkoSecretKey : 0 )

#define __CK_KEY_TYPE(__CK_KEY_TYPE__)                 (((__CK_KEY_TYPE__) == CKK_EC) ? &CkkEc :    \
                                                        ((__CK_KEY_TYPE__) == CKK_GENERIC_SECRET)? &CkkGenericSecret:0 )

#define __CK_MECHANISM_TYPE(__CK_MECH_TYPE__)          (((__CK_MECH_TYPE__) == CKM_EC_KEY_PAIR_GEN) ?\
                                                        &CkmEcKeyPairGen :  0)

#define __CK_CRT_TYPE(__CK_CRT_TYPE__)        (((__CK_CRT_TYPE__) == CKC_X_509) ? &CkcX509 : 0 )

#define __CK_CRT_CAT(__CK_CRT_CAT__)  (((__CK_CRT_CAT__) ==  CK_CERTIFICATE_CATEGORY_TOKEN_USER) ? \
                                       &CkCertificateCategoryTokenUser :\
                                       ((__CK_CRT_CAT__) ==  CK_CERTIFICATE_CATEGORY_AUTHORITY)  ? \
                                       &CkCertificateCategoryAuthority :  0)

#define __CK_EXT_TOK_PRVKEY_SLOT(__CK_EXT_TOK_PRVKEY_SLOT__) (((__CK_EXT_TOK_PRVKEY_SLOT__) == CKE_PRVKEY_SLOT0)?\
                                                              &CkePrvKeySlot0 :   \
                                                              ((__CK_EXT_TOK_PRVKEY_SLOT__) == CKE_PRVKEY_SLOT1)? \
                                                              &CkePrvKeySlot1 :   \
                                                              ((__CK_EXT_TOK_PRVKEY_SLOT__) == CKE_PRVKEY_SLOTFF)? \
                                                              &CkePrvKeySlotFF: 0)

#define __CK_EXT_TOK_ZONE(__CK_EXT_TOK_ZONE__)              (((__CK_EXT_TOK_ZONE__) == CKE_ZONE_0) ? &CkeZone0 :\
                                                             ((__CK_EXT_TOK_ZONE__) == CKE_ZONE_1) ? &CkeZone1 :\
                                                             ((__CK_EXT_TOK_ZONE__) == CKE_ZONE_2) ? &CkeZone2 :\
                                                             ((__CK_EXT_TOK_ZONE__) == CKE_ZONE_3) ? &CkeZone3 :\
                                                             ((__CK_EXT_TOK_ZONE__) == CKE_ZONE_4) ? &CkeZone4 : 0)

#define __CK_EXT_TOK_ZONE_AC(__CK_EXT_TOK_ZONE_AC__)        (((__CK_EXT_TOK_ZONE_AC__) == CKE_ZONE_AC_ALWAYS) ? \
                                                             &CkeZoneAcAlways :\
                                                             ((__CK_EXT_TOK_ZONE_AC__) == CKE_ZONE_AC_HOST) ?\
                                                             &CkeZoneAcHost : 0)

/* Exported types ------------------------------------------------------------*/

typedef struct
{
  CK_ATTRIBUTE_PTR pTemplate;  /*List of attributes*/
  CK_ULONG ulCount;            /*Number of CK_ATTRIBUTE for the object*/
} STSAFE_ObjectTypeDef;

/* Exported constants --------------------------------------------------------*/
#define STSAFE_P11_NUMBER_OBJECTS                            12UL               /*!< STSAFE Number of objects */
#define STSAFE_P11_MAX_TEMPLATE_SIZE                         21UL              /*!< STSAFE Max Template Size */

/*VENDOR DEFINED Type definition to map STSAFE Private Key Slots*/
#define CKA_EXT_TOKEN_PRVKEY_SLOT                             (CKA_VENDOR_DEFINED | 0x00000001UL)
typedef CK_BYTE CK_EXT_TOKEN_PRVKEY_SLOT;     /*CK EXT TOKEN PRIVATE SLOT Type*/
typedef CK_EXT_TOKEN_PRVKEY_SLOT CK_PTR CK_EXT_TOKEN_PRVKEY_SLOT_PTR;

#define CKE_PRVKEY_SLOT0                                      0x00U   /*External Token Priv Key Slot 0 */
#define CKE_PRVKEY_SLOT1                                      0x01U   /*External Token Priv Key Slot 1 */
#define CKE_PRVKEY_SLOTFF                                     0xFFU   /*External Token Priv Key Slot FF */


/*VENDOR DEFINED Type definition to map STSAFE Zones*/
#define CKA_EXT_TOKEN_ZONE                                   (CKA_VENDOR_DEFINED | 0x00000002UL)
typedef CK_BYTE CK_EXT_TOKEN_ZONE;     /*CK EXT TOKEN ZONE Type*/
typedef CK_EXT_TOKEN_ZONE CK_PTR CK_EXT_TOKEN_ZONE_PTR;

#define CKE_ZONE_0                                            0x00U   /*External Token Zone 0 */
#define CKE_ZONE_1                                            0x01U   /*External Token Zone 1 */
#define CKE_ZONE_2                                            0x02U   /*External Token Zone 2 */
#define CKE_ZONE_3                                            0x03U   /*External Token Zone 3 */
#define CKE_ZONE_4                                            0x04U   /*External Token Zone 4 */
#define CKE_ZONE_5                                            0x05U   /*External Token Zone 5 */
#define CKE_ZONE_6                                            0x06U   /*External Token Zone 6 */
#define CKE_ZONE_7                                            0x07U   /*External Token Zone 7 */

/*VENDOR DEFINED Type definition to map STSAFE ZONE Access conditions*/
#define CKA_EXT_TOKEN_ZONE_AC                                 (CKA_VENDOR_DEFINED | 0x00000003UL)
typedef CK_BYTE CK_EXT_TOKEN_ZONE_AC;     /*CK EXT TOKEN ZONE AC Type*/
typedef CK_EXT_TOKEN_ZONE_AC CK_PTR CK_EXT_TOKEN_ZONE_AC_PTR;

/*Note: only a subset of AC is used*/
#define CKE_ZONE_AC_ALWAYS                                    0x00U   /*External Token Zone Access condition  : Always
*/
#define CKE_ZONE_AC_HOST                                      0x01U   /*External Token Zone Access condition  : Host */

/*VENDOR DEFINED Type definition to map an object to a session*/
#define CKA_VENDOR_SESSION_HANDLE                             (CKA_VENDOR_DEFINED | 0x00000004UL)   /* Object session */


typedef CK_ULONG CK_VENDOR_SESSION_HANDLE;     /*CK VENDOR SESSION HANDLE*/
typedef CK_VENDOR_SESSION_HANDLE CK_PTR CK_VENDOR_SESSION_HANDLE_PTR;

#define CKA_VENDOR_OBJECT_AVAILABILITY                        (CKA_VENDOR_DEFINED | 0x00000005UL)   /* To know object availability*/

/*Note: Look for objects associated to any session*/
#define CKV_SESSION_HANDLE_ANY                            CK_INVALID_HANDLE  /*Object not associated to any session*/

/* External variables --------------------------------------------------------*/
#ifdef STSAFE_PLATF_OBJECTS_C
/**
  * @brief CK Bbool.
  * @{
  */
const CK_BBOOL CkTrue = CK_TRUE;
const CK_BBOOL CkFalse = CK_FALSE;
/**
  * @}
  */
/**
  * @brief CK Object Class.
  * @note Only a subset of values is reported here.
  * @{
  */
const CK_OBJECT_CLASS CkoPrivateKey = CKO_PRIVATE_KEY;
const CK_OBJECT_CLASS CkoPublicKey = CKO_PUBLIC_KEY;
const CK_OBJECT_CLASS CkoCertificate = CKO_CERTIFICATE;
const CK_OBJECT_CLASS CkoSecretKey = CKO_SECRET_KEY;
/**
  * @}
  */

/**
  * @brief CK Key Type.
  * @note Only a subset of values is reported here.
  * @{
  */
const CK_KEY_TYPE CkkEc = CKK_EC;
const CK_KEY_TYPE CkkGenericSecret = CKK_GENERIC_SECRET;
/**
  * @}
  */
/**
  * @brief CK Mechanism Type.
  * @note Only a subset of values is reported here.
  * @{
  */
const CK_MECHANISM_TYPE CkmEcKeyPairGen = CKM_EC_KEY_PAIR_GEN;

/**
  * @}
  */
/**
  * @brief CK Certificate Type.
  * @note Only a subset of values is reported here.
  * @{
  */
const CK_CERTIFICATE_TYPE CkcX509 = CKC_X_509;
/**
  * @}
  */
/**
  * @brief CK Certificate Category.
  * @note Only a subset of values is reported here.
  * @{
  */
const CK_CERTIFICATE_CATEGORY CkCertificateCategoryTokenUser = CK_CERTIFICATE_CATEGORY_TOKEN_USER;
const CK_CERTIFICATE_CATEGORY CkCertificateCategoryAuthority = CK_CERTIFICATE_CATEGORY_AUTHORITY;
/**
  * @}
  */

/**
  * @brief CK External Token Feature.
  * @note Only a subset of values is reported here.
  * @{
  */
const CK_EXT_TOKEN_PRVKEY_SLOT CkePrvKeySlot0 = CKE_PRVKEY_SLOT0;
const CK_EXT_TOKEN_PRVKEY_SLOT CkePrvKeySlot1 = CKE_PRVKEY_SLOT1;
const CK_EXT_TOKEN_PRVKEY_SLOT CkePrvKeySlotFF = CKE_PRVKEY_SLOTFF;

const CK_EXT_TOKEN_ZONE CkeZone0 = CKE_ZONE_0;
const CK_EXT_TOKEN_ZONE CkeZone1 = CKE_ZONE_1;
const CK_EXT_TOKEN_ZONE CkeZone2 = CKE_ZONE_2;
const CK_EXT_TOKEN_ZONE CkeZone3 = CKE_ZONE_3;
const CK_EXT_TOKEN_ZONE CkeZone4 = CKE_ZONE_4;

const CK_EXT_TOKEN_ZONE_AC CkeZoneAcAlways = CKE_ZONE_AC_ALWAYS;
const CK_EXT_TOKEN_ZONE_AC CkeZoneAcHost = CKE_ZONE_AC_HOST;

/**
  * @}
  */


/**
  * @brief Device Private Key.
  * @noteMapped to STSAFE Private Key Slot 0: CKA_EC_PARAMS depends on STSAFE configuration.
  * @{
  */

const CK_MECHANISM_TYPE pDevPrvKey_Mechanisms[] = {CKM_ECDSA};

const CK_ATTRIBUTE pDevPrvKey_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_PRIVATE_KEY), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_DEV_PRV_KEY, sizeof(P11_OBJ_LABEL_DEV_PRV_KEY) / sizeof(CK_BYTE)},
  {CKA_COPYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Key objects attributes*/
  {CKA_KEY_TYPE, (void *) __CK_KEY_TYPE(CKK_EC), sizeof(CK_KEY_TYPE)},
  {CKA_ID, (void *) P11_OBJ_ID_DEV, sizeof(P11_OBJ_ID_DEV) / sizeof(CK_BYTE)},
  {CKA_DERIVE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LOCAL, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_ALLOWED_MECHANISMS, (void *) pDevPrvKey_Mechanisms, sizeof(pDevPrvKey_Mechanisms) / sizeof(CK_MECHANISM_TYPE)},
  /* Private key objects attributes - Note: only a subset of attributes listed here */
  {CKA_SENSITIVE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_SIGN, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_EXTRACTABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_ALWAYS_SENSITIVE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_NEVER_EXTRACTABLE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  /* Elliptic curve private key objects attributes */
  {CKA_EC_PARAMS, (void *) STSAFEA_ECC_CURVE_OID_NIST_P_256_DER, STSAFEA_ECC_CURVE_OID_NIST_P_256_DER_LEN},
  {CKA_VALUE, (void *) NULL, 0x0UL},
  /* External token feature - Note: VENDOR defined attribute*/
  {
    CKA_EXT_TOKEN_PRVKEY_SLOT, (void *) __CK_EXT_TOK_PRVKEY_SLOT(CKE_PRVKEY_SLOT0),
    sizeof(CK_EXT_TOKEN_PRVKEY_SLOT)
  }
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_DevicePrivateKey =
{
  (CK_ATTRIBUTE_PTR)pDevPrvKey_Template,
  (CK_ULONG)(sizeof(pDevPrvKey_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @}
  */

/**
  * @brief Ephemeral Private Key.
  * @noteMapped to STSAFE Private Key Slot 1: CKA_EC_PARAMS depends on STSAFE configuration.
  * @{
  */

const CK_MECHANISM_TYPE pEphPrvKey_Mechanisms[] = {CKM_ECDH1_DERIVE};

const CK_ATTRIBUTE pEphPrvKey_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_PRIVATE_KEY), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_EPH_PRV_KEY, sizeof(P11_OBJ_LABEL_EPH_PRV_KEY) / sizeof(CK_BYTE) },
  {CKA_COPYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Key objects attributes*/
  {CKA_KEY_TYPE, (void *) __CK_KEY_TYPE(CKK_EC), sizeof(CK_KEY_TYPE)},
  {CKA_ID, (void *) P11_OBJ_ID_EPH, sizeof(P11_OBJ_ID_EPH) / sizeof(CK_BYTE)},
  {CKA_DERIVE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_LOCAL, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_KEY_GEN_MECHANISM, (void *) __CK_MECHANISM_TYPE(CKM_EC_KEY_PAIR_GEN), sizeof(CK_MECHANISM_TYPE)},
  {CKA_ALLOWED_MECHANISMS, (void *) pEphPrvKey_Mechanisms, sizeof(pEphPrvKey_Mechanisms) / sizeof(CK_MECHANISM_TYPE)},
  /* Private key objects attributes - Note: only a subset of attributes listed here */
  {CKA_SENSITIVE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_SIGN, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_EXTRACTABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_ALWAYS_SENSITIVE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_NEVER_EXTRACTABLE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  /* Elliptic curve private key objects attributes */
  {CKA_EC_PARAMS, (void *) STSAFEA_ECC_CURVE_OID_NIST_P_256_DER, STSAFEA_ECC_CURVE_OID_NIST_P_256_DER_LEN},
  {CKA_VALUE, (void *) NULL, 0x0UL},
  /* External token feature - Note: VENDOR defined attribute*/
  {
    CKA_EXT_TOKEN_PRVKEY_SLOT, (void *) __CK_EXT_TOK_PRVKEY_SLOT(CKE_PRVKEY_SLOTFF),
    sizeof(CK_EXT_TOKEN_PRVKEY_SLOT)
  }
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_EphemeralPrivateKey =
{
  (CK_ATTRIBUTE_PTR)pEphPrvKey_Template,
  (CK_ULONG)(sizeof(pEphPrvKey_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @brief Dynamic Private Key
  * @noteMapped to STSAFE Private Key Slot 1: CKA_EC_PARAMS depends on STSAFE-A configuration.
  * @{
  */

const CK_MECHANISM_TYPE pDynPrvKey_Mechanisms[] = {CKM_ECDSA};
CK_BBOOL bDynPrivKey_Status = CK_FALSE;

const CK_ATTRIBUTE pDynPrvKey_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_PRIVATE_KEY), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_DYN_PRV_KEY, sizeof(P11_OBJ_LABEL_DYN_PRV_KEY) / sizeof(CK_BYTE)},
  {CKA_COPYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Key objects attributes*/
  {CKA_KEY_TYPE, (void *) __CK_KEY_TYPE(CKK_EC), sizeof(CK_KEY_TYPE)},
  {CKA_ID, (void *) P11_OBJ_ID_DEV, sizeof(P11_OBJ_ID_DEV) / sizeof(CK_BYTE)},
  {CKA_DERIVE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LOCAL, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_ALLOWED_MECHANISMS, (void *) pDynPrvKey_Mechanisms, sizeof(pDynPrvKey_Mechanisms) / sizeof(CK_MECHANISM_TYPE)},
  /* Private key objects attributes - Note: only a subset of attributes listed here */
  {CKA_SENSITIVE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_SIGN, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_EXTRACTABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_ALWAYS_SENSITIVE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_NEVER_EXTRACTABLE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  /* Elliptic curve private key objects attributes */
  {CKA_EC_PARAMS, (void *) STSAFEA_ECC_CURVE_OID_NIST_P_256_DER, STSAFEA_ECC_CURVE_OID_NIST_P_256_DER_LEN},
  {CKA_VALUE, (void *) NULL, 0x0UL},
  /* External token feature - Note: VENDOR defined attribute*/
  {
    CKA_EXT_TOKEN_PRVKEY_SLOT, (void *) __CK_EXT_TOK_PRVKEY_SLOT(CKE_PRVKEY_SLOT1),
    sizeof(CK_EXT_TOKEN_PRVKEY_SLOT)
  },

  {CKA_VENDOR_OBJECT_AVAILABILITY, (void *) &bDynPrivKey_Status, sizeof(CK_BBOOL)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_DynamicPrivateKey =
{
  (CK_ATTRIBUTE_PTR)pDynPrvKey_Template,
  (CK_ULONG)(sizeof(pDynPrvKey_Template) / sizeof(CK_ATTRIBUTE)),
};



/**
  * @}
  */

/**
  * @brief Ephemeral Public Key.
  * @noteMapped to STSAFE Private Key Slot FF : CKA_EC_PARAMS and CKA_EC_POINT ulValueLen depend on STSAFE profile.
  * @{
  */

const CK_MECHANISM_TYPE pEphPubKey_Mechanisms[] = {CKM_ECDH1_DERIVE};
CK_BYTE pEphPubKey_ECPoint[3U + (STSAFEA_SHA_256_LENGTH * 2UL)];
CK_ULONG ulEphPubKey_SessionHandle;

const CK_ATTRIBUTE pEphPubKey_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_PUBLIC_KEY), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_EPH_PUB_KEY, sizeof(P11_OBJ_LABEL_EPH_PUB_KEY) / sizeof(CK_BYTE)},
  {CKA_COPYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Key objects attributes*/
  {CKA_KEY_TYPE, (void *)__CK_KEY_TYPE(CKK_EC), sizeof(CK_KEY_TYPE)},
  {CKA_ID, (void *) P11_OBJ_ID_EPH, sizeof(P11_OBJ_ID_EPH) / sizeof(CK_BYTE)},
  {CKA_LOCAL, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_KEY_GEN_MECHANISM, (void *) __CK_MECHANISM_TYPE(CKM_EC_KEY_PAIR_GEN), sizeof(CK_MECHANISM_TYPE)},
  {CKA_ALLOWED_MECHANISMS, (void *) pEphPubKey_Mechanisms, sizeof(pEphPubKey_Mechanisms) / sizeof(CK_MECHANISM_TYPE)},
  /* Public key objects attributes - Note: only a subset of attributes listed here */
  {CKA_VERIFY, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Elliptic curve public key objects attributes */
  {CKA_EC_PARAMS, (void *) STSAFEA_ECC_CURVE_OID_NIST_P_256_DER, STSAFEA_ECC_CURVE_OID_NIST_P_256_DER_LEN},
  {CKA_EC_POINT, (void *) pEphPubKey_ECPoint, sizeof(pEphPubKey_ECPoint) / sizeof(CK_BYTE)},
  /* Session Handle - Note: VENDOR defined attribute*/
  {CKA_VENDOR_SESSION_HANDLE, (void *) &ulEphPubKey_SessionHandle, sizeof(CK_ULONG)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_EphemeralPublicKey =
{
  (CK_ATTRIBUTE_PTR)pEphPubKey_Template,
  (CK_ULONG)(sizeof(pEphPubKey_Template) / sizeof(CK_ATTRIBUTE)),
};
/**
  * @}
  */

/**
  * @brief Dynamic Public Key.
  * @noteMapped to STSAFE Private Key Slot 1: CKA_EC_PARAMS and CKA_EC_POINT ulValueLen depend on STSAFE profile.
  * @{
  */

const CK_MECHANISM_TYPE pDynPubKey_Mechanisms[] = {CKM_ECDSA};
CK_BYTE pDynPubKey_ECPoint[3U + (STSAFEA_SHA_256_LENGTH * 2UL)];
CK_ULONG ulDynPubKey_SessionHandle;
CK_BBOOL bDynPubKey_Status=CK_FALSE;

const CK_ATTRIBUTE pDynPubKey_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_PUBLIC_KEY), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_DYN_PUB_KEY, sizeof(P11_OBJ_LABEL_DYN_PUB_KEY) / sizeof(CK_BYTE)},
  {CKA_COPYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Key objects attributes*/
  {CKA_KEY_TYPE, (void *)__CK_KEY_TYPE(CKK_EC), sizeof(CK_KEY_TYPE)},
  {CKA_ID, (void *) P11_OBJ_ID_DYN, sizeof(P11_OBJ_ID_DYN) / sizeof(CK_BYTE)},
  {CKA_LOCAL, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_KEY_GEN_MECHANISM, (void *) __CK_MECHANISM_TYPE(CKM_EC_KEY_PAIR_GEN), sizeof(CK_MECHANISM_TYPE)},
  {CKA_ALLOWED_MECHANISMS, (void *) pDynPubKey_Mechanisms, sizeof(pDynPubKey_Mechanisms) / sizeof(CK_MECHANISM_TYPE)},
  /* Public key objects attributes - Note: only a subset of attributes listed here */
  {CKA_VERIFY, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  /* Elliptic curve public key objects attributes */
  {CKA_EC_PARAMS, (void *) STSAFEA_ECC_CURVE_OID_NIST_P_256_DER, STSAFEA_ECC_CURVE_OID_NIST_P_256_DER_LEN},
  {CKA_EC_POINT, (void *) pDynPubKey_ECPoint, sizeof(pDynPubKey_ECPoint) / sizeof(CK_BYTE)},
  {CKA_VENDOR_OBJECT_AVAILABILITY, (void *) &bDynPubKey_Status, sizeof(CK_BBOOL)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_DynamicPublicKey =
{
  (CK_ATTRIBUTE_PTR)pDynPubKey_Template,
  (CK_ULONG)(sizeof(pDynPubKey_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @}
  */


/**
  * @brief Verification Public Key - 256 bit.
  * @{
  */

const CK_MECHANISM_TYPE pVerPubKey256_Mech[] = {CKM_ECDSA};
CK_BYTE pVerPubKey256_ECPoint[3U + (STSAFEA_SHA_256_LENGTH * 2UL)]; /*Including DER header: (0x04 | Len | 0x04 )*/
CK_ULONG ulVerPubKey256_SessionHandle;

const CK_ATTRIBUTE pVerPubKey256_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_PUBLIC_KEY), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_VER_PUB_KEY_256, sizeof(P11_OBJ_LABEL_VER_PUB_KEY_256) / sizeof(CK_BYTE)},
  {CKA_COPYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Key objects attributes*/
  {CKA_KEY_TYPE, (void *) __CK_KEY_TYPE(CKK_EC), sizeof(CK_KEY_TYPE)},
  {CKA_ID, (void *) P11_OBJ_ID_VER_256, sizeof(P11_OBJ_ID_VER_256) / sizeof(CK_BYTE)},
  {CKA_LOCAL, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_ALLOWED_MECHANISMS, (void *) pVerPubKey256_Mech, sizeof(pVerPubKey256_Mech) / sizeof(CK_MECHANISM_TYPE)},
  /* Public key objects attributes - Note: only a subset of attributes listed here */
  {CKA_VERIFY, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  /* Elliptic curve public key objects attributes */
  {CKA_EC_PARAMS, (void *) STSAFEA_ECC_CURVE_OID_NIST_P_256_DER, STSAFEA_ECC_CURVE_OID_NIST_P_256_DER_LEN },
  /*Including the ASN header: (0x06 | Len )*/
  { CKA_EC_POINT, (void *) pVerPubKey256_ECPoint, sizeof(pVerPubKey256_ECPoint) / sizeof(CK_BYTE)},
  /* Session Handle - Note: VENDOR defined attribute*/
  {CKA_VENDOR_SESSION_HANDLE, (void *) &ulVerPubKey256_SessionHandle, sizeof(CK_ULONG)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_VerificationPubKey256 =
{
  (CK_ATTRIBUTE_PTR)pVerPubKey256_Template,
  (CK_ULONG)(sizeof(pVerPubKey256_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @}
  */


/**
  * @brief Verification Public Key - 384 bit.
  * @{
  */

const CK_MECHANISM_TYPE pVerPubKey384_Mech[] = {CKM_ECDSA};
CK_BYTE pVerPubKey384_ECPoint[3U + (STSAFEA_SHA_384_LENGTH * 2UL)];/*Including DER header: (0x04 | Len | 0x04 )*/
CK_ULONG ulVerPubKey384_SessionHandle;

const CK_ATTRIBUTE pVerPubKey384_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_PUBLIC_KEY), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_VER_PUB_KEY_384, sizeof(P11_OBJ_LABEL_VER_PUB_KEY_384) / sizeof(CK_BYTE)},
  {CKA_COPYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Key objects attributes*/
  {CKA_KEY_TYPE, (void *) __CK_KEY_TYPE(CKK_EC), sizeof(CK_KEY_TYPE)},
  {CKA_ID, (void *) P11_OBJ_ID_VER_384, sizeof(P11_OBJ_ID_VER_384) / sizeof(CK_BYTE)},
  {CKA_LOCAL, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_ALLOWED_MECHANISMS, (void *) pVerPubKey384_Mech, sizeof(pVerPubKey384_Mech) / sizeof(CK_MECHANISM_TYPE)},
  /* Public key objects attributes - Note: only a subset of attributes listed here */
  {CKA_VERIFY, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  /* Elliptic curve public key objects attributes */
  {CKA_EC_PARAMS, (void *) STSAFEA_ECC_CURVE_OID_NIST_P_384_DER, STSAFEA_ECC_CURVE_OID_NIST_P_384_DER_LEN },
  {CKA_EC_POINT, (void *) pVerPubKey384_ECPoint, sizeof(pVerPubKey384_ECPoint) / sizeof(CK_BYTE)},
  /* Session Handle - Note: VENDOR defined attribute*/
  {CKA_VENDOR_SESSION_HANDLE, (void *) &ulVerPubKey256_SessionHandle, sizeof(CK_ULONG)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_VerificationPubKey384 =
{
  (CK_ATTRIBUTE_PTR)pVerPubKey384_Template,
  (CK_ULONG)(sizeof(pVerPubKey384_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @}
  */

/**
  * @brief DH Shared Secret X Value
  * @note: CKA_VALUE_LEN and CKA_EC_POINT ulValueLen have
  *to be consistent with Ephemeral Private / Public Key (256/384 bit).
  * @{
  */
/*Note: following parameters to be consistent with Ephemeral Private & Public Key type (256/384 bit)*/
CK_BYTE pDhSecretX_CKA_VALUE[STSAFEA_SHA_256_LENGTH];
const CK_ULONG DhSecretX_CKA_VALUE_LEN = STSAFEA_SHA_256_LENGTH;
CK_ULONG ulDhSecretX_SessionHandle;

const CK_ATTRIBUTE pDhSecretX_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_SECRET_KEY), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_DH_SECRET_X, sizeof(P11_OBJ_LABEL_DH_SECRET_X) / sizeof(CK_BYTE)},
  {CKA_COPYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Key objects attributes*/
  {CKA_KEY_TYPE, (void *) __CK_KEY_TYPE(CKK_GENERIC_SECRET), sizeof(CK_KEY_TYPE)},
  {CKA_ID, (void *) P11_OBJ_ID_DH_SECRET_X, sizeof(P11_OBJ_ID_DH_SECRET_X) / sizeof(CK_BYTE)},
  {CKA_LOCAL, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  /* Secret key objects attributes- Note: only a subset of attributes listed here */
  {CKA_SENSITIVE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_VERIFY, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_SIGN, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_EXTRACTABLE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_ALWAYS_SENSITIVE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_NEVER_EXTRACTABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_VALUE, (void *) pDhSecretX_CKA_VALUE, sizeof(pDhSecretX_CKA_VALUE) / sizeof(CK_BYTE)},
  {CKA_VALUE_LEN, (void *) &DhSecretX_CKA_VALUE_LEN, sizeof(CK_ULONG)},
  /* Session Handle - Note: VENDOR defined attribute*/
  {CKA_VENDOR_SESSION_HANDLE, (void *) &ulDhSecretX_SessionHandle, sizeof(CK_ULONG)}

};

const STSAFE_ObjectTypeDef STSAFE_OBJ_DHSharedSecretX =
{
  (CK_ATTRIBUTE_PTR)pDhSecretX_Template,
  (CK_ULONG)(sizeof(pDhSecretX_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @}
  */

/**
  * @brief Device Certificate
  * @{
  */
const CK_ATTRIBUTE pDevCrt_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_CERTIFICATE), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_COPYABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_DEV_CRT, sizeof(P11_OBJ_LABEL_DEV_CRT) / sizeof(CK_BYTE)},
  /* Certificate objects attributes*/
  {CKA_CERTIFICATE_TYPE, (void *)__CK_CRT_TYPE(CKC_X_509), sizeof(CK_CERTIFICATE_TYPE)},
  {CKA_CERTIFICATE_CATEGORY, (void *)__CK_CRT_CAT(CK_CERTIFICATE_CATEGORY_TOKEN_USER), sizeof(CK_CERTIFICATE_CATEGORY)},
  /* X.509 public key certificate objects - Note: only a subset of attributes listed here */
  {CKA_ID, (void *) P11_OBJ_ID_DEV, sizeof(P11_OBJ_ID_DEV) / sizeof(CK_BYTE)},
  {CKA_VALUE, (void *) NULL, 0x0UL},
  /* External token feature - Note: VENDOR defined attribute*/
  {CKA_EXT_TOKEN_ZONE, (void *)__CK_EXT_TOK_ZONE(CKE_ZONE_0), sizeof(CK_EXT_TOKEN_ZONE)},
  {CKA_EXT_TOKEN_ZONE_AC, (void *)__CK_EXT_TOK_ZONE_AC(CKE_ZONE_AC_HOST), sizeof(CK_EXT_TOKEN_ZONE_AC)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_DeviceCertificate =
{
  (CK_ATTRIBUTE_PTR)pDevCrt_Template,
  (CK_ULONG)(sizeof(pDevCrt_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @}
  */

/**
  * @brief Dynamic Certificate associated with zone 4 of STSAFE-A110
  * @{
  */

CK_BBOOL bDynCrt_Status=CK_FALSE;

const CK_ATTRIBUTE pDynCrt_Template[] =
{
  {CKA_CLASS, (void *) __CK_OBJECT_CLASS(CKO_CERTIFICATE), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *) __CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *) __CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_COPYABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *)__CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_DYN_CRT, sizeof(P11_OBJ_LABEL_DYN_CRT) / sizeof(CK_BYTE)},
  /* Certificate objects attributes*/
  {CKA_CERTIFICATE_TYPE, (void *)__CK_CRT_TYPE(CKC_X_509), sizeof(CK_CERTIFICATE_TYPE)},
  {CKA_CERTIFICATE_CATEGORY, (void *)__CK_CRT_CAT(CK_CERTIFICATE_CATEGORY_TOKEN_USER), sizeof(CK_CERTIFICATE_CATEGORY)},
  /* X.509 public key certificate objects - Note: only a subset of attributes listed here */
  {CKA_ID, (void *) P11_OBJ_ID_DYN, sizeof(P11_OBJ_ID_DYN) / sizeof(CK_BYTE)},
  {CKA_VALUE, (void *) NULL, 0x0UL},
  /* External token feature - Note: VENDOR defined attribute*/
  {CKA_EXT_TOKEN_ZONE, (void *)__CK_EXT_TOK_ZONE(CKE_ZONE_4), sizeof(CK_EXT_TOKEN_ZONE)},
  {CKA_EXT_TOKEN_ZONE_AC, (void *)__CK_EXT_TOK_ZONE_AC(CKE_ZONE_AC_HOST), sizeof(CK_EXT_TOKEN_ZONE_AC)},
  {CKA_VENDOR_OBJECT_AVAILABILITY, (void *) &bDynCrt_Status, sizeof(CK_BBOOL)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_DynamicCertificate =
{
  (CK_ATTRIBUTE_PTR)pDynCrt_Template,
  (CK_ULONG)(sizeof(pDynCrt_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @}
  */

/**
  * @brief SBSFU Root CA Certificate
  * @{
  */

const CK_ATTRIBUTE pSBSFUCACrt_Template[] =
{
  {CKA_CLASS, (void *)__CK_OBJECT_CLASS(CKO_CERTIFICATE), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *)__CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_COPYABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_SBSFU_ROOT_CA_CRT, sizeof(P11_OBJ_LABEL_SBSFU_ROOT_CA_CRT) / sizeof(CK_BYTE)},
  /* Certificate objects attributes*/
  {CKA_CERTIFICATE_TYPE, (void *)__CK_CRT_TYPE(CKC_X_509), sizeof(CK_CERTIFICATE_TYPE)},
  {CKA_CERTIFICATE_CATEGORY, (void *)__CK_CRT_CAT(CK_CERTIFICATE_CATEGORY_AUTHORITY), sizeof(CK_CERTIFICATE_CATEGORY)},
  /* X.509 public key certificate objects - Note: only a subset of attributes listed here */
  {CKA_ID, (void *) P11_OBJ_ID_SBSFU_ROOT_CA_CRT, sizeof(P11_OBJ_ID_SBSFU_ROOT_CA_CRT) / sizeof(CK_BYTE)},
  {CKA_VALUE, (void *) NULL, 0x0UL},
  /* External token feature - Note: VENDOR defined attribute*/
  {CKA_EXT_TOKEN_ZONE, (void *)__CK_EXT_TOK_ZONE(CKE_ZONE_2), sizeof(CK_EXT_TOKEN_ZONE)},
  {CKA_EXT_TOKEN_ZONE_AC, (void *)__CK_EXT_TOK_ZONE_AC(CKE_ZONE_AC_HOST), sizeof(CK_EXT_TOKEN_ZONE_AC)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_SBSFURootCACertificate =
{
  (CK_ATTRIBUTE_PTR)pSBSFUCACrt_Template,
  (CK_ULONG)(sizeof(pSBSFUCACrt_Template) / sizeof(CK_ATTRIBUTE)),
};
/**
  * @}
  */


/**
  * @brief OEM Interm CA Certificate
  * @{
  */

const CK_ATTRIBUTE pOEMIntmCACrt_Template[] =
{
  {CKA_CLASS, (void *)__CK_OBJECT_CLASS(CKO_CERTIFICATE), sizeof(CK_OBJECT_CLASS)},
  /* Storage objects attributes*/
  {CKA_TOKEN, (void *)__CK_BBOOL(CK_TRUE), sizeof(CK_BBOOL)},
  {CKA_PRIVATE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_MODIFIABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_COPYABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_DESTROYABLE, (void *)__CK_BBOOL(CK_FALSE), sizeof(CK_BBOOL)},
  {CKA_LABEL, (void *) P11_OBJ_LABEL_OEM_INTM_CA_CRT, sizeof(P11_OBJ_LABEL_OEM_INTM_CA_CRT) / sizeof(CK_BYTE)},
  /* Certificate objects attributes*/
  {CKA_CERTIFICATE_TYPE, (void *)__CK_CRT_TYPE(CKC_X_509), sizeof(CK_CERTIFICATE_TYPE)},
  {CKA_CERTIFICATE_CATEGORY, (void *)__CK_CRT_CAT(CK_CERTIFICATE_CATEGORY_AUTHORITY), sizeof(CK_CERTIFICATE_CATEGORY)},
  /* X.509 public key certificate objects - Note: only a subset of attributes listed here */
  {CKA_ID, (void *) P11_OBJ_ID_OEM_INTM_CA_CRT, sizeof(P11_OBJ_ID_OEM_INTM_CA_CRT) / sizeof(CK_BYTE)},
  {CKA_VALUE, (void *) NULL, 0x0UL},
  /* External token feature - Note: VENDOR defined attribute*/
  {CKA_EXT_TOKEN_ZONE, (void *)__CK_EXT_TOK_ZONE(CKE_ZONE_3), sizeof(CK_EXT_TOKEN_ZONE)},
  {CKA_EXT_TOKEN_ZONE_AC, (void *)__CK_EXT_TOK_ZONE_AC(CKE_ZONE_AC_HOST), sizeof(CK_EXT_TOKEN_ZONE_AC)}
};

const STSAFE_ObjectTypeDef STSAFE_OBJ_OEMIntermCACertificate =
{
  (CK_ATTRIBUTE_PTR)pOEMIntmCACrt_Template,
  (CK_ULONG)(sizeof(pOEMIntmCACrt_Template) / sizeof(CK_ATTRIBUTE)),
};

/**
  * @}
  */

/**
  * @brief Embedded Object List.
  * @{
  */

const STSAFE_ObjectTypeDef *pSTSAFE_OBJ_List[STSAFE_P11_NUMBER_OBJECTS] =
{
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_DevicePrivateKey, /* Device Private Key */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_DeviceCertificate,/* Device Certificate */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_DynamicCertificate,/* Dynamic Certificate */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_EphemeralPrivateKey,/* Ephemeral PrivateKey */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_DynamicPrivateKey,/* Ephemeral PrivateKey */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_EphemeralPublicKey, /* Ephemeral PublicKey */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_DynamicPublicKey, /* Dynamic PublicKey */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_VerificationPubKey256,/* Msg Verification PublicKey-256*/
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_VerificationPubKey384,/* Msg Verification PublicKey-384 */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_DHSharedSecretX,/* DH Shared secret (X coordinate)*/
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_SBSFURootCACertificate, /* SBSFU Root CA Certificate */
  (STSAFE_ObjectTypeDef *) &STSAFE_OBJ_OEMIntermCACertificate, /* OEM Interm CA Certificate */
};

/**
  * @}
  */

#endif /*STSAFE_PLATF_OBJECTS_C*/

/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /*STSAFE_PLATF_OBJECTS_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
