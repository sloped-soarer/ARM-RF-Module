################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../target/common/devo/protocol/pwm.c 

OBJS += \
./target/common/devo/protocol/pwm.o 

C_DEPS += \
./target/common/devo/protocol/pwm.d 


# Each subdirectory must supply rules for building sources it contributes
target/common/devo/protocol/%.o: ../target/common/devo/protocol/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror=undef -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g3 -DSTM32F1 -DUNIMOD=3 -I"/home/sloped-soarer/eclipse_work/rfmodule" -I"/home/sloped-soarer/eclipse_work/rfmodule/libopencm3/include" -I"/home/sloped-soarer/eclipse_work/rfmodule/target/unimod" -std=gnu99 --specs=nano.specs -fno-builtin-printf -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


