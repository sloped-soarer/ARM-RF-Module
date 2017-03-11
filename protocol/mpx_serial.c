
#ifdef MODULAR
  //Allows the linker to properly relocate
  #define MPX_SERIAL_Cmds PROTO_Cmds
  #pragma long_calls
#endif

#include "common.h"
#include "interface.h"
#include "mixer.h"
#include "config/model.h"
#include "mpx_serial.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>

#include "unimod.h"
#define MPX_MAX_CHANNELS 16


#ifdef MODULAR
  #pragma long_calls_off
  extern unsigned _data_loadaddr;
  const unsigned long protocol_type = (unsigned long)&_data_loadaddr;
#endif

static const char * const mpx_serial_opts[] = {
		_tr_noop("Telemetry"),  _tr_noop("Off"), _tr_noop("On"), NULL,
		NULL
};

enum {
      PROTOOPTS_TELEMETRY = 0
};
//ctassert(LAST_PROTO_OPT <= NUM_PROTO_OPTS, too_many_protocol_opts);


volatile struct MPX_Telemetry mpx_telemetry[TELEM_MPX_CHANNEL_LAST];
extern volatile enum PROTO_MODE proto_mode;

// Linear buffer for received characters.
#define BUFFER_SIZE 50 // Frame is 35 octets.
volatile unsigned char l_buffer[BUFFER_SIZE];
#define MPX_TELEM_PKT_SIZE 5
volatile u8 mpx_telem_pkt[MPX_TELEM_PKT_SIZE];
extern volatile u8 g_sync_count;

void tim3_isr()
{
u16 us;

	if(l_buffer[0] & 0x02) us = 21000; // Fast response off.
	else us = 14000;

	timer_clear_flag(TIM3, TIM_SR_CC1IF);
    timer_set_oc_value(TIM3, TIM_OC1, us + TIM_CCR1(TIM3));

    if(RF_EN_STATE()) // Only send packets when signal is enabled ... becomes inactive when you stuff something into the trainer port.
    {
    build_mpx_telem_pkt();
    // Enable TXE interrupt.
    usart_enable_tx_interrupt(USART2);
    }
}


void usart2_isr(void)
{
static u8 read_ptr = 1;
static u8 write_ptr = 0; // next free position
static u16 previous_cnt =0;
static u8 servo_count = 0;
static u8 tx_ptr = 0;

// Disable receive interrupts.

/* Check if we were called because of RXNE. */
	if( USART_SR(USART2) & USART_SR_RXNE )
	{
	/* Retrieve the char from the peripheral. */
	u8 temp_octet = usart_recv(USART2);
	u16 current_cnt = 0xffff & TIM_CNT(TIM3);

	/*
	Frame seems to be comprised of the following :
	One header byte usually 0x8n, but i have seen 0x9? when a model memory was corrupted.
	nibble n appears to be a bitfield containing bit 2=set failsafe, 1=fast response off.

	Two bytes for each channel 1-16, LSB first (16 bit signed value) Range of -1521 to +1520.
	Two termination bytes of 0x00.
	*/

	u16 cnt_diff;
	if(current_cnt >= previous_cnt) cnt_diff = current_cnt - previous_cnt;
	else cnt_diff = (0xffff - previous_cnt) + current_cnt + 1 ;

	if(cnt_diff > 500) // > 500 uS.
		// Reset pointers for next frame.
		{
		write_ptr = 0;
		read_ptr = 1;
		g_sync_count ++;
		servo_count = 0;
		}

		l_buffer[write_ptr] = temp_octet;
		if(write_ptr < BUFFER_SIZE -1) write_ptr ++;

	// Read out channel data, little and often is good.
			if(write_ptr > read_ptr+1)
			{
				if(servo_count < NUM_OUT_CHANNELS)
				{
// Just some test values.
//				if (servo_count == 0) Channels[0] = CHAN_MIN_VALUE;
//				else if (servo_count == 1) Channels[1] = 0;
//				else if (servo_count == 2) Channels[2] = CHAN_MAX_VALUE;
//				else
					Channels[servo_count] = (l_buffer[read_ptr] | (l_buffer[read_ptr+1]<<8)) * MPX_MULTIPLIER;

				read_ptr +=2;
				servo_count++;
				}
			}
		previous_cnt = current_cnt;
	}
	/* Check if we were called because of TXE. */
	else if( USART_SR(USART2) & USART_SR_TXE )
	{
		//usart_disable_tx_interrupt(USART2);
		//for(u8 i=0; i< MPX_TELEM_PKT_SIZE; i++) usart_send_blocking(USART2, mpx_telem_pkt[i]);

		usart_send(USART2, mpx_telem_pkt[tx_ptr]);
		tx_ptr++;
		if(tx_ptr >= MPX_TELEM_PKT_SIZE)
		{
		usart_disable_tx_interrupt(USART2);;
		tx_ptr =0;
		}
	}
}


void build_mpx_telem_pkt()
{
/*
Telemetry frame consists of :
5 Octets.
1st Octet is a bitfield of
bit 7=?, 6=range test active, 5=bind active, 4=no rx detected,
bit 3=?, 2=normally set, 1=fast response off.
2nd Octet = 0xFF
3rd Octet high nibble = telemetry channel (0-14), low nibble=telemetry units e.g.
Volts, Amps, m/s Km/h rpm degC degF metres %fuel %LQI mAh mL
4th & 5th Bytes = telemetry value (15 bit signed  << 1), bit 0 = alarm condition. LSB first.
*/
static u8 telem_channel = TELEM_MPX_CHANNEL_0;

struct MPX_Telem_Flags {
unsigned not_known_0  :1;
unsigned fast_response_off :1;
unsigned normally_set :1;
unsigned not_known_3  :1;
unsigned no_rx        :1;
unsigned bind_active  :1;
unsigned range_active :1;
unsigned not_known_7  :1;
};

static union{
struct MPX_Telem_Flags flags_bits;
unsigned char flags;
}mpx_telem;

mpx_telem.flags = 0;
mpx_telem.flags_bits.normally_set =1;
// Return the state of the fast response bit.
if(l_buffer[0] & 0x02) mpx_telem.flags_bits.fast_response_off =1;

	if(proto_mode == BIND_MODE)
	{
		mpx_telem.flags_bits.bind_active =1;
		mpx_telem_pkt[0] = mpx_telem.flags;
		mpx_telem_pkt[1] = 0xFF;
		mpx_telem_pkt[2] = 0x00;
		mpx_telem_pkt[3] = 0x00;
		mpx_telem_pkt[4] = 0x00;
	}
	else // NORMAL_MODE and RANGE_MODE.
	{
		if(proto_mode == RANGE_MODE) mpx_telem.flags_bits.range_active =1;
		mpx_telem_pkt[0] = mpx_telem.flags;
		mpx_telem_pkt[1] = 0xFF;
		mpx_telem_pkt[2] = (telem_channel << 4) | mpx_telemetry[telem_channel].units;
		mpx_telem_pkt[3] = mpx_telemetry[telem_channel].value << 1 | mpx_telemetry[telem_channel].alarm;
		mpx_telem_pkt[4] = mpx_telemetry[telem_channel].value >> 7;
	}

	telem_channel ++;
	if(telem_channel > TELEM_MPX_CHANNEL_14) telem_channel = TELEM_MPX_CHANNEL_0;
}


unsigned char mpx_start(void)
{
u16 c;
unsigned char timeout =0;

// Initialise USART2.
 rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_USART2EN); // old libopencm3
// rcc_periph_clock_enable(RCC_USART2); // new libopencm3

/* Setup USART parameters. */
usart_set_baudrate(USART2, 19200);
usart_set_parity(USART2, USART_PARITY_NONE);
usart_set_databits(USART2, 8);
usart_set_stopbits(USART2, USART_STOPBITS_1);
usart_set_mode(USART2, USART_MODE_TX_RX);
usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

/* Enable the USART. */
usart_enable(USART2);

/* Setup PA2 & PA3 pins GPIO_USART2_TX & _RX  to Input floating */
gpio_set_mode(GPIO_BANK_USART2_RX,
		GPIO_MODE_INPUT,
	    GPIO_CNF_INPUT_FLOAT,
	    GPIO_USART2_TX | GPIO_USART2_RX );


// Start communication using polling.
	while(1)
	{
	if(usart_get_flag(USART2, USART_SR_RXNE)) break;  // Wait for read data register not empty.
	CLOCK_delayms(20);
	timeout++;
	if(timeout > 100) return 0; // 100 * 20ms = 2sec
	}

	c = usart_recv(USART2);
	if(c != 'v') return 0; // Transmitter asks for version of module ... only sends this twice before giving up.

	// Set TX pin to Output.
	gpio_set_mode(GPIO_BANK_USART2_TX,
			GPIO_MODE_OUTPUT_50_MHZ,
		    GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		    GPIO_USART2_TX);

	CLOCK_delayms(12);

	mpx_puts("F-LINK   V3044"); // HFM-4 M-LINK Identifier. Also "M-LINK   V3200".
	usart_send_blocking(USART2,0x0D);
	usart_send_blocking(USART2,0x0A);


	// Wait for transmit complete.
	while(! usart_get_flag(USART2, USART_SR_TC));  // Wait for read data register not empty.

	do
	{
	// Wait for receive complete.
	c = usart_recv_blocking(USART2);
	}
	while( (c !='b') && (c !='r') && (c !='a') ); // (b)ind(en), (r)ange, (a)ctivate ?.

	if(c=='b') proto_mode = BIND_MODE;
	else if(c=='r') proto_mode = RANGE_MODE;
	else proto_mode = NORMAL_MODE;

	usart_set_baudrate(USART2, 115200);

	/* Enable USART2 Receive interrupt. */
	// clear RXNE flag in SR by writing it to 0.
	usart_enable_rx_interrupt(USART2);

	/* Enable the USART2 interrupt. */
	nvic_set_priority(NVIC_USART2_IRQ, (1 << 4) ); // Priority 1. Same priority group as TIM3 (No preemption).
	nvic_enable_irq(NVIC_USART2_IRQ);

return 1;
}


void mpx_puts(char * str)
{
	while(str[0]!=0)
	{
		usart_send_blocking(USART2, str[0]);
	    str++;
	}
}


static void initialize()
{
	// Initialise telemetry structure.
	for(u8 i=0; i < TELEM_MPX_CHANNEL_LAST; i++)
    {
    	mpx_telemetry[i].alarm = 0;
    	mpx_telemetry[i].value = 0;
    	mpx_telemetry[i].units = TELEM_MPX_NOT_USED;
    }

    if(mpx_start())
    {
        /* Enable TIM3 clock. */
        rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM3EN);
        timer_enable_1us_oc1(TIM3);
        nvic_set_priority(NVIC_TIM3_IRQ, (1 << 4) ); // Priority 1. Same priority group as TIM3 (No preemption).
    	nvic_enable_irq(NVIC_TIM3_IRQ);
        timer_generate_event(TIM3, TIM_EGR_UG);

        timer_set_oc_value(TIM3, TIM_OC1, 10000 + TIM_CNT(TIM3));
        timer_enable_counter(TIM3);
    }
    else MPX_SERIAL_Cmds(PROTOCMD_DEINIT);
}


const void * MPX_SERIAL_Cmds(enum ProtoCmds cmd)
{
    switch(cmd) {
        case PROTOCMD_INIT:  initialize(); return 0;
        case PROTOCMD_DEINIT:
        case PROTOCMD_RESET:
        	nvic_disable_irq(NVIC_TIM3_IRQ);
        	timer_reset(TIM3);
        	usart_disable(USART2);
    		/* Set TX & RX  to Input floating */
    		gpio_set_mode(GPIO_BANK_USART2_TX,
    				GPIO_MODE_INPUT,
    			    GPIO_CNF_INPUT_FLOAT,
    			    GPIO_USART2_TX | GPIO_USART2_RX );
    		return (void *) 1L;

        case PROTOCMD_CHECK_AUTOBIND: return (void *)0L; //Never Autobind
        case PROTOCMD_BIND:  initialize(); return 0;
        case PROTOCMD_NUMCHAN: return (void *) 7L;
        case PROTOCMD_DEFAULT_NUMCHAN: return (void *)16L;
        case PROTOCMD_GETOPTIONS: return mpx_serial_opts;
        case PROTOCMD_TELEMETRYSTATE: return (void *)(long) PROTO_TELEM_ON;
        default: break;
    }
    return 0;
}

