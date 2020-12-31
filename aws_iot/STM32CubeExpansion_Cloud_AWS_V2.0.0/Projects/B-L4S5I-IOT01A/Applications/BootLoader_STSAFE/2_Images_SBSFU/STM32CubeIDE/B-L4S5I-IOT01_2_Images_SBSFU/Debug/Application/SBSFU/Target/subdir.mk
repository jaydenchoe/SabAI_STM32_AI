################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/SBSFU/Target/sfu_low_level.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/SBSFU/Target/sfu_low_level_flash.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/SBSFU/Target/sfu_low_level_security.c 

OBJS += \
./Application/SBSFU/Target/sfu_low_level.o \
./Application/SBSFU/Target/sfu_low_level_flash.o \
./Application/SBSFU/Target/sfu_low_level_security.o 

C_DEPS += \
./Application/SBSFU/Target/sfu_low_level.d \
./Application/SBSFU/Target/sfu_low_level_flash.d \
./Application/SBSFU/Target/sfu_low_level_security.d 


# Each subdirectory must supply rules for building sources it contributes
Application/SBSFU/Target/sfu_low_level.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/SBSFU/Target/sfu_low_level.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED '-DMBEDTLS_CONFIG_FILE=<sfu_scheme_x509_config_mbedtls.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../Core/Inc -I../../../SBSFU/App -I../../../SBSFU/Target -I../../../../2_Images_SECoreBin/Inc -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/tKMS -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/Third_Party/MbedTLS/include -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../../../Linker_Common/STM32CubeIDE -Os -ffunction-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Application/SBSFU/Target/sfu_low_level.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/SBSFU/Target/sfu_low_level_flash.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/SBSFU/Target/sfu_low_level_flash.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED '-DMBEDTLS_CONFIG_FILE=<sfu_scheme_x509_config_mbedtls.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../Core/Inc -I../../../SBSFU/App -I../../../SBSFU/Target -I../../../../2_Images_SECoreBin/Inc -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/tKMS -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/Third_Party/MbedTLS/include -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../../../Linker_Common/STM32CubeIDE -Os -ffunction-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Application/SBSFU/Target/sfu_low_level_flash.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/SBSFU/Target/sfu_low_level_security.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/SBSFU/Target/sfu_low_level_security.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED '-DMBEDTLS_CONFIG_FILE=<sfu_scheme_x509_config_mbedtls.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../Core/Inc -I../../../SBSFU/App -I../../../SBSFU/Target -I../../../../2_Images_SECoreBin/Inc -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/tKMS -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/Third_Party/MbedTLS/include -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../../../Linker_Common/STM32CubeIDE -Os -ffunction-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Application/SBSFU/Target/sfu_low_level_security.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

