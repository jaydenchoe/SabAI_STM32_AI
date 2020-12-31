################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/Core/Src/main.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/Core/Src/stm32l4xx_hal_msp.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/Core/Src/stm32l4xx_it.c 

OBJS += \
./Application/Core/main.o \
./Application/Core/stm32l4xx_hal_msp.o \
./Application/Core/stm32l4xx_it.o 

C_DEPS += \
./Application/Core/main.d \
./Application/Core/stm32l4xx_hal_msp.d \
./Application/Core/stm32l4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Core/main.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/Core/Src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED '-DMBEDTLS_CONFIG_FILE=<sfu_scheme_x509_config_mbedtls.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../Core/Inc -I../../../SBSFU/App -I../../../SBSFU/Target -I../../../../2_Images_SECoreBin/Inc -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/tKMS -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/Third_Party/MbedTLS/include -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../../../Linker_Common/STM32CubeIDE -Os -ffunction-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Application/Core/main.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/Core/stm32l4xx_hal_msp.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/Core/Src/stm32l4xx_hal_msp.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED '-DMBEDTLS_CONFIG_FILE=<sfu_scheme_x509_config_mbedtls.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../Core/Inc -I../../../SBSFU/App -I../../../SBSFU/Target -I../../../../2_Images_SECoreBin/Inc -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/tKMS -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/Third_Party/MbedTLS/include -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../../../Linker_Common/STM32CubeIDE -Os -ffunction-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Application/Core/stm32l4xx_hal_msp.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/Core/stm32l4xx_it.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SBSFU/Core/Src/stm32l4xx_it.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED '-DMBEDTLS_CONFIG_FILE=<sfu_scheme_x509_config_mbedtls.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../Core/Inc -I../../../SBSFU/App -I../../../SBSFU/Target -I../../../../2_Images_SECoreBin/Inc -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/tKMS -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/Third_Party/MbedTLS/include -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../../../Linker_Common/STM32CubeIDE -Os -ffunction-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Application/Core/stm32l4xx_it.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

