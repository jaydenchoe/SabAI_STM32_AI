################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_stack_smuggler_GNU.s 

C_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_bootinfo.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_callgate.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_crypto_common.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_exception.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_fwimg.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_startup.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_user_application.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_utils.c 

OBJS += \
./Middlewares/STM32_Secure_Engine/se_bootinfo.o \
./Middlewares/STM32_Secure_Engine/se_callgate.o \
./Middlewares/STM32_Secure_Engine/se_crypto_common.o \
./Middlewares/STM32_Secure_Engine/se_exception.o \
./Middlewares/STM32_Secure_Engine/se_fwimg.o \
./Middlewares/STM32_Secure_Engine/se_stack_smuggler_GNU.o \
./Middlewares/STM32_Secure_Engine/se_startup.o \
./Middlewares/STM32_Secure_Engine/se_user_application.o \
./Middlewares/STM32_Secure_Engine/se_utils.o 

S_DEPS += \
./Middlewares/STM32_Secure_Engine/se_stack_smuggler_GNU.d 

C_DEPS += \
./Middlewares/STM32_Secure_Engine/se_bootinfo.d \
./Middlewares/STM32_Secure_Engine/se_callgate.d \
./Middlewares/STM32_Secure_Engine/se_crypto_common.d \
./Middlewares/STM32_Secure_Engine/se_exception.d \
./Middlewares/STM32_Secure_Engine/se_fwimg.d \
./Middlewares/STM32_Secure_Engine/se_startup.d \
./Middlewares/STM32_Secure_Engine/se_user_application.d \
./Middlewares/STM32_Secure_Engine/se_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/STM32_Secure_Engine/se_bootinfo.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_bootinfo.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_bootinfo.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Middlewares/STM32_Secure_Engine/se_callgate.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_callgate.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_callgate.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Middlewares/STM32_Secure_Engine/se_crypto_common.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_crypto_common.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_crypto_common.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Middlewares/STM32_Secure_Engine/se_exception.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_exception.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_exception.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Middlewares/STM32_Secure_Engine/se_fwimg.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_fwimg.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_fwimg.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Middlewares/STM32_Secure_Engine/se_stack_smuggler_GNU.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_stack_smuggler_GNU.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_stack_smuggler_GNU.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@" "$<"
Middlewares/STM32_Secure_Engine/se_startup.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_startup.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_startup.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Middlewares/STM32_Secure_Engine/se_user_application.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_user_application.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_user_application.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"
Middlewares/STM32_Secure_Engine/se_utils.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STM32_Secure_Engine/Core/se_utils.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DKMS_ENABLED -DSTSAFE_A110 '-DMBEDTLS_CONFIG_FILE=<../Inc/mbed_crypto_config.h>' -DENABLE_IMAGE_STATE_HANDLING -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../2_Images_SBSFU/SBSFU/App -I../../../../../../../../Middlewares/ST/STM32_Cryptographic/Fw_Crypto/STM32L4/Inc -I../../../../../../../../Drivers/CMSIS/Include -I../../ -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../../Linker_Common/STM32CubeIDE -I../../../../../../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -Os -ffunction-sections -Wall -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"Middlewares/STM32_Secure_Engine/se_utils.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -o "$@"

