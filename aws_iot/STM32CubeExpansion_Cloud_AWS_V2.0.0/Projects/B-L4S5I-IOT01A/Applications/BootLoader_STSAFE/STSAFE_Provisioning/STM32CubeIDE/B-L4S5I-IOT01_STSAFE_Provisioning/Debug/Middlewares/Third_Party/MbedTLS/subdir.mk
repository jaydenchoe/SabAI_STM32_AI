################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/aes.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/cipher.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/cipher_wrap.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/cmac.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/platform.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/platform_util.c 

OBJS += \
./Middlewares/Third_Party/MbedTLS/aes.o \
./Middlewares/Third_Party/MbedTLS/cipher.o \
./Middlewares/Third_Party/MbedTLS/cipher_wrap.o \
./Middlewares/Third_Party/MbedTLS/cmac.o \
./Middlewares/Third_Party/MbedTLS/platform.o \
./Middlewares/Third_Party/MbedTLS/platform_util.o 

C_DEPS += \
./Middlewares/Third_Party/MbedTLS/aes.d \
./Middlewares/Third_Party/MbedTLS/cipher.d \
./Middlewares/Third_Party/MbedTLS/cipher_wrap.d \
./Middlewares/Third_Party/MbedTLS/cmac.d \
./Middlewares/Third_Party/MbedTLS/platform.d \
./Middlewares/Third_Party/MbedTLS/platform_util.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/MbedTLS/aes.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/aes.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/MbedTLS/aes.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/MbedTLS/cipher.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/cipher.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/MbedTLS/cipher.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/MbedTLS/cipher_wrap.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/cipher_wrap.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/MbedTLS/cipher_wrap.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/MbedTLS/cmac.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/cmac.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/MbedTLS/cmac.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/MbedTLS/platform.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/platform.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/MbedTLS/platform.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/MbedTLS/platform_util.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/mbedTLS/library/platform_util.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/MbedTLS/platform_util.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

