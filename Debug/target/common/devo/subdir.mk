################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../target/common/devo/clock.c \
../target/common/devo/power.c \
../target/common/devo/ppmin.c \
../target/common/devo/proto_stubs.c \
../target/common/devo/spi_proto.c \
../target/common/devo/tx_misc.c \
../target/common/devo/uart.c 

OBJS += \
./target/common/devo/clock.o \
./target/common/devo/power.o \
./target/common/devo/ppmin.o \
./target/common/devo/proto_stubs.o \
./target/common/devo/spi_proto.o \
./target/common/devo/tx_misc.o \
./target/common/devo/uart.o 

C_DEPS += \
./target/common/devo/clock.d \
./target/common/devo/power.d \
./target/common/devo/ppmin.d \
./target/common/devo/proto_stubs.d \
./target/common/devo/spi_proto.d \
./target/common/devo/tx_misc.d \
./target/common/devo/uart.d 


# Each subdirectory must supply rules for building sources it contributes
target/common/devo/%.o: ../target/common/devo/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror=undef -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g3 -DSTM32F1 -DUNIMOD=3 -I"/home/sloped-soarer/eclipse_work/rfmodule" -I"/home/sloped-soarer/eclipse_work/rfmodule/libopencm3/include" -I"/home/sloped-soarer/eclipse_work/rfmodule/target/unimod" -std=gnu99 --specs=nano.specs -fno-builtin-printf -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


