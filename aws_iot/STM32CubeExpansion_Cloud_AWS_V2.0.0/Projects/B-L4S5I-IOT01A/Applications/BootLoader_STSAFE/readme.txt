/**
  @page Secure Boot and Secure Firmware Update Demo Application

  @verbatim
  ******************** (C) COPYRIGHT 2020 STMicroelectronics *******************
  * @file    readme.txt
  * @brief   This application shows Secure Boot and Secure Firmware Update example.
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description

The X-CUBE-SBSFU Secure Boot and Secure Firmware Update solution allows the update
of the STM32 microcontroller built-in program with new firmware versions, adding
new features and correcting potential issues. The update process is performed in
a secure way to prevent unauthorized updates and access to confidential on-device
data such as secret code and firmware encryption key.

The Secure Boot (Root of Trust services) is an immutable code, always executed
after a system reset, that checks STM32 static protections, activates STM32
runtime protections and then verifies the authenticity and integrity of user
application code before every execution in order to ensure that invalid or
malicious code cannot be run.

The Secure Firmware Update application receives the firmware image via a UART
interface with the Ymodem protocol, checks its authenticity, and checks the
integrity of the code before installing it. The firmware update is done on the
complete firmware image, or only on a portion of the firmware image.

The secure key management services provide cryptographic services to the user
application through the PKCS # APIs (KEY IDbased APIs) that are executed inside
a protected and isolated environment. User application keys are stored in the
protected and isolated environment for their secured update: authenticity check,
data decryption and data integrity check.

STSAFE-A110 is a tamper-resistant secure element (HW Common Criteria EAL5+
certified) used to host X509 certificates and keys, and perform verifications
that are used for firmware image authentication during Secure Boot and Secure
Firmware Update procedures.

This examples is provided with for dual firmware image configurations in order
to ensure safe image installation and enable over-the-air firmware update
capability commonly used in IOT devices.

@par Directory contents

   - 2_Images_KMS_Blob   Project to create blobs for KMS, you do not need to compile this one.
                         This is present only to provide SECoreBin with a set of KMS keys.
   - STSAFE_Provisioning Personalize secure element STSAFE-A for SBSFU application
   - 2_Images_SECoreBin  Generate secure engine binary including all the "trusted" code
   - 2_Images_SBSFU      Secure boot and secure firmware update application
   - Linker_Common       Linker files definition shared between SECoreBin, SBSFU, AWS applications


@par How to use it ?

You need to follow a strict compilation order :

1. Compile SECoreBin application
This step is needed to create the Secure Engine core binary including all the "trusted"
code and keys mapped inside the protected environment. The binary is linked with the
SBSFU code in step 2.

2. Compile SBSFU application
This step compiles the SBSFU source code implementing the state machine and
configuring the protections. In addition, it links the code with the SECore binary
generated at step 1 in order to generate a single SBSFU binary including the SE
trusted code.

3. Compile STSAFE_Provisioning
This step creates the application used to personalize the secure element STSAFE-A
for Secure Boot & Secure Firmware Update application.
This is needed to provision the keys and certificates required by SBSFU.

@par Specificities regarding the AWS application firmware image state

These versions of the X-CUBE-SBSFU and X-CUBE-AWS implement a specific management of the AWS application
firmware image state. This management is dedicated to the AWS Over The Air update mechanism.
Hence, when triggered by an OTA update the installation of a new AWS firmware image implies a self test phase.
After these self tests the firmware image is validated (if they are successful).
If you update the firmware image by other means then the image state will not be updated by the AWS application.
This means that at the next reset your newly installed AWS application might be deemed as invalid.
If a previous firmware image exists, then it will be restored. If no previous image exists or if this previous image
is invalid then you will face a boot failure and end up in local download state to let you install a new firmware image.
The local loader provided by X-CUBE-SBSFU avoids this situation by setting the firmware image state to "valid" automatically.
To sum up, the firmware image state management is tightly coupled with the AWS Over The Air Firmware Update mechanism
so please keep it in mind when considering other ways of updating the AWS application Firmware Image.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
