#ifndef _SPIPROTO_H_
#define _SPIPROTO_H_

#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/spi.h>

inline u8 PROTOSPI_read3wire(){
    u8 data;

    spi_disable(SPI2);
    spi_set_bidirectional_receive_only_mode(SPI2);

//    asm ("CPSID I\n"); //Disable global interrupts.
    spi_enable(SPI2);  //This starts the data read.

    //Wait > 1 SPI clock (but less than 8). Clock is 4.5MHz
    asm volatile ("NOP\n\tNOP\n\tNOP\n\tNOP\n\tNOP\n\t"
                  "NOP\n\tNOP\n\tNOP\n\tNOP\n\tNOP\n\t"
                  "NOP\n\tNOP\n\tNOP\n\tNOP\n\tNOP\n\t"
                  "NOP\n\tNOP\n\tNOP\n\tNOP\n\tNOP");

    spi_disable(SPI2); //This ends the read window when the transfer has finished.
//    asm ("CPSIE I\n"); //Enable global interrupts.
    data = spi_read(SPI2);
    spi_set_unidirectional_mode(SPI2);
    spi_enable(SPI2);
    return data;
}

#ifdef UNIMOD
// Hard coded CS pin.
#define CS GPIO12
#define PROTOSPI_pin_set(io) gpio_set(GPIOB, CS)
#define PROTOSPI_pin_clear(io) gpio_clear(GPIOB, CS)
#endif // UNIMOD

#ifndef UNIMOD
#define PROTOSPI_pin_set(io) gpio_set(io.port,io.pin)
#define PROTOSPI_pin_clear(io) gpio_clear(io.port,io.pin)
#endif // UNIMOD

#define PROTOSPI_xfer(byte) spi_xfer(SPI2, byte)
#define _NOP()  asm volatile ("NOP")

static const struct mcu_pin CYRF_RESET_PIN ={GPIOB, GPIO11};
static const struct mcu_pin AVR_RESET_PIN ={GPIO_BANK_JTCK_SWCLK, GPIO_JTCK_SWCLK};

#endif // _SPIPROTO_H_
