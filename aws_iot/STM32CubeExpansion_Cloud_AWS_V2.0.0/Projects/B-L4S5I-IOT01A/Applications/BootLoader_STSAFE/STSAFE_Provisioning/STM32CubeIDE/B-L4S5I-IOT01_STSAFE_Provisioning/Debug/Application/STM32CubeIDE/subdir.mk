################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/STSAFE_Provisioning/STM32CubeIDE/startup_stm32l4S5xx.s 

C_SRCS += \
/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/STSAFE_Provisioning/STM32CubeIDE/syscalls.c 

OBJS += \
./Application/STM32CubeIDE/startup_stm32l4S5xx.o \
./Application/STM32CubeIDE/syscalls.o 

S_DEPS += \
./Application/STM32CubeIDE/startup_stm32l4S5xx.d 

C_DEPS += \
./Application/STM32CubeIDE/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
Application/STM32CubeIDE/startup_stm32l4S5xx.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/STSAFE_Provisioning/STM32CubeIDE/startup_stm32l4S5xx.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"Application/STM32CubeIDE/startup_stm32l4S5xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
Application/STM32CubeIDE/syscalls.o: /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/BootLoader_STSAFE/STSAFE_Provisioning/STM32CubeIDE/syscalls.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32L4S5xx -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 '-DMBEDTLS_CONFIG_FILE=<config_mbedtls.h>' -DSTSAFE_A110 -c -I../../../Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/BSP/B-L475E-IOT01 -I../../../../../../../../Drivers/BSP/Components/Common -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include -I../../../../../../../../Middlewares/Third_Party/mbedTLS/include/mbedtls -I../../../../../../../../Middlewares/ST/STSAFE_A1xx/CoreModules/Inc -I../../../../Linker_Common/SW4STM32 -I../../../../../../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Application/STM32CubeIDE/syscalls.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

