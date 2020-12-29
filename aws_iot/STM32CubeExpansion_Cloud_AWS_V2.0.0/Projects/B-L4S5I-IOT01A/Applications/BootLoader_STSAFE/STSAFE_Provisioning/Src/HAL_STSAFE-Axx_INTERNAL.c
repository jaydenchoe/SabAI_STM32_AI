/**
  ******************************************************************************
  * @file    HAL_STSAFE-Axx_INTERNAL.c
  * @author  MCD Application team
  * @brief   Secure storage API, functions not exported to the outside world
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
#include "CommonAppli.h"
#include "flash_if.h"
#include "HAL_STSAFE-Axx.h"
#include "flash_if.h"

#pragma pack(push, 1)
#ifdef _MSC_VER
#pragma warning(disable : 4200)
#endif /* _MSC_VER */


/* Public function prototypes -----------------------------------------------*/

/*!
 * \brief Update data through zone partition
 *
 * \param *handle                : Handle for STSAFEA_A interface (must be created before any usage)
 * \param zone                   : Zone number
 * \param in_Data                : Data to copy inside zone
 * \param offset                 : Offset in selected zone
 * \return OK if no error
 */

uint8_t UpdateZone(void *handle, uint8_t zone, StSafeA_LVBuffer_t in_Data, uint16_t offset)
{
  uint8_t StatusCode = STSAFEA_OK;
  uint16_t Remaining_Data_To_Copy = 0;

  if (in_Data.Length <= MAX_SIZE_BUFFER_I2_FOR_DATAS)
  {

    STS_CHK(StatusCode, StSafeA_Update(handle, 0, 0, 0, 0, zone, offset, &in_Data, STSAFEA_MAC_HOST_CMAC));
    
#ifdef PRINTF_ON
    if (StatusCode != STSAFEA_OK)
    {
      printf("ERROR: StatusCode StSafeA_Update=0x%x \n\r", (uint8_t)StatusCode);
    }
    else
    {
      printf("StSafeA_Update STSAFEA_OK \n\r");
    }
#endif /* PRINTF */
  }
  else /* Size of datas to be send exceed max size of STSAFE-Axx buffer */
  {
#ifdef PRINTF_ON
    printf("Size of data's to be send exceed max size of STSAFE-Axx buffer \n\r");
    printf("We have to split data's in blocks \n\r");
#endif /* PRINTF */

    uint16_t size = in_Data.Length;

    /* First Write of datas*/

    in_Data.Length = MAX_SIZE_BUFFER_I2_FOR_DATAS;

    STS_CHK(StatusCode, StSafeA_Update(handle, 0, 0, 0, 0, zone, offset, &in_Data, STSAFEA_MAC_HOST_CMAC));

#ifdef PRINTF_ON

    if (StatusCode != STSAFEA_OK)
    {
      printf("ERROR: StatusCode StSafeA_Update=0x%x \n\r", (uint8_t)StatusCode);
    }
    else
    {
      printf("First StSafeA_Update STSAFEA_OK \n\r");
    }

    printf("Size of data to copy %d \n\r", size - MAX_SIZE_BUFFER_I2_FOR_DATAS);

#endif /* PRINTF */
    /* Management of 2nd block of datas with max datas of 1000 bytes*/
    /* Check if it's needed to split in 2 */

    Remaining_Data_To_Copy = size - MAX_SIZE_BUFFER_I2_FOR_DATAS;

    if (Remaining_Data_To_Copy <= MAX_SIZE_BUFFER_I2_FOR_DATAS)
    {
      STSAFEA_DYNAMIC_ALLOCATE_LV_BUFFER_VAR(StSafeA_LVBuffer_t, DataUpdate, Remaining_Data_To_Copy + 2);
      DataUpdate.Length = Remaining_Data_To_Copy;

      for (uint8_t i = 0; i < DataUpdate.Length; i++)
      {
        DataUpdate.Data[i] = in_Data.Data[i + MAX_SIZE_BUFFER_I2_FOR_DATAS];
      }

      /* 2nd Write of datas*/

      STS_CHK(StatusCode, StSafeA_Update(handle, 0, 0, 0, 0, zone, MAX_SIZE_BUFFER_I2_FOR_DATAS + offset, &DataUpdate,
                                         STSAFEA_MAC_HOST_CMAC));

#ifdef PRINTF_ON

      if (StatusCode != STSAFEA_OK)
      {
        printf("ERROR: StatusCode StSafeA_Update=0x%x \n\r", (uint8_t)StatusCode);
      }
      else
      {
        printf("Second StSafeA_Update STSAFEA_OK \n\r");
      }

      printf("Size of data to copy %d \n\r", Remaining_Data_To_Copy);

#endif /* PRINTF */
      STSAFEA_DYNAMIC_FREE_LV_BUFFER_VAR(DataUpdate);
    }
    else /*  it's needed to split in 3 blocks*/
    {
#ifdef PRINTF_ON
      printf("%d remaining data's, it's needed to split in 2 more blocks \n\r", Remaining_Data_To_Copy);
#endif /* PRINTF */

      STSAFEA_DYNAMIC_ALLOCATE_LV_BUFFER_VAR(StSafeA_LVBuffer_t, DataUpdate, MAX_SIZE_BUFFER_I2_FOR_DATAS + 2);

      DataUpdate.Length = MAX_SIZE_BUFFER_I2_FOR_DATAS;

      for (uint8_t i = 0; i < DataUpdate.Length; i++)
      {
        DataUpdate.Data[i] = in_Data.Data[i + MAX_SIZE_BUFFER_I2_FOR_DATAS];
      }

      /* 2nd Write of datas*/

      STS_CHK(StatusCode, StSafeA_Update(handle, 0, 0, 0, 0, zone, MAX_SIZE_BUFFER_I2_FOR_DATAS + offset, &DataUpdate,
                                         STSAFEA_MAC_HOST_CMAC));

      STSAFEA_DYNAMIC_FREE_LV_BUFFER_VAR(DataUpdate);

#ifdef PRINTF_ON
      if (StatusCode != STSAFEA_OK)
      {
        printf("ERROR: StatusCode StSafeA_Update=0x%x \n\r", (uint8_t)StatusCode);
      }
      else
      {
        printf("Second StSafeA_Update STSAFEA_OK \n\r");
      }
#endif /* PRINTF */
      Remaining_Data_To_Copy = size - 2 * MAX_SIZE_BUFFER_I2_FOR_DATAS;

#ifdef PRINTF_ON

      printf("Remaining Size of data to copy %d \n\r", Remaining_Data_To_Copy);

#endif /* PRINTF */
      STSAFEA_DYNAMIC_ALLOCATE_LV_BUFFER_VAR(StSafeA_LVBuffer_t, DataUpdate2, Remaining_Data_To_Copy + 2);
      DataUpdate2.Length = Remaining_Data_To_Copy;

      for (uint8_t i = 0; i < DataUpdate2.Length; i++)
      {
        DataUpdate2.Data[i] = in_Data.Data[i + 2 * MAX_SIZE_BUFFER_I2_FOR_DATAS];
      }

      /* third Write of datas*/

      STS_CHK(StatusCode, StSafeA_Update(handle,
                                         0,
                                         0,
                                         0,
                                         0,
                                         zone,
                                         2 * MAX_SIZE_BUFFER_I2_FOR_DATAS + offset, &DataUpdate2,
                                         STSAFEA_MAC_HOST_CMAC
                                        ));

#ifdef PRINTF_ON

      if (StatusCode != STSAFEA_OK)
      {
        printf("ERROR: StatusCode StSafeA_Update=0x%x \n\r", (uint8_t)StatusCode);
      }
      else
      {
        printf("Third StSafeA_Update STSAFEA_OK \n\r");
      }
#endif /* PRINTF */
      STSAFEA_DYNAMIC_FREE_LV_BUFFER_VAR(DataUpdate2);
    }
  }
  return (StatusCode);
}

/*!
 * \brief Check host keys presence and if not existing enable it in STSAFE-Axx and store value in flash
 *
 * \param *handle                : Handle for STSAFEA_A interface (must be created before any usage)
 * \param perso_type             : type of perso as explained below
 *
 *    type=0: use default keys and meanwhile set buf = NULL
 *    type=1: use random generated keys and meanwhile set buf = NULL
 *    type=2: use the keys customers prefer and meanwhile set buf = preferred key.
 *
 * \param buf                   : Host Mac and Cipher key value
 * \return OK if no error
 */

uint8_t Check_Host_Keys(void *handle, uint8_t perso_type, uint8_t *buf)
{
  uint8_t StatusCode = 0;
  uint8_t Host_MAC_Cipher_Key[SIZE_HOST_MAC_CIPHER_KEY] =
  {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, /* Host MAC key */
    0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88 /* Host cipher key */
  };
  uint32_t i;
  StSafeA_HostKeySlotBuffer_t HostKeySlot;

  switch (perso_type)
  {
    case 0 :
#ifdef PRINTF_ON
      printf("use default keys and meanwhile set buf = NULL\n");
#endif /* PRINTF */
      break;
    case 1 :
#ifdef PRINTF_ON
      printf("use random generated keys and meanwhile set buf = NULL\n");
#endif /* PRINTF */
      STS_CHK(StatusCode, GenerateUnsignedChallenge(handle,
                                                    sizeof(Host_MAC_Cipher_Key) / sizeof(Host_MAC_Cipher_Key[0]),
                                                    Host_MAC_Cipher_Key
                                                   ));
      break;
    case 2 :
#ifdef PRINTF_ON
      printf("use the keys customers prefer and meanwhile set buf = preferred key\n");
#endif /* PRINTF */
      memcpy(Host_MAC_Cipher_Key, buf, 32);

      break;
#ifdef PRINTF_ON
    default :
      printf("Invalid choice\n");
#endif /* PRINTF */
  }
  /* Check if host cipher key & host MAC key are populated */
  STS_CHK(StatusCode, StSafeA_HostKeySlotQuery(handle, &HostKeySlot, STSAFEA_MAC_NONE));

  if (HostKeySlot.HostKeyPresenceFlag == 0U)      /* Not populated */
    STS_CHK(StatusCode, StSafeA_PutAttribute(handle, STSAFEA_TAG_HOST_KEY_SLOT, Host_MAC_Cipher_Key,
                                             sizeof(Host_MAC_Cipher_Key) / sizeof(Host_MAC_Cipher_Key[0]),
                                             STSAFEA_MAC_NONE));

  /* Store in flash Host Mac & Data encryption key */
  /* Generating code for key storing as ARM instructions */

  /* Values of Host keys written in below code are loaded in STM32 register r0 to r7 */
  /* This is the function PCROP_Write_Mac_Keys_To_Registers called by secure bootloader */
  /* This function will be written in PCROP area */
  /* on then flushed in RAM with pointer given as argument */

  uint16_t PCROP_Code_buff[44];

  PCROP_Code_buff[0] =  0xb4f0;
  PCROP_Code_buff[1] =  0xf240;
  PCROP_Code_buff[2] =  0x0000;
  PCROP_Code_buff[3] =  0xf2c0;
  PCROP_Code_buff[4] =  0x0000;
  PCROP_Code_buff[5] =  0xf240;
  PCROP_Code_buff[6] =  0x0000;
  PCROP_Code_buff[7] =  0xf2c0;
  PCROP_Code_buff[8] =  0x0000;
  PCROP_Code_buff[9] =  0xf240;
  PCROP_Code_buff[10] = 0x0000;
  PCROP_Code_buff[11] = 0xf2c0;
  PCROP_Code_buff[12] = 0x0000;
  PCROP_Code_buff[13] = 0xf240;
  PCROP_Code_buff[14] = 0x0000;
  PCROP_Code_buff[15] = 0xf2c0;
  PCROP_Code_buff[16] = 0x0000;
  PCROP_Code_buff[17] = 0xe880;
  PCROP_Code_buff[18] = 0x00f0;
  PCROP_Code_buff[19] = 0xbcf0;
  PCROP_Code_buff[20] = 0x4770;
  PCROP_Code_buff[21] = 0x0000;
  PCROP_Code_buff[22] = 0xb4f0;
  PCROP_Code_buff[23] = 0xf240;
  PCROP_Code_buff[24] = 0x0000;
  PCROP_Code_buff[25] = 0xf2c0;
  PCROP_Code_buff[26] = 0x0000;
  PCROP_Code_buff[27] = 0xf240;
  PCROP_Code_buff[28] = 0x0000;
  PCROP_Code_buff[29] = 0xf2c0;
  PCROP_Code_buff[30] = 0x0000;
  PCROP_Code_buff[31] = 0xf240;
  PCROP_Code_buff[32] = 0x0000;
  PCROP_Code_buff[33] = 0xf2c0;
  PCROP_Code_buff[34] = 0x0000;
  PCROP_Code_buff[35] = 0xf240;
  PCROP_Code_buff[36] = 0x0000;
  PCROP_Code_buff[37] = 0xf2c0;
  PCROP_Code_buff[38] = 0x0000;
  PCROP_Code_buff[39] = 0xe880;
  PCROP_Code_buff[40] = 0x00f0;
  PCROP_Code_buff[41] = 0xbcf0;
  PCROP_Code_buff[42] = 0x4770;
  PCROP_Code_buff[43] = 0x0000;

  for (i = 0; i < 8; i++)
  {
    unsigned char zz;
    unsigned char yy;

    zz = Host_MAC_Cipher_Key[(2 * i) + 1 + 16];
    yy = Host_MAC_Cipher_Key[2 * i + 16];

    PCROP_Code_buff[1 + 2 * i] |= (((zz & 0x08) >> 1) << 8) | ((zz & 0xF0) >> 4);
    PCROP_Code_buff[1 + 2 * i + 1] |= ((((zz & 0x07) << 4) | (4 + i / 2)) << 8) | yy;

    zz = Host_MAC_Cipher_Key[(2 * i) + 1];
    yy = Host_MAC_Cipher_Key[2 * i ];
    PCROP_Code_buff[23 + 2 * i] |= (((zz & 0x08) >> 1) << 8) | ((zz & 0xF0) >> 4);
    PCROP_Code_buff[23 + 2 * i + 1] |= ((((zz & 0x07) << 4) | (4 + i / 2)) << 8) | yy;
  }
  /* Store Keys in clear inside STM32 flash*/
  STS_CHK(StatusCode, WriteUnsignedCharArrayToFlash((unsigned char *)Host_MAC_Cipher_Key, SIZE_HOST_MAC_CIPHER_KEY,
                                                    KEY_ADDR));
  /* Store executable code into STM32 flash */
  STS_CHK(StatusCode, WriteUnsignedCharArrayToFlash((unsigned char *)PCROP_Code_buff, sizeof(PCROP_Code_buff),
                                                    PCROP_ADDR));

  return StatusCode;
}

/* Private function prototypes -----------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/*!
 * \brief Write unsigned char array in STM32 flash at choosen adress
 *
 * \param in : *MyArray : Array to write
 * \param in : SizeMyArray : zize of array
 * \param in : Addr : Address to write in STM32 flash
 * \return OK if no error
 */

uint8_t WriteUnsignedCharArrayToFlash(unsigned char *MyArray, uint8_t SizeMyArray, uint32_t Addr)
{
  uint8_t StatusCode = 0;

  /* Erase page before programming  */
  STS_CHK(StatusCode, FLASH_If_Erase_NumPages(Addr, SizeMyArray));

  StatusCode |= HAL_FLASH_Unlock();

  /* Programming flash */
  for (uint8_t i = 0; i < SizeMyArray ; i += 8)
  {
    StatusCode |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Addr + i, *(uint64_t *)(MyArray + i));
  }

  StatusCode |= HAL_FLASH_Lock();

  return (StatusCode);
}

/*!
 * \brief challenge inside a Bytes Array
 *
 * \param in : *handle_se : Handle for STSAFE-Axx
 * \param in : size      : size of challenge to be generated
 * \param in : out_Data  : challenge
 * \return OK if no error
 */

uint8_t GenerateUnsignedChallenge(void *handle, uint32_t size, uint8_t *buf)
{
  uint8_t StatusCode = 0;

  STSAFEA_DYNAMIC_ALLOCATE_LV_BUFFER_VAR(StSafeA_LVBuffer_t, K, size + 2);

  if (buf)
  {

    STS_CHK(StatusCode, StSafeA_GenerateRandom(handle,
                                               STSAFEA_EPHEMERAL_RND,
                                               size / sizeof(uint8_t),
                                               &K,
                                               STSAFEA_MAC_NONE
                                              ));
    if (!StatusCode)
    {
      memcpy(buf, K.Data, size * sizeof(uint8_t));
    }
  }

  return (StatusCode);
}
