################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/se_key.s \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/startup_stm32l4S5xx.s 

C_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/data_init.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/syscalls.c 

OBJS += \
./Application/STM32CubeIDE/data_init.o \
./Application/STM32CubeIDE/se_key.o \
./Application/STM32CubeIDE/startup_stm32l4S5xx.o \
./Application/STM32CubeIDE/syscalls.o 

S_DEPS += \
./Application/STM32CubeIDE/se_key.d \
./Application/STM32CubeIDE/startup_stm32l4S5xx.d 

C_DEPS += \
./Application/STM32CubeIDE/data_init.d \
./Application/STM32CubeIDE/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
Application/STM32CubeIDE/data_init.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/data_init.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Application/STM32CubeIDE/data_init.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Application/STM32CubeIDE/se_key.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/se_key.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"Application/STM32CubeIDE/se_key.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@" "$<"
Application/STM32CubeIDE/startup_stm32l4S5xx.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/startup_stm32l4S5xx.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"Application/STM32CubeIDE/startup_stm32l4S5xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@" "$<"
Application/STM32CubeIDE/syscalls.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/syscalls.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Application/STM32CubeIDE/syscalls.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"

