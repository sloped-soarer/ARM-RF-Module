################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../target/devo8/channels.c 

OBJS += \
./target/devo8/channels.o 

C_DEPS += \
./target/devo8/channels.d 


# Each subdirectory must supply rules for building sources it contributes
target/devo8/%.o: ../target/devo8/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfix-cortex-m3-ldrd -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror=undef -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g3 -DSTM32F1 -D"assert_param(x)=" -DSTM32F10X_HD -I"C:\Users\user\Downloads\rfmodule\libopencm3\include" -I"C:\Users\user\Downloads\rfmodule\target\devo8" -I"C:\Users\user\Downloads\rfmodule\target\common\devo" -I"C:\Users\user\Downloads\rfmodule" -std=gnu99 --specs=nano.specs -fno-builtin-printf -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


