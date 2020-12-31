################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/amazon-freertos/libraries/freertos_plus/standard/utils/subdir.mk
-include Middlewares/amazon-freertos/libraries/freertos_plus/standard/tls/subdir.mk
-include Middlewares/amazon-freertos/libraries/freertos_plus/standard/pkcs11/subdir.mk
-include Middlewares/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_posix/subdir.mk
-include Middlewares/amazon-freertos/libraries/freertos_plus/standard/crypto/subdir.mk
-include Middlewares/amazon-freertos/libraries/freertos_plus/aws/ota/subdir.mk
-include Middlewares/amazon-freertos/libraries/freertos_plus/aws/greengrass/subdir.mk
-include Middlewares/amazon-freertos/libraries/c_sdk/standard/serializer/subdir.mk
-include Middlewares/amazon-freertos/libraries/c_sdk/standard/mqtt/subdir.mk
-include Middlewares/amazon-freertos/libraries/c_sdk/standard/https/subdir.mk
-include Middlewares/amazon-freertos/libraries/c_sdk/standard/common/taskpool/subdir.mk
-include Middlewares/amazon-freertos/libraries/c_sdk/standard/common/logging/subdir.mk
-include Middlewares/amazon-freertos/libraries/c_sdk/standard/common/subdir.mk
-include Middlewares/amazon-freertos/libraries/c_sdk/aws/shadow/subdir.mk
-include Middlewares/amazon-freertos/libraries/c_sdk/aws/defender/subdir.mk
-include Middlewares/amazon-freertos/libraries/abstractions/platform/subdir.mk
-include Middlewares/amazon-freertos/libraries/3rdparty/tinycbor/subdir.mk
-include Middlewares/amazon-freertos/libraries/3rdparty/mbedtls/subdir.mk
-include Middlewares/amazon-freertos/libraries/3rdparty/jsmn/subdir.mk
-include Middlewares/amazon-freertos/libraries/3rdparty/http_parser/subdir.mk
-include Middlewares/amazon-freertos/freertos_kernel/portable/subdir.mk
-include Middlewares/amazon-freertos/freertos_kernel/subdir.mk
-include Middlewares/amazon-freertos/demos/tcp/subdir.mk
-include Middlewares/amazon-freertos/demos/shadow/subdir.mk
-include Middlewares/amazon-freertos/demos/ota/subdir.mk
-include Middlewares/amazon-freertos/demos/network_manager/subdir.mk
-include Middlewares/amazon-freertos/demos/mqtt/subdir.mk
-include Middlewares/amazon-freertos/demos/https/subdir.mk
-include Middlewares/amazon-freertos/demos/dev_mode_key_provisioning/subdir.mk
-include Middlewares/amazon-freertos/demos/demo_runner/subdir.mk
-include Middlewares/amazon-freertos/demos/defender/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/Components/vl53l0x/subdir.mk
-include Drivers/BSP/Components/lsm6dsl/subdir.mk
-include Drivers/BSP/Components/lps22hb/subdir.mk
-include Drivers/BSP/Components/lis3mdl/subdir.mk
-include Drivers/BSP/Components/hts221/subdir.mk
-include Drivers/BSP/Components/es_wifi/subdir.mk
-include Drivers/BSP/B-L475E-IOT01/subdir.mk
-include BootLoader_STSAFE/2_Images_SBSFU/STM32CubeIDE/subdir.mk
-include Application/User/subdir.mk
-include Application/Startup/subdir.mk
-include Application/AWS/ports/subdir.mk
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

BUILD_ARTIFACT_NAME := B-L4S5I-IOT01_aws_demos
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
B-L4S5I-IOT01_aws_demos.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
B-L4S5I-IOT01_aws_demos.list \

OBJCOPY_BIN += \
B-L4S5I-IOT01_aws_demos.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory pre-build && $(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: B-L4S5I-IOT01_aws_demos.elf secondary-outputs

# Tool invocations
B-L4S5I-IOT01_aws_demos.elf: $(OBJS) $(USER_OBJS) /Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/Cloud/aws_demos/STM32CubeIDE/STM32L4S5VITX_FLASH_sbsfu.ld
	arm-none-eabi-gcc -z max-page-size=0x200 -o "B-L4S5I-IOT01_aws_demos.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/Cloud/aws_demos/STM32CubeIDE/STM32L4S5VITX_FLASH_sbsfu.ld" --specs=nosys.specs -Wl,-Map="B-L4S5I-IOT01_aws_demos.map" -Wl,--gc-sections -static -L../../../../BootLoader_STSAFE/2_Images_SBSFU/STM32CubeIDE/B-L4S5I-IOT01_2_Images_SBSFU/Debug -Xlinker -L ../../../../BootLoader_STSAFE/Linker_Common/STM32CubeIDE --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

B-L4S5I-IOT01_aws_demos.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "B-L4S5I-IOT01_aws_demos.list"
	@echo 'Finished building: $@'
	@echo ' '

B-L4S5I-IOT01_aws_demos.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "B-L4S5I-IOT01_aws_demos.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

pre-build:
	rm -f B-L4S5I-IOT01_aws_demos.elf
	-@echo ' '

post-build:
	"../../../../BootLoader_STSAFE/2_Images_SECoreBin/STM32CubeIDE/postbuild.sh" ".." "./B-L4S5I-IOT01_aws_demos.elf" "./B-L4S5I-IOT01_aws_demos.bin" "1" "bigelf"
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
