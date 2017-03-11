################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../target/rfmodule/channels.c \
../target/rfmodule/printf.c \
../target/rfmodule/unimod.c 

OBJS += \
./target/rfmodule/channels.o \
./target/rfmodule/printf.o \
./target/rfmodule/unimod.o 

C_DEPS += \
./target/rfmodule/channels.d \
./target/rfmodule/printf.d \
./target/rfmodule/unimod.d 


# Each subdirectory must supply rules for building sources it contributes
target/rfmodule/%.o: ../target/rfmodule/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror=undef -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g3 -DSTM32F1 -DUNIMOD=1 -I"D:\eclipse_work\rfmodule" -I"D:\eclipse_work\rfmodule\protocol" -I"D:\eclipse_work\rfmodule\target\rfmodule" -I"D:\eclipse_work\rfmodule\libopencm3\include" -std=gnu99 --specs=nano.specs -fno-builtin-printf -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


