################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include target/unimod/subdir.mk
-include target/common/devo/protocol/subdir.mk
-include target/common/devo/subdir.mk
-include protocol/spi/subdir.mk
-include protocol/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
rfmodule.hex \

SECONDARY_SIZE += \
rfmodule.siz \


# All Target
all: pre-build main-build

# Main-build Target
main-build: rfmodule.elf secondary-outputs

# Tool invocations
rfmodule.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror=undef -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g3 -T "/home/sloped-soarer/eclipse_work/rfmodule/target/unimod/rfmodule.ld" -nostartfiles -Xlinker --gc-sections -L"/home/sloped-soarer/eclipse_work/rfmodule/libopencm3/lib" -Wl,-Map,"rfmodule.map" -Xlinker --cref --specs=nano.specs -lc -lnosys -o "rfmodule.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

rfmodule.hex: rfmodule.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "rfmodule.elf"  "rfmodule.hex"
	@echo 'Finished building: $@'
	@echo ' '

rfmodule.siz: rfmodule.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "rfmodule.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) rfmodule.elf
	-@echo ' '

pre-build:
	-make -C "/home/sloped-soarer/eclipse_work/rfmodule/libopencm3" TARGETS=stm32/f1 lib
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build

-include ../makefile.targets
