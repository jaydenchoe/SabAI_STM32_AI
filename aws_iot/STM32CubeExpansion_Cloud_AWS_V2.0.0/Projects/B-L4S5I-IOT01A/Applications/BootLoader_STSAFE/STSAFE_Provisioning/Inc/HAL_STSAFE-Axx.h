/**
  ******************************************************************************
  * @file    HAL_STSAFE-Axx.h
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

#ifndef HAL_STSAFEA100_H
#define HAL_STSAFEA100_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Exported functions ------------------------------------------------------- */

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

uint8_t Init_HAL(void *handle_se);

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

uint8_t Init_Perso(void *handle_se, uint8_t perso_type, uint8_t *buf);

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

uint8_t HAL_Store_Data_Zone(void *handle_se, uint8_t zone, uint16_t size, uint8_t *in_Data, uint16_t offset);

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

uint8_t HAL_Get_Data_Zone(void *handle_se, uint8_t zone, uint16_t size, uint8_t *buf, uint16_t offset);

/*!
 * \brief Erase datas in STSAFE-Axx zone
 *
 * This command set to 0x0 STSAFE-Axx zone
 *
 * Exemple : for size=5, it set first 5 bytes of zone to 0x0

 * \param in : handle_se : Handle for STSAFE-Axx
 * \param in : zone      : zone number
 * \param in : size      : size of data to be erased
 * \param in : in_Data   : offset
*/

uint8_t HAL_Erase_Data_Zone(void *handle_se, uint8_t zone, uint16_t size, uint16_t offset);

/*!
 * \brief Get Library version
 *
 * This command must be executed one time in the life of the product

 * \param out :  Library version
 *
 * \return 0 if no error else 1
 *
 */

void HAL_Version(char *string);

#ifdef __cplusplus
}

#endif /* __cplusplus */

#endif /* HAL_STSAFEA100_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

