/**
  @page STSAFE_Provisionning project

  @verbatim
  ******************** (C) COPYRIGHT 2020 STMicroelectronics *******************
  * @file    readme.txt
  * @brief   Provisionning project
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

@endverbatim

@par Application Description

  This application is used to personalize secure element STSAFE-A for Secure Boot & Secure Firmware Update application (SBSFU).

  It is based on the following middlewares:

  - STSAFE_A1xx Middleware
  - MbedTLS

  Project need to be compiled to generate the binary file to be loaded inside STM32 that allow to personalize STSAFE-A chip.
  Once run this application is doing several tasks :

  1. Establish communication between STM32 and STSAFE-A
  2. Check if Pairing keys (Host_MAC_Key and Host_Cipher_Key) are set inside STM32 & STSAFE-A else it set theses keys
  3. Write in clear Host_MAC_Key and Host_Cipher_Key and executable code inside 510 & 511 page of STM32 flash
  4. Clear STSAFE-A100 zone 2 & 3
  5. Write certificate CERT_CA_FW inside STSAFE-A zone 2
  6. Write certificate CERT_CA_OEM_FW inside STSAFE-A zone 3

  Purpose of provisionning tools is also to provide to SECoreBin project values of pairing keys (Host_MAC_Key and Host_Cipher_Key).
  Theses keys can be retrieved using script provided called "Get_Pairing_Keys.exe & Get_Pairing_Keys.sh" located inside project
  directory Bin.

  For more details, refer to UM2262 "Getting started with SBSFU - software expansion for STM32Cube"
  available from the STMicroelectronics microcontroller website www.st.com.
  Note: The information available for STSAFE-A100 is also valid for STSAFE-A110.

@par Directory contents

  - STSAFE_Provisioning/Src/main.c                                      Main program body
  - STSAFE_Provisioning/Src/flash_if.c                                  This file provides all the memory related operation functions
  - STSAFE_Provisioning/Src/stm32l4xx_hal_msp.c                         provides code for the MSP Initialization
  - STSAFE_Provisioning/Src/stm32l4xx_it.c                              Interrupt Service Routines
  - STSAFE_Provisioning/Src/system_stm32l4xx.c                          CMSIS Cortex-M4 Device Peripheral Access Layer System Source File
  - STSAFE_Provisioning/Src/HAL_STSAFE-Axx.c                           Secure storage API external functions
  - STSAFE_Provisioning/Src/HAL_STSAFE-Axx_INTERNAL.c                  Secure storage API internal functions
  - STSAFE_Provisioning/Src/stsafea_crypto_mbedtls_interface.c          Crypto Interface file to support the crypto services required by the STSAFE-A middleware
  - STSAFE_Provisioning/Src/stsafea_service_interface.c                 Service Interface file to support the hardware services required by the STSAFE-A Middleware
  - STSAFE_Provisioning/Src/Pairing.c                                   PCROP functions

  - STSAFE_Provisioning/Inc/main.h                                      Headers for Main program body
  - STSAFE_Provisioning/Inc/stm32l4xx_hal_conf.h                        Headers for HAL configuration file
  - STSAFE_Provisioning/Inc/stm32l4xx_it.h                              Headers for Interrupt Service Routines headers
  - STSAFE_Provisioning/Inc/flash_if.h                                  Headers for flash memory related operation functions
  - STSAFE_Provisioning/Inc/Commonappli.h                               Global configuration file for the application
  - STSAFE_Provisioning/Inc/config_mbedtls.h                            Configuration file for MBEDTLS
  - STSAFE_Provisioning/Inc/HAL_STSAFE-Axx.h                           Header for Secure storage API internal functions
  - STSAFE_Provisioning/Inc/HAL_STSAFE-Axx_INTERNAL.h                  Header for Secure storage API internal functions
  - STSAFE_Provisioning/Inc/pairing.h                                   Headers for PCROP functions
  - STSAFE_Provisioning/Inc/stsafea_conf.h                              STSAFE-A1xx Middleware configuration file
  - STSAFE_Provisioning/Inc/stsafea_interface_conf.h                    STSAFE-A1xx Middleware interface configuration

@par Hardware and Software environment

  - This example runs on STM32L4S5xx devices
  - This example has been tested with B-L4S5I-IOT01A board and can be
    easily tailored to any other supported device and development board.
  - An up-to-date version of ST-LINK firmware (V2.J29 or later) is required. Upgrading ST-LINK firmware
    is a feature provided by STM32Cube programmer available on www.st.com.
  - Microsoft Windows has a limitation whereby paths to files and directories cannot
    be longer than 256 characters. Paths to files exceeding that limits cause tools (e.g. compilers,
    shell scripts) to fail reading from or writing to such files.
    As a workaround, it is advised to use the subst.exe command from within a command prompt to set
    up a local drive out of an existing directory on the hard drive, such as:
    C:\> subst X: <PATH_TO_CUBEFW>\Firmware

@par IDE prebuild script

  None

@par How to use it ?

  A) Installation

  In order to generate Binary file which does provisionning, you must do the following :

  1. Open your preferred toolchain
  2. Rebuild the project
  3. Connect the B-L4S5I-IOT01A board to your PC
  4. Flash binary Provisioning.bin using your IDE or STM32 STLINK Utility
  5. Open a terminal(exemple TERATERM) and connect it to STM32 virtual com Port (Speed 115200, Data 8 bits, Parity none, Stop bit 1 bit, Flow control none)
  6. Push reset Button
  7. After provisionning ran successfully you should see under terminal :

  -------------------------------------------------------------------------------
  Start provisionning of STSAFE

  Force STSAFE-A110 Perso

  Launching STSAFE-A110 Perso
  Check if Pairing Host keys available
  Perso OK

  Erase Data : OK

  Now Store Certificate STM_POC_SBSFU_ROOT_TEST_CA_00 inside STSAFE
  Certificate STM_POC_SBSFU_ROOT_TEST_CA_00 successfully written inside STSAFE
  Now Store Data using HAL_Store_Data

  Now Store Certificate  STM_POC_SBSFU_OEM_TEST_CA_00 inside STSAFE
  Certificate STM_POC_SBSFU_OEM_TEST_CA_00 successfully written inside STSAFE

  End provisionning of STSAFE

  B) Tuning of Host_MAC_Key and Host_Cipher_Key though Secure storage API (HAL_STSAFE-Axx.h)

  Provisionning tools can generate Host_MAC_Key and Host_Cipher_Key following 3 differents methods as defined inside
  Init_Perso function. This means with with perso_type=0 theses keys are known and fixed internally to the API secure storage.
  This is default method once using provisionning project for developement and testing purposes. With perso_type=1 keys are random,
  this method is prefered once switcting to production, because keys are differentiated per chip. This is the best method
  in terms of security implemntation. For perso_type=2 it's possible to provide
  as parameter Host_MAC_Key and Host_Cipher_Key values through a buffer.

  *  perso_type=0: use default keys and meanwhile set buf = NULL
  *  perso_type=1: use random generated keys and meanwhile set buf = NULL
  *  perso_type=2: use the keys customers prefer and meanwhile set buf = preferred key.

  Exemple of personalization mode random pairing keys :

  Modification to be done inside main.c :

  Mode perso_type=0 => Init_Perso(&stsafea_handle,DEFAULT_KEYS,NULL)
  Mode perso_type=1 => Init_Perso(&stsafea_handle,RANDOM_KEYS,NULL)
  Mode perso_type=2 => Init_Perso(&stsafea_handle,USER_KEYS,buffer)

  * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
  */
