################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../protocol/devo_cyrf6936.c \
../protocol/dsm2_cyrf6936.c \
../protocol/fbl100_nrf24l01_rick.c \
../protocol/flysky_a7105_rick.c \
../protocol/frsky1way_cc2500_rick.c \
../protocol/frsky2way_cc2500_rick.c \
../protocol/h377_nrf24l01.c \
../protocol/hisky_nrf24l01.c \
../protocol/hm830_nrf24l01.c \
../protocol/hubsan_a7105.c \
../protocol/j6pro_cyrf6936.c \
../protocol/mpx_serial.c \
../protocol/ppm_ic.c \
../protocol/ppmout.c \
../protocol/protocol.c \
../protocol/skyartec_cc2500.c \
../protocol/slt_nrf24l01.c \
../protocol/testrf.c \
../protocol/usbhid.c \
../protocol/v202_nrf24l01.c \
../protocol/wk2x01.c \
../protocol/yd717_nrf24l01.c 

OBJS += \
./protocol/devo_cyrf6936.o \
./protocol/dsm2_cyrf6936.o \
./protocol/fbl100_nrf24l01_rick.o \
./protocol/flysky_a7105_rick.o \
./protocol/frsky1way_cc2500_rick.o \
./protocol/frsky2way_cc2500_rick.o \
./protocol/h377_nrf24l01.o \
./protocol/hisky_nrf24l01.o \
./protocol/hm830_nrf24l01.o \
./protocol/hubsan_a7105.o \
./protocol/j6pro_cyrf6936.o \
./protocol/mpx_serial.o \
./protocol/ppm_ic.o \
./protocol/ppmout.o \
./protocol/protocol.o \
./protocol/skyartec_cc2500.o \
./protocol/slt_nrf24l01.o \
./protocol/testrf.o \
./protocol/usbhid.o \
./protocol/v202_nrf24l01.o \
./protocol/wk2x01.o \
./protocol/yd717_nrf24l01.o 

C_DEPS += \
./protocol/devo_cyrf6936.d \
./protocol/dsm2_cyrf6936.d \
./protocol/fbl100_nrf24l01_rick.d \
./protocol/flysky_a7105_rick.d \
./protocol/frsky1way_cc2500_rick.d \
./protocol/frsky2way_cc2500_rick.d \
./protocol/h377_nrf24l01.d \
./protocol/hisky_nrf24l01.d \
./protocol/hm830_nrf24l01.d \
./protocol/hubsan_a7105.d \
./protocol/j6pro_cyrf6936.d \
./protocol/mpx_serial.d \
./protocol/ppm_ic.d \
./protocol/ppmout.d \
./protocol/protocol.d \
./protocol/skyartec_cc2500.d \
./protocol/slt_nrf24l01.d \
./protocol/testrf.d \
./protocol/usbhid.d \
./protocol/v202_nrf24l01.d \
./protocol/wk2x01.d \
./protocol/yd717_nrf24l01.d 


# Each subdirectory must supply rules for building sources it contributes
protocol/%.o: ../protocol/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror=undef -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable  -g3 -DSTM32F1 -DUNIMOD=3 -I"/home/sloped-soarer/eclipse_work/rfmodule" -I"/home/sloped-soarer/eclipse_work/rfmodule/libopencm3/include" -I"/home/sloped-soarer/eclipse_work/rfmodule/target/unimod" -std=gnu99 --specs=nano.specs -fno-builtin-printf -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


