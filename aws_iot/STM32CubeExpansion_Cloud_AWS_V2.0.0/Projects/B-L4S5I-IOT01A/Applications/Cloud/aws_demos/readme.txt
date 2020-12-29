/**
  @page FreeRTOS aws_demos application

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS aws_demos application.
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics International N.V. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description

Foreword:
X-CUBE-AWS v2.x follows the X-CUBE-AWS v1.x version.
It connects to the same AWS IoT Core Services, it can inter-operate with the same cloud services
as used with the X-CUBE-AWS v1.x.
But it is a different thing: It is based on a complete SDK (FreeRTOS) which coexists with the STM32
Cube environment, not on a single middleware library anymore.


FreeRTOS is an operating system for microcontrollers. The services it provides widely exceed
the scope of the FreeRTOS kernel.
For instance, it includes libraries for cryptography, IP-based networking, Bluetooth, etc.

The aws_demos application of X-CUBE-AWS is both:
 * a port to the B-L4S5I-IOT01A STM32 Discovery board of the FreeRTOS over-the-air firmware update (OTA) demo
   provided by Amazon;
 * a demonstration of the usage of the on-board STSAFE-A110 Secure Element for hardening the security of:
   - the MCU boot process,
   - the TLS device authentication towards the AWS IoT Core server,
   - the verification of the OTA firmware image integrity and authenticity.


The base demo application is documented in the FreeRTOS user guide.
The most relevant sections are:
 * Application
   https://docs.aws.amazon.com/freertos/latest/userguide/freertos-next-steps.html
   https://docs.aws.amazon.com/freertos/latest/userguide/ota-demo.html

 * OTA principle, pre-requisites, tutorial
   https://docs.aws.amazon.com/freertos/latest/userguide/freertos-ota-dev.html

 * IoT Core account and device credentials
   https://docs.aws.amazon.com/freertos/latest/userguide/freertos-getting-started.html

 * Multi-account registration
   https://aws.amazon.com/about-aws/whats-new/2020/04/simplify-iot-device-registration-and-easily-move-devices-between-aws-accounts-with-aws-iot-core-multi-account-registration
   https://docs.aws.amazon.com/iot/latest/developerguide/x509-client-certs.html
   https://docs.aws.amazon.com/iot/latest/developerguide/manual-cert-registration.html#manual-cert-registration-noca-cli


The usage of the STSAFE Secure Element for improving the security level of the device
is inherited from the X-CUBE-SBSFU v2.2 STM32 Cube Expansion Package, with application-specific
modifications.

Those modifications consist in offloading some of the cryptographic services from the Secure Engine
to the STSAFE.
Important consequence:  You have to pair once the STSAFE with the bootloader by programming and running
the STSAFE provisioning application before programming any bootloader-enabled user application.
The binary is available in the package: Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/STSAFE_Provisioning/Binary/Provisioning.bin

The pre-built bootloader binaries delivered in the X-CUBE-AWS package do not activate MCU-specific protections
by default. See bootloader_readme.txt.

For more details, please refer to the User Manual UM2178 - Getting started with X-CUBE-AWS Amazon web services IoT software expansion for STM32Cube, v3.0 or later.


@par Directory Contents

---> in .
config_files/*                          FreeRTOS configuration files
  aws_application_version.h             Application version. To be modified for testing the OTA firmware update flow.

EWARM                                   IAR build
  PostBuild                             Post-build output binaries (not delivered in the X-CUBE-AWS package)
    B-L4S5I-IOT01A_aws_demos.sfb           User application update file.
                                        Intended for OTA.
    SBSFU_B-L4S5I-IOT01A_aws_demos.bin     Packed raw binary of the bootloader and the user application.
                                        Intended for direct programming.
    SBSFU_B-L4S5I-IOT01A_aws_demos.elf     Packed ELF of the bootloader and the user application.
                                        Intended for debugging the user application with IAR.
  Project.custom_argvars                Definition of the location of the bootloader and of the FreeRTOS directories.
                                        Custom argvars are used instead of static paths in Project.ewp in order to
                                        ease the transposition of the STM32Cube project when builing in the FreeRTOS source tree
                                        as published on GitHub.
  Project.ewp                           Build project
  Project.eww                           Build workspace
  startup_stm32l4s5.s                   Interrupt vector table
  stm32l4s5xx_flash_for_sfu.icf         Linker file compatible with the SBSFU bootloader


Inc
  es_wifi_conf.h                        Es-WiFi driver configuration
  es_wifi_io.h                          Es-WiFi physical physical interface
  flash.h                               System flash programming interface
  main.h                                Board specific definitions
  stm32l4xx_hal_conf.h                  HAL configuration
  stm32l4xx_it.h                        Interrupt handlers definition
  watchdogkicker.h                      Configuration of the OTA update watchdog kicker task

Src
  es_wifi_io.c                          Es-WiFi physical interface
  flash_l4.c                            System flash programming interface
  main.c                                User application entry point, HAL and RTOS init, device provisionning, interrupt handler callbacks.
  ports                                 FreeRTOS porting abstraction layer implementation
    aws_ota_pal.c
    iot_pkcs11_st.c
    iot_secure_sockets.c
    iot_wifi.c
  stm32l4xx_hal_msp.c                   MCU Support Package
  stm32l4xx_hal_timebase_tim.c          HAL and RTOS ticks
  stm32l4xx_it.c                        Interrupt handlers
  system_stm32l4xx.c                    Low level system init
  watchdogkicker.c                      OTA update watchdog kicker task

STM32CubeIDE
  .cproject                             Compiler project  
  .project                              Build project
  Application
    Startup
      startup_stm32l4s5vitx.s           Vector table
    User
      syscalls.c                        Low level libc port
      sysmem.c                          Low level allocator
  PostBuild                             Post-build output binaries (not delivered in the X-CUBE-AWS package)
    B-L4S5I-IOT01A_aws_tests.sfb           User application update file.
                                        Intended for OTA.
    SBSFU_B-L4S5I-IOT01A_aws_tests.elf     Packed ELF of the bootloader and the user application.
                                        Intended for debugging the user application with STM32CubeIDE.
  STM32L4S5VITX_FLASH_sbsfu.ld          Linker file

bootloader_readme.txt                   Readme of the bootloader application set
readme.txt



---> in Middlewares/Third_Party/amazon-freertos
demos/include/aws_clientcredential.h    Definition of the WiFi credentials, of the device name, of the AWS IoT Core endpoint address.


---> in ../../BootLoader_STSAFE
2_Images_SBSFU/*                        Bootloader binary, and security settings (in SBSFU/App/app_sfu.h)
2_Images_SECoreBin/*                    Post-build scripts for the user application.
Linker_Common/*                         Image memory mapping definitions for the user application.
STSAFE_Provisioning/*                   Provisioning application for pairing the STSAFE Secure Element with the MCU bootloader.



@par Hardware and Software environment

- B-L4S5I-IOT01A board.

- WiFi access point.
      * With a transparent Internet connectivity: No proxy, no firewall blocking the outgoing traffic.
      * Running a DHCP server delivering the IP and DNS configuration to the board.

- Amazon AWS account.

- AWS CLI with web access (possibly through an HTTP proxy)


@par How to use it?

- WARNING: Before opening the project with any toolchain be sure your folder
  installation path is not too in-depth since the toolchain may report errors
  after building.

- If not already done, pair the STSAFE with the bootloader by programming and running the STSAFE provisioning application on the STM32 board MCU.
  The raw binary is available in the package: Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/STSAFE_Provisioning/Binary/Provisioning.bin
  You can proceed in the same way as written two paragraphs below.

- Open the IAR IDE and compile the project (see the release note for detailed information about the version).
  Alternatively you can use CubeIDE for STM32 (see the release note for detailed information about the version).

- Program the firmware on the STM32 board: If you generated a raw binary file,
  you can copy (or drag and drop) it from
  Projects\B-L4S5I-IOT01A\Applications\Cloud\aws_demos\<toolchain>\PostBuild\SBSFU_B-L4S5I-IOT01A_aws_demos.bin
  to the USB mass storage location created when you plug the STM32 board to your PC.
  If the host is a Linux PC, the STM32 device can be found in the /media folder
  with the name e.g. "DIS_L4IOT". For example, if the created mass
  storage location is "/media/DIS_L4IOT", then the command to program the board
  with a binary file named "my_firmware.bin" is simply: cp my_firmware.bin
  /media/DIS_L4IOT.

  Alternatively, you can program the STM32 board directly through one of the
  supported development toolchains, the STM32 ST-Link Utility, or STM32CubeProgrammer.

- Configure the board console (to be done only once):
  - When the board is connected to a PC with USB (ST-LINK USB port),
    open a serial terminal emulator, find the board's COM port and configure it with:
    - 8N1, 115200 bauds, no HW flow control
    - set the receive line endings to LF.

- Reset the board (thanks to the IDE, the STM32 ST-Link Utility, STM32CubeProgrammer, or the black reset button)
  and copy the pre-provisioned x509 certificate PEM string which gets printed to the console to a e.g. my_dumped_cert.pem file.
  Remark: This certificate is immutable, unique and is and stored in the on-board STSAFE chip.
          It will be used for authenticating the device each time it connects to AWS.

- Create your AWS account, set the required access rights and policies for the multi-account registration mechanism.
   - Create an IoT thing policy. We call it "my_iot_policy" in the next steps.
    Remark: If you have used AWS IoT Core in the past (for instance with X-CUBE-AWS v1.x),
            you can reuse your existing thing policy.

- Setup the AWS CLI
  - In addition to the [default] section, set the [profile adminuser] section in ~/.aws/credentials, with your
     region
     aws_access_key_id
     and aws_secret_access_key

     If your personal AWS account is given the required access rigths, you may simply copy the [default] settings
     to the [profile adminuser] section.

- Register your device

  - register the x509 certificate copied from the board console,
    > aws iot register-certificate-without-ca --certificate-pem file://my_dumped_cert.pem --status ACTIVE

    -> Note the contents of the certificateArn field which is returned by the command. We call it "my_device_cert_arn" in the next steps.

  - create your "thing"
    > aws iot create-thing --thing-name my_thing_name

  - empower your "thing" by granting its certificate with the access rights defined in the thing policy
    > aws iot attach-policy --policy-name "my_iot_policy" --target "my_device_cert_arn"

    > aws iot attach-thing-principal --thing-name my_thing_name --principal my_device_cert_arn


  - retrieve the address of your IoT Core endpoint
    > aws iot describe-endpoint --endpoint-type iot:Data-ATS


- Set the credentials in the aws_demos configuration file
  Middlewares/Third_Party/amazon-freertos/demos/include/aws_clientcredentials.h
  - clientcredentialMQTT_BROKER_ENDPOINT  is the endpoint address retrieved just above.
  - clientcredentialIOT_THING_NAME        is my_thing_name
  - clientcredentialWIFI_SSID             is the name of your WLAN
  - clientcredentialWIFI_PASSWORD         is the password of your WLAN
  - clientcredentialWIFI_SECURITY         is the security mode of your WLAN

- Re-build the application with the new configuration, flash it, and launch it.

  aws_demos
  - connects to the endpoint over TCP/IP and WiFi,
  - authentifies itself thanks to TLS and its pre-provisioned device certificate,
  - enters the MQTT wait loop, pending on the reception of an OTA firmware update job.


- In order to go one step further and actually test the OTA update, further configuration is required:

  - Create a code signing certificate and profile on the AWS console (once for all)
    See https://docs.aws.amazon.com/freertos/latest/userguide/ota-code-sign-cert-win.html
  - Copy the code signing certificate string to the pcClientCertificatePem[] static array in the OTA PAL (aws_ota_pal.c).
    Important: This code signing certificate is NOT the device certificate (my_device_cert_arn) that was used for the device registration.
  - Build the application, flash and reset the board.

  - Prepare the application updade file and upload it to AWS
    - Increment the application version in aws_application_version.h - this is mandatory, otherwise the update will be rejected by the demo self-test.
    - Build the application, but do not flash it to the board.
    - Upload the resulting <toolchain_name>/PostBuild/B-L4S5I-IOT01A_aws_demos.sfb file to a versionned Amazon S3 bucket which belongs to your AWS account.
  
  - Create a FreeRTOS OTA update job from the AWS console (IoT Core / Manage / Jobs / Create) and select:
    - the 'thing' to update: my_thing_name
    - the MQTT protocol for image transfer, (HTTP is not supported)
    - "Sign a new firmware for me",
    - your code signing profile,
    - your .sfb firmware file, referenced from the S3 bucket where you have uploaded it,
    - any non-empty destination pathname (e.g. firmware.bin)
    - the OtaUpdateServiceRole,
    and finally choose a unique job ID.

    The application will automatically
    - receive and handle the job request,
    - download the .sfb file and store it to the Slot0 region of the system flash,
    - verify the integrity and the authenticity of the .sfb file thanks to the file signature attached to the job description
      and to the AWS code signing certificate provisioned in the OTA PAL.
      If the file signature verification fails (you will see on the board console: "[prvPAL_CloseFile] ERROR - Failed to pass sig-sha256-ecdsa signature verification"), 
      the OTA update job is aborted and reported FAILED to the AWS OTA update sevice.
    - reset the MCU and let the secure bootloader detect the new .sfb in the Slot0, verify the integrity and the
      authenticity of the new user application thanks to the secure bootloader certificate, install it to Slot1, and reset the MCU again,
    - launch the new application version, which will perform the self-test and inform the OTA update service of the update success.
      In case of self-test error, the user application update is rejected and the previous application version is restored.
      If the new application version cannot complete the self-test within about 90 seconds, a timer expiration resets the MCU,
      the secure bootloader roll-backs to the previous application version, and the OTA update job is reported FAILED.

@par Troubleshooting

- When using the AWS CLI from an MSDOS shell, the command arguments strings (e.g. file://) must be enclosed by quotes.

- STM32CubeIDE: Missing post-build dependencies
  - Several python packages may have to be installed explicitly. See Middlewares/ST/STM32_Secure_Engine/Utilities/KeysAndImages/readme.txt
  - STM32CubeProgrammer must be available in your PATH.

- STM32CubeIDE debug: when creating an STM32CubeIDE debug configuration, the GDB
  server option for Reset Behavior must be set to "None" instead of "Debug under reset".
  If "Debug under Reset" is set, the Bootloader will start and then it will be
  interrupted and re-started by the reset.

- Build or post-build failure (bootloader + user application)
  - The bootloader switches to the local download mode:
    " ...
            No valid FW found in the active slot nor new encrypted FW found in the UserApp download area
            Waiting for the local download to start...
      = [SBOOT] STATE: DOWNLOAD NEW USER FIRMWARE
            File> Transfer> YMODEM> Send .......
    "
    -> Verify the build log, the post-build log (<toolchain_name>/output.txt)

- After flashing with STM32CubeProgrammer, the application does not start and the HW reset button (black) on the board is ignored.
    -> Power down/up the board by unplugging the ST-Link USB cable.
    -> Alternatively, use the --start command of the STM32CubeProgrammer CLI: "STM32_Programmer_CLI.exe -c port=SWD --start"

- Wi-Fi channels 12, 13 and 14 are not enabled by default.
    ->  If you are outside the USA, replace the default region code (US) in the es_wifi_conf.h file of the user application.

- Wi-Fi connection failure
   - The board console output is blocked on
   "= [SBOOT] STATE: EXECUTE USER FIRMWARE0 524 [Tmr Svc] WiFi module initialized."
    -> Verify your WLAN settings in Middlewares/Third_Party/amazon-freertos/demos/include/aws_clientcredentials.h

COPYRIGHT STMicroelectronics
