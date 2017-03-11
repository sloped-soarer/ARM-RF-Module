################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../telemetry/telem_devo.c \
../telemetry/telem_dsm.c 

OBJS += \
./telemetry/telem_devo.o \
./telemetry/telem_dsm.o 

C_DEPS += \
./telemetry/telem_devo.d \
./telemetry/telem_dsm.d 


# Each subdirectory must supply rules for building sources it contributes
telemetry/%.o: ../telemetry/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror=undef -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g3 -DSTM32F1 -DUNIMOD=1 -D"assert_param(x)=" -I"D:\eclipse_work\rfmodule" -I"D:\eclipse_work\rfmodule\protocol" -I"D:\eclipse_work\rfmodule\target\rfmodule" -I"D:\eclipse_work\rfmodule\libopencm3\include" -std=gnu99 --specs=nano.specs -fno-builtin-printf -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


