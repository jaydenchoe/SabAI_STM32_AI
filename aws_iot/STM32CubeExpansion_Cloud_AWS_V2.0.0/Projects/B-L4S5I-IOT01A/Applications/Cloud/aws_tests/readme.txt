/**
  @page FreeRTOS aws_tests application

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS aws_tests application.
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

The aws_tests application of X-CUBE-AWS is a port to the B-L4S5I-IOT01A STM32 Discovery board of the
FreeRTOS Qualification test application. 
 
It is intended for being customized, built, launched and verified by the IoT Device Tester (IDT) Amazon automatic test tool .
It is not expected to be used by end users, unless they want to validate their own port to another board.

__To get started with X-CUBE-AWS v2.x, please begin with the aws_demos application.__

The following test groups are supported:
  testrunnerFULL_CRYPTO_ENABLED
  testrunnerFULL_HTTPS_CLIENT_ENABLED
  testrunnerFULL_MEMORYLEAK_ENABLED
  testrunnerFULL_MQTT_AGENT_ENABLED
  testrunnerFULL_MQTT_STRESS_TEST_ENABLED
  testrunnerFULL_MQTTv4_ENABLED
  testrunnerFULL_MQTT_AGENT_ENABLED
  testrunnerFULL_MQTT_STRESS_TEST_ENABLED
  testrunnerFULL_MQTTv4_ENABLED
  testrunnerFULL_OTA_AGENT_ENABLED   /* Disabled by default because it fails if run after TCP tests. To be run separately. */
  testrunnerFULL_OTA_PAL_ENABLED
  testrunnerFULL_PKCS11_ENABLED
  testrunnerFULL_OTA_PAL_ENABLED
  testrunnerFULL_PKCS11_ENABLED
  testrunnerUTIL_PLATFORM_CLOCK_ENABLED
  testrunnerUTIL_PLATFORM_THREADS_ENABLED
  testrunnerFULL_POSIX_ENABLED
  testrunnerFULL_SERIALIZER_ENABLED
  testrunnerFULL_SHADOW_ENABLED
  testrunnerFULL_SHADOWv4_ENABLED
  testrunnerFULL_TASKPOOL_ENABLED
  testrunnerFULL_TCP_ENABLED
  testrunnerFULL_TLS_ENABLED
  testrunnerFULL_WIFI_ENABLED

Important:
There are two ways to use aws_tests.

1) by manually
  - selecting the test groups to run in aws_test_runner_config.h
  - editing your device and network settings in Middlewares/Third_Party/amazon-freertos/tests/include/aws_clientcredentials.h
  - setting a TCP and TLS echo server in Middlewares/Third_Party/amazon-freertos/tests/include/aws_tests_tcp.h, with its root CA certificate

2) trough the IDT.
  - However, IDT has some expectations regarding the source tree organization that are not satisfied by the usual X-Cube tree.
    Basically, IDT expects to find X-CUBE-AWS installed in amazon-freertos/vendors/st/, while X-CUBE-AWS installs amazon-freertos in Middlewares/Third_Party/
  - If you ever choose to move the folders around and try IDT, note that like with aws_demos, the SecureSocket abstraction of FreeRTOS relies on the STSAFE-A110
    secure element soldered on the board to authentify the device towards the MQTT endpoint during the TLS handshake.
    IDT can therefore not generate a device certificate on-the-fly, and must be provided with the secure element public key in its device.json configuration file.
    See the IDT documentation for more information.
    https://docs.aws.amazon.com/freertos/latest/userguide/qual-steps.html


@par Directory Contents

---> in .
config_files/*                          FreeRTOS configuration files
  aws_test_runner_config.h              Selection ot the test groups to embed in aws_tests.

EWARM                                   IAR build
  PostBuild                             Post-build output binaries (not delivered in the X-CUBE-AWS package)
    B-L4S5I-IOT01A_aws_tests.sfb           User application update file.
                                        Intended for OTA.
    SBSFU_B-L4S5I-IOT01A_aws_tests.bin     Packed raw binary of the bootloader and the user application.
                                        Intended for direct programming.
    SBSFU_B-L4S5I-IOT01A_aws_tests.elf     Packed ELF of the bootloader and the user application.
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
tests/include/aws_clientcredential.h    Definition of the WiFi credentials, of the device name, of the AWS IoT Core endpoint address.
tests/include/aws_tests_tcp.h           Definition of the TCP and TLS echo servers, and its root CA certificate.

---> in ../../BootLoader_STSAFE
2_Images_SBSFU/*                        Bootloader binary, and security settings (in SBSFU/App/app_sfu.h)
2_Images_SECoreBin/*                    Post-build scripts for the user application.
Linker_Common/*                         Image memory mapping definitions for the user application.
STSAFE_Provisioning/*                   Provisioning application for pairing the STSAFE Secure Element with the MCU bootloader.


@par Hardware and Software environment

Same as aws_demos.


@par How to use it?

All the preliminary steps of the aws_demos readme apply.

- Set the credentials in the aws_tests configuration file
  Middlewares/Third_Party/amazon-freertos/tests/include/aws_clientcredentials.h
  - clientcredentialMQTT_BROKER_ENDPOINT  is the endpoint address retrieved just above.
  - clientcredentialIOT_THING_NAME        is my_thing_name
  - clientcredentialWIFI_SSID             is the name of your WLAN
  - clientcredentialWIFI_PASSWORD         is the password of your WLAN
  - clientcredentialWIFI_SECURITY         is the security mode of your WLAN

- Choose the test groups to run in aws_test_runner_config.h

- Build the application, flash it, and launch it.

For a description of the aws_test cases, please look at the FreeRTOS porting documentation, or better, directly to the code.



@par Troubleshooting

- All the troubleshooting hints of the aws_demos readme apply.
- One of the test cases of the SecureSockets test group, AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasks (enabled by testrunnerFULL_TCP_ENABLED) is known to occasionnally fail.
  When it fails, it makes the next socket connections unstable. That is why the line "RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasks )"
  is commented out in Middlewares/Third_Party/amazon-freertos/libraries/abstractions/secure_sockets/test/iot_test_tcp.c



COPYRIGHT STMicroelectronics
