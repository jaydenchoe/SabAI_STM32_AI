################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/mbedTLS/subdir.mk
-include Middlewares/STM32_Secure_Engine/subdir.mk
-include Middlewares/STM32_Key_Management_Services/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/B-L475E-IOT01/subdir.mk
-include Application/STM32CubeIDE/subdir.mk
-include Application/SBSFU/Target/subdir.mk
-include Application/SBSFU/App/subdir.mk
-include Application/Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

BUILD_ARTIFACT_NAME := SBSFU
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SBSFU.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SBSFU.list \

OBJCOPY_BIN += \
SBSFU.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: SBSFU.elf secondary-outputs

# Tool invocations
SBSFU.elf: $(OBJS) $(USER_OBJS) ../STM32L4S5VI_FLASH.ld
	arm-none-eabi-gcc -o "SBSFU.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"../STM32L4S5VI_FLASH.ld" --specs=nosys.specs -Wl,-Map="B-L4S5I-IOT01_2_Images_SBSFU.map" -Wl,--gc-sections -static -Xlinker -L ../../../../Linker_Common/STM32CubeIDE --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SBSFU.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SBSFU.list"
	@echo 'Finished building: $@'
	@echo ' '

SBSFU.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "SBSFU.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	arm-none-eabi-objcopy -O binary "SBSFU.elf" "SBSFU.bin"
	arm-none-eabi-size "SBSFU.elf"
	arm-none-eabi-objcopy -j .SE_IF_Code "SBSFU.elf" se_inter.elf > /dev/null 2>>1
	arm-none-eabi-objcopy --extract-symbol se_inter.elf se_interface_app.elf
	arm-none-eabi-objcopy -S --keep-symbols=../se_interface.txt se_interface_app.elf se_interface_app.o
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY: post-build

-include ../makefile.targets
