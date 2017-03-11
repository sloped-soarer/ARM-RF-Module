################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../protocol/spi/a7105.c \
../protocol/spi/avr_program.c \
../protocol/spi/cc2500.c \
../protocol/spi/cyrf6936.c \
../protocol/spi/nrf24l01.c 

OBJS += \
./protocol/spi/a7105.o \
./protocol/spi/avr_program.o \
./protocol/spi/cc2500.o \
./protocol/spi/cyrf6936.o \
./protocol/spi/nrf24l01.o 

C_DEPS += \
./protocol/spi/a7105.d \
./protocol/spi/avr_program.d \
./protocol/spi/cc2500.d \
./protocol/spi/cyrf6936.d \
./protocol/spi/nrf24l01.d 


# Each subdirectory must supply rules for building sources it contributes
protocol/spi/%.o: ../protocol/spi/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror=undef -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g3 -DSTM32F1 -DUNIMOD=3 -I"/home/sloped-soarer/eclipse_work/rfmodule" -I"/home/sloped-soarer/eclipse_work/rfmodule/libopencm3/include" -I"/home/sloped-soarer/eclipse_work/rfmodule/target/unimod" -std=gnu99 --specs=nano.specs -fno-builtin-printf -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


