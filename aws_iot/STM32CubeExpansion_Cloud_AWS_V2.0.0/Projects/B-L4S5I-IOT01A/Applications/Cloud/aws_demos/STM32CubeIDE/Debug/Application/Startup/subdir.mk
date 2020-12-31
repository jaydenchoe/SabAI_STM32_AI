################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Application/Startup/startup_stm32l4s5vitx.s 

OBJS += \
./Application/Startup/startup_stm32l4s5vitx.o 

S_DEPS += \
./Application/Startup/startup_stm32l4s5vitx.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Startup/startup_stm32l4s5vitx.o: ../Application/Startup/startup_stm32l4s5vitx.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -I../../../../../../../Middlewares/Third_Party/amazon-freertos/freertos_kernel/include -I../../config_files -I../ -x assembler-with-cpp -MMD -MP -MF"Application/Startup/startup_stm32l4s5vitx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

