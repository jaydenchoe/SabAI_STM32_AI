
/**
  @page BootLoader support

  @verbatim
  ******************** (C) COPYRIGHT 2019 STMicroelectronics *******************
  * @file    bootloader_readme.txt
  * @brief   Boot-loader project.
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

This application is not a standalone application. The user application is concatenated to
a bootloader to create a single executable file. The bootloader and the
application are merged together when building the application. A specific post-build
phase (after link stage) is in charge of concatenating both executable files. The
generated executable file has the same name as the original application executable
file. Thanks to this approach, the executable file can be flashed, run and debugged
using the usual IDE command.

The bootloader is provided as a pre-build executable. The executable name depends on the
selected toolchain:
    IAR	    :	./2_Images_SBSFU/EWARM/<BOARDNAME>/Exe/Project.out
    Keil	: 	./2_Images_SBSFU/MDK-ARM/<BOARDNAME>_2_Images_SBSFU/SBSFU.axf
    SW4STM32:	./2_Images_SBSFU/SW4STM32/<BOARDNAME>_2_Images_SBSFU/Debug/SBSFU.elf

The bootloader enables the secure firmware update feature. An encrypted application
image can be downloaded and installed on the device. The bootloader is in charge of
decrypting, installing, and launching the user application. All these actions are 
performed in a secure environment, activating specific STM32 security hardware
mechanisms. The bootloader is based on the Secure Boot (SB) and Secure Firmware
Update (SFU) solution. These features are supported by the X-CUBE-SBSFU package.
The below folders are imported from the X-CUBE-SBSFU package version 2.1.0.Few
Modifications have been performed on the original SBSFU package to ease integration
with cloud package.

	-./2_Images_SBSFU,
	-./2_Images_SECoreBin
	-./Linker_Common


The Linker_Common folder contains the linker script files. The memory layout is
different from normal application. A specific range of the flash memory is
reserved for bootloader while others ranges, named "slot0" or "slot1" are
reserved for the application.

Rebuilding the bootloader is not required unless the memory mapping or the
bootloader configuration have changed.

	+ The configuration is specified in: ./2_Images_SBSFU/SBSFU/App/app_sfu.h

The current bootloader is configured as follows. The secure firmware image
is encrypted, but hardware protections are not enforced. For instance, the JTAG
link is on, so debugging remains possible:

	+ SFU_ENCRYPTED_IMAGE
	+ SFU_DEBUG_MODE
	+ SECBOOT_DISABLE_SECURITY_IPS

Please read the documentation of the X-CUBE-SBSFU package to get more information
about the different security settings.

Re-building the bootloader

1) Edit the configuration file as needed  ./2_Images_SBSFU/SBSFU/App/app_sfu.h

2) Build the projects in the following order (as each component depends on the previous one):
  - Applications/BootLoader/2_Images_SECoreBin
  - Applications/BootLoader/2_Images_SBSFU

3) Build the application
	For IAR tools chain, due to a dependency issue, touch a source file
	to ensure build process is run to the end and BootLoader modifications
	are taken into account.


While building the application, several files are generated in the "PostBuild"
folder. The file with extension "sfb" is the encrypted image of the user application.
This file is the one to be downloaded to the Flash memory, and then decrypted and
installed. The files with extensions "bin" and "elf" contain  the application
concatenated with the bootloader. In those files, the application is not encrypted
but is ready to be started by the bootloader.

Debug the application

After flashing the elf file (or bin), the bootloader part is executed and starts
the application. The execution breaks when reaching the "main" function of the user
application.

Known Issues
When using SW4STM32 debugger, the execution of the bootloader is skipped because
the debugger starts directly the application part. The workaround is to push the
reset button on the device board to force the correct boot sequence.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
