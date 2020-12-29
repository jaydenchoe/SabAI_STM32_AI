/**
  ******************************************************************************
  * @file    HAL_STSAFE-Axx.c
  * @author  MCD Application team
  * @brief   Secure storage API
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "HAL_STSAFE-Axx_INTERNAL.h"
#include "HAL_STSAFE-Axx.h"
#include "CommonAppli.h"
#include "flash_if.h"
#include "Commonappli.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/sha256.h"
#include "stsafea_conf.h"
#include "stsafea_core.h"
#include "flash_if.h"

#define STSAFE_A_HAL_VERSION_STRING    "HAL_STSAFE-Axx 3.0.2"  /*!< Full name and version as string */

#pragma pack(push, 1)
#ifdef _MSC_VER
#pragma warning(disable : 4200)
#endif /* End _MSC_VER*/

uint8_t Check_Host_Keys(void *handle, uint8_t perso_type, uint8_t *buf);

/*!
 * \brief Initialize the library
 *
 * This command must be executed one time before using other HAL functions
 * It set communication with STSAFE-Axx, verify if STSAFE-Axx is personalized or not
 *
  * \param in/out : handle_se : Handle for STSAFE-A1xx
 * \return 0 if no error else 1
 *
 */

uint8_t Init_HAL(void *handle_se)

{
  uint8_t StatusCode = STSAFEA_OK;
  StSafeA_HostKeySlotBuffer_t HostKeySlot;

#ifdef PRINTF_ON
  printf("\r\nCheck if Pairing Host keys available \r\n");
#endif /* PRINTF */

  STS_CHK(StatusCode, StSafeA_HostKeySlotQuery(handle_se, &HostKeySlot, STSAFEA_MAC_NONE));

  if (HostKeySlot.HostKeyPresenceFlag == 0U)      /*  Not populated */
  {
#ifdef PRINTF_ON
    printf("\r\nHost key is not set, personalization of STSAFE-A has never been done before !  \r\n");
#endif /* PRINTF */
    return (STSAFEA_KEY_NOT_FOUND);
  }
#ifdef PRINTF_ON
  else
  {
    printf("\r\nHost Key is set : STSAFEA_OK \r\n");
  }
#endif /* PRINTF */

  if (StatusCode != STSAFEA_OK)
  {
#ifdef PRINTF_ON
    printf("\r\nERROR: StatusCode =0x%x \n\r", (uint8_t)StatusCode);
#endif /* PRINTF */
    return (NOK);
  }
  else
  {
    return (STSAFEA_OK);
  }

}

/*!
 * \brief Run personalization of chip
 *
 * This command must be executed one time in the life of the product
 * and does personalization of STSAFE-Axx
 *
 * \param in : handle_se : Handle for STSAFE-Axx
 * \param in : perso_type define which personalization to be ran :
 *
 *  perso_type=0: use default keys and meanwhile set buf = NULL
 *  perso_type=1: use random generated keys and meanwhile set buf = NULL
 *  perso_type=2: use the keys customers prefer and meanwhile set buf = preferred key.
 *
 * \param in : buff : Buffer which countains host keys Only used if perso_type=2
 *
 * \return 0 if no error else 1
 *
 */

uint8_t Init_Perso(void *handle_se, uint8_t perso_type, uint8_t *buf)
{
  int8_t  StatusCode = STSAFEA_OK;

  printf("Check if Pairing Host keys available \r\n");

  STS_CHK(StatusCode, Check_Host_Keys(handle_se, perso_type, buf));

#ifdef PRINTF_ON
  if (StatusCode != STSAFEA_OK)
  {
    printf("ERROR: StatusCode check_host_keys=0x%x \n\r", (uint8_t)StatusCode);
  }
  else
  {
    printf("check_host_keys STSAFEA_OK \n\r");
  }
#endif /* PRINTF_ON */

#ifdef PRINTF_ON
  printf("Set access condition to host for Zone 2 & 3 \r\n");
#endif /* PRINTF */

  STSAFEA_DYNAMIC_ALLOCATE_LV_BUFFER_VAR(StSafeA_LVBuffer_t, DatatoStore, 1 + 2);
  DatatoStore.Length = 1 ;
  DatatoStore.Data[0] = 0;

  /* Set to access condition STSAFEA_AC_HOSTSTfor zones 2 & 3 & 4 */
  for (uint8_t i = 2; i < 5; i++)
  {
    StSafeA_Update(handle_se, 0, 1, 1, STSAFEA_AC_HOST, i, DatatoStore.Length, &DatatoStore, STSAFEA_MAC_HOST_CMAC);
  }

  if (StatusCode != STSAFEA_OK)
  {
#ifdef PRINTF_ON
    printf("ERROR: StatusCode =0x%x \n\r", (uint8_t)StatusCode);
#endif /* PRINTF */
    return (NOK);
  }
  else
  {
    return (STSAFEA_OK);
  }
}

/*!
 * \brief Store data in STSAFE-Axx
 *
 * This command allow to store unencrypted data inside zone

 * \param in : handle_se : Handle for STSAFE-Axx
 * \param in : zone : zone number
 * \param in : size : Size of data to store
 * \param in : in_Data : data to store
 * \param in : in_Data : offset
 *
 * \return 0 if no error else 1
 *
 */

uint8_t HAL_Store_Data_Zone(void *handle_se, uint8_t zone, uint16_t size, uint8_t *in_Data, uint16_t offset)
{
  uint32_t  StatusCode = STSAFEA_OK;

  if (size == 0)
  {
#ifdef PRINTF_ON
    printf("Size of data's must not equals zero \n\r");
#endif /* PRINTF */
    return (STSAFEA_VALUE_OUT_OF_RANGE);
  }

  STSAFEA_DYNAMIC_ALLOCATE_LV_BUFFER_VAR(StSafeA_LVBuffer_t, DatatoStore, size + 2);

  DatatoStore.Length = size ;
  memcpy(DatatoStore.Data, in_Data, size);

  if (!StatusCode)
  {
    STS_CHK(StatusCode, UpdateZone(handle_se, zone, DatatoStore, offset));

#ifdef PRINTF_ON
    if (StatusCode != STSAFEA_OK)
    {
      printf("ERROR: StatusCode UpdateZone=0x%x \n\r", (uint8_t)StatusCode);
    }
    else
    {
      printf("UpdateZone STSAFEA_OK \n\r");
    }
#endif /* PRINTF */
  }

  STSAFEA_DYNAMIC_FREE_LV_BUFFER_VAR(DatatoStore);

  if (StatusCode != STSAFEA_OK)
  {
#ifdef PRINTF_ON
    printf("ERROR: StatusCode =0x%x \n\r", (uint8_t)StatusCode);
#endif /* PRINTF */
    return (NOK);
  }
  else
  {
    return (STSAFEA_OK);
  }
}

/*!
 * \brief Get data inside zone
 *
 * This command allow to get data encrypted inside STSAFE-Axx
 * Max datas that could be retrieved  is 800 bytes
 *
 * \param in : handle_se : Handle for STSAFE-Axx
 * \param in : zone      : zone number
 * \param in : size      : Size of data read from zone
 * \param out: buf       : Data read from zone
 * \param in : in_Data   : offset
 * \return 0 if no error else 1
 */

uint8_t HAL_Get_Data_Zone(void *handle_se, uint8_t zone, uint16_t size, uint8_t *buf, uint16_t offset)

{
  uint32_t StatusCode = STSAFEA_OK;

  if (size == 0)
  {
#ifdef PRINTF_ON
    printf("Size of data's must be in between 1 to size of zone \n\r");
#endif /* PRINTF */
    return (STSAFEA_VALUE_OUT_OF_RANGE);
  }

  STSAFEA_STATIC_ALLOCATE_LV_BUFFER_VAR(StSafeA_LVBuffer_t, sts_read,
                                        MAX_SIZE_BUFFER_I2_FOR_DATAS); /* To be improved size could be smaller  */

  if (size <= MAX_SIZE_BUFFER_I2_FOR_DATAS)
  {

    STS_CHK(StatusCode, StSafeA_Read(
              handle_se, /* handle */
              0 /*in_change_ac_indicator*/,
              0 /*in_new_read_ac_right*/,
              STSAFEA_AC_ALWAYS /*in_new_read_ac*/,
              zone,
              offset /*in_offset*/,
              size/*in_length*/,
              &sts_read /*out_read*/,
              STSAFEA_MAC_HOST_CMAC)
           );

    if (StatusCode != STSAFEA_OK)
    {
#ifdef PRINTF_ON
      printf("ERROR: StatusCode Read Zone=0x%x \n\r", (uint8_t)StatusCode);
#endif /* PRINTF */
    }
    else
    {

#ifdef PRINTF_ON
      printf("Read Zone STSAFEA_OK \n\r");
#endif /* PRINTF */
      memcpy(buf, sts_read.Data, sts_read.Length);
    }
  }
  else /* length exceed MAX_SIZE_BUFFER_I2_FOR_DATAS */
  {
#ifdef PRINTF_ON
    printf("length exceed MAX_SIZE_BUFFER_I2_FOR_DATAS it's needed to read by blocks\n\r");
#endif /* PRINTF */


    STS_CHK(StatusCode, StSafeA_Read(
              handle_se, /* handle */
              0 /*in_change_ac_indicator*/,
              0 /*in_new_read_ac_right*/,
              STSAFEA_AC_ALWAYS /*in_new_read_ac*/,
              zone,
              offset /*in_offset*/,
              MAX_SIZE_BUFFER_I2_FOR_DATAS,/*in_length*/
              &sts_read /*out_read*/,
              STSAFEA_MAC_HOST_CMAC)
           );

    memcpy(buf, sts_read.Data, sts_read.Length);
    uint16_t Remaining_Data_To_Read = size - MAX_SIZE_BUFFER_I2_FOR_DATAS;

    if (Remaining_Data_To_Read <= MAX_SIZE_BUFFER_I2_FOR_DATAS) /* 2 read is enough to read data's*/
    {

      STS_CHK(StatusCode, StSafeA_Read(
                handle_se, /* handle */
                0 /*in_change_ac_indicator*/,
                0 /*in_new_read_ac_right*/,
                STSAFEA_AC_ALWAYS /*in_new_read_ac*/,
                zone,
                offset + MAX_SIZE_BUFFER_I2_FOR_DATAS /*in_offset*/,
                Remaining_Data_To_Read,/*in_length*/
                &sts_read /*out_read*/,
                STSAFEA_MAC_HOST_CMAC)
             );

      for (uint16_t i = MAX_SIZE_BUFFER_I2_FOR_DATAS; i < size; i++)
      {
        buf[i] = sts_read.Data[i - MAX_SIZE_BUFFER_I2_FOR_DATAS];
      }

    }
    else /* 3 read is enough to read data's*/
    {

      STS_CHK(StatusCode, StSafeA_Read(
                handle_se, /* handle */
                0 /*in_change_ac_indicator*/,
                0 /*in_new_read_ac_right*/,
                STSAFEA_AC_ALWAYS /*in_new_read_ac*/,
                zone,
                offset + MAX_SIZE_BUFFER_I2_FOR_DATAS /*in_offset*/,
                MAX_SIZE_BUFFER_I2_FOR_DATAS,/*in_length*/
                &sts_read /*out_read*/,
                STSAFEA_MAC_HOST_CMAC)
             );

      for (uint16_t i = MAX_SIZE_BUFFER_I2_FOR_DATAS; i < 2 * MAX_SIZE_BUFFER_I2_FOR_DATAS; i++)
      {
        buf[i] = sts_read.Data[i - MAX_SIZE_BUFFER_I2_FOR_DATAS];
      }

      Remaining_Data_To_Read = size - 2 * MAX_SIZE_BUFFER_I2_FOR_DATAS;

      STS_CHK(StatusCode, StSafeA_Read(
                handle_se, /* handle */
                0 /*in_change_ac_indicator*/,
                0 /*in_new_read_ac_right*/,
                STSAFEA_AC_ALWAYS /*in_new_read_ac*/,
                zone,
                offset + 2 * MAX_SIZE_BUFFER_I2_FOR_DATAS /*in_offset*/,
                Remaining_Data_To_Read,/*in_length*/
                &sts_read /*out_read*/,
                STSAFEA_MAC_HOST_CMAC)
             );


      for (uint8_t i = 0; i < Remaining_Data_To_Read; i++)
      {
        buf[i + 2 * MAX_SIZE_BUFFER_I2_FOR_DATAS] = sts_read.Data[i];
      }
    }
  }

  STSAFEA_DYNAMIC_FREE_LV_BUFFER_VAR(sts_read);

  if (StatusCode != STSAFEA_OK)
  {
#ifdef PRINTF_ON
    printf("ERROR: StatusCode =0x%x \n\r", (uint8_t)StatusCode);
#endif /* PRINTF */
    return (NOK);
  }
  else
  {
    return (STSAFEA_OK);
  }
}

/*!
 * \brief Erase datas in STSAFE-Axx zone
 *
 * This command set to 0x0 STSAFE-Axx zone
 *
 * Exemple : for size=5, it set first 5 bytes of zone to 0x0

 * \param in : *handle_se : Handle for STSAFE-Axx
 * \param in : zone      : zone number
 * \param in : size      : size of data to be erased
 * \param in : in_Data   : offset
*/

uint8_t HAL_Erase_Data_Zone(void *handle_se, uint8_t zone, uint16_t size, uint16_t offset)
{
  uint32_t StatusCode = STSAFEA_OK;

  if (size == 0)
  {
#ifdef PRINTF_ON
    printf("Size of data's must be in between 1 to size of zone \n\r");
#endif /* PRINTF */
    return (STSAFEA_INCONSISTENT_COMMAND_DATA);
  }

  STSAFEA_DYNAMIC_ALLOCATE_LV_BUFFER_VAR(StSafeA_LVBuffer_t, DatatoStore, size + 2);

  DatatoStore.Length = size ;

  memset(DatatoStore.Data, 0x0, size);

  if (!StatusCode)
  {
    STS_CHK(StatusCode, UpdateZone(handle_se, zone, DatatoStore, offset));

#ifdef PRINTF_ON
    if (StatusCode != STSAFEA_OK)
    {
      printf("ERROR: StatusCode UpdateZone=0x%x \n\r", (uint8_t)StatusCode);
    }
    else
    {
      printf("UpdateZone STSAFEA_OK \n\r");
    }
#endif /* PRINTF */

  }

  STSAFEA_DYNAMIC_FREE_LV_BUFFER_VAR(DatatoStore);

  if (StatusCode != STSAFEA_OK)
  {
#ifdef PRINTF_ON
    printf("ERROR: StatusCode =0x%x \n\r", (uint8_t)StatusCode);
#endif /* PRINTF */
    return (NOK);
  }
  else
  {
    return (STSAFEA_OK);
  }
}

/*!
 * \brief Get Library version
 *
 * This command must be executed one time in the life of the product
 * \param out :  Library version
 *
 * \return 0 if no error else 1
 *
 */

void HAL_Version(char *string)
{
  if (string != NULL)
  {
    memcpy(string, STSAFE_A_HAL_VERSION_STRING, sizeof(STSAFE_A_HAL_VERSION_STRING));
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
