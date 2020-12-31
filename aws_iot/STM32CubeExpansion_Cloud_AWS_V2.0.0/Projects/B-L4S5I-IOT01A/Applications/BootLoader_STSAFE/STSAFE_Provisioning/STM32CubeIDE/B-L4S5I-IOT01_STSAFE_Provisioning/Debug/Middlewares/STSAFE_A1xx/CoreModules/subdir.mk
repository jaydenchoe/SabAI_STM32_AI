################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STSAFE_A1xx/CoreModules/Src/stsafea_core.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STSAFE_A1xx/CoreModules/Src/stsafea_crypto.c \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STSAFE_A1xx/CoreModules/Src/stsafea_service.c 

OBJS += \
./Middlewares/STSAFE_A1xx/CoreModules/stsafea_core.o \
./Middlewares/STSAFE_A1xx/CoreModules/stsafea_crypto.o \
./Middlewares/STSAFE_A1xx/CoreModules/stsafea_service.o 

C_DEPS += \
./Middlewares/STSAFE_A1xx/CoreModules/stsafea_core.d \
./Middlewares/STSAFE_A1xx/CoreModules/stsafea_crypto.d \
./Middlewares/STSAFE_A1xx/CoreModules/stsafea_service.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/STSAFE_A1xx/CoreModules/stsafea_core.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STSAFE_A1xx/CoreModules/Src/stsafea_core.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/STSAFE_A1xx/CoreModules/stsafea_core.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/STSAFE_A1xx/CoreModules/stsafea_crypto.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STSAFE_A1xx/CoreModules/Src/stsafea_crypto.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/STSAFE_A1xx/CoreModules/stsafea_crypto.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/STSAFE_A1xx/CoreModules/stsafea_service.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/ST/STSAFE_A1xx/CoreModules/Src/stsafea_service.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/STSAFE_A1xx/CoreModules/stsafea_service.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

