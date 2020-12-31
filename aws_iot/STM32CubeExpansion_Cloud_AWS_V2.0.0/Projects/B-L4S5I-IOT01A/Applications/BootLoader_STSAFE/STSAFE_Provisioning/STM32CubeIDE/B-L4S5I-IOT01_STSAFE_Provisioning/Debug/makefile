################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/MbedTLS/subdir.mk
-include Middlewares/STSAFE_A1xx/CoreModules/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/subdir.mk
-include Drivers/BSP/B-L475E-IOT01/subdir.mk
-include Application/User/HAL_STSAFE-Axx/subdir.mk
-include Application/User/subdir.mk
-include Application/STM32CubeIDE/subdir.mk
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

BUILD_ARTIFACT_NAME := B-L4S5I-IOT01_STSAFE_Provisioning
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
B-L4S5I-IOT01_STSAFE_Provisioning.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
B-L4S5I-IOT01_STSAFE_Provisioning.list \

OBJCOPY_BIN += \
B-L4S5I-IOT01_STSAFE_Provisioning.bin \


# All Target
all: main-build

# Main-build Target
main-build: B-L4S5I-IOT01_STSAFE_Provisioning.elf secondary-outputs

# Tool invocations
B-L4S5I-IOT01_STSAFE_Provisioning.elf: $(OBJS) $(USER_OBJS) ../STM32L4S5VI_FLASH.ld
	arm-none-eabi-gcc -o "B-L4S5I-IOT01_STSAFE_Provisioning.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"../STM32L4S5VI_FLASH.ld" --specs=nosys.specs -Wl,-Map="B-L4S5I-IOT01_STSAFE_Provisioning.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

B-L4S5I-IOT01_STSAFE_Provisioning.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "B-L4S5I-IOT01_STSAFE_Provisioning.list"
	@echo 'Finished building: $@'
	@echo ' '

B-L4S5I-IOT01_STSAFE_Provisioning.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "B-L4S5I-IOT01_STSAFE_Provisioning.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY:

-include ../makefile.targets
