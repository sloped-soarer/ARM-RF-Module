// TODO EEPROM read and write.
// TODO Watchdog ... with quick return to normal comm's if activated.


#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/f1/memorymap.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/desig.h>

#include "common.h"
#include "unimod.h"
#include "config/model.h"
#include "protocol/interface.h"
#include "protocol/mpx_serial.h"


//#define PROTODEF(proto, module, map, cmd, name)
//#include "protocol/protocol.h"
//#undef PROTODEF

extern volatile struct MPX_Telemetry mpx_telemetry[];
volatile enum PROTO_MODE proto_mode = NORMAL_MODE; // static u8 proto_state is similar in function, declared in protocol.c
extern volatile u32 msecs;
volatile u32 bind_press_time = 0;
volatile u8 g_sync_count = 0;

//extern void a7105_write_id(u8 * data);
//extern void a7105_read_id(u8 * data);
extern void NRF24L01_WriteRegisterMulti_test(u8 reg, u8 * data, u8 len);
extern void NRF24L01_ReadRegisterMulti_test(u8 reg, u8 * data, u8 len);
void NRF24L01_spi_test(void);

extern const void * (*PROTO_Cmds)(enum ProtoCmds);


// printf for USART3 (debug)
#define printf_u3 tfp_printf // Avoids lots of stuff in std.h
void init_printf(void *, void (*put_u3)(void *, char));
void put_u3(void * p, char c);
void output_data(void);


void uni_mod()
{
	initialise_hardware();

    init_printf(NULL, put_u3); // Apparently we have to do this.
    // Might be a possibility of re-initialising to point to a different port on the fly.
    printf_u3("\x1B\x5BH\n\n\n\n\n\n\n\n\n\n\n\n");
    u32 result[3];
    desig_get_unique_id(result); // D33FA63A  D126A83B
    result[0] = result[0] ^ result[1] ^ result[2];
    printf_u3("%08X\n\r", result[0]);

    // printf_u3("%08X\n\r", (u32 *)SCB_AIRCR);

    // TODO Retrieve Model.protocol etc from EEPROM.
    Model.fixed_id = 0x76543210; // rick
    Model.num_channels = 7;
    Model.tx_power = TXPOWER_100mW,
    Model.protocol = PROTOCOL_FRSKY2WAY;
    /* Hardcode protocol options for now */
    if(PROTOCOL_YD717 == Model.protocol)
    {
    	Model.proto_opts[0] = 1; // Format = "Sky Wlkr"
    	Model.proto_opts[1] = 1; // Telemetry = "On"
    }
    if(PROTOCOL_HUBSAN == Model.protocol)
    {
       	Model.proto_opts[0] = 1; // vTX MHz = 0
//    	Model.proto_opts[1] = 0; // Telemetry = "Off"
    }
    if(PROTOCOL_FRSKY1WAY || PROTOCOL_FRSKY2WAY == Model.protocol)
    {
    	Model.proto_opts[0] = -8; // Freq-Fine = -17
    	// Frsky rf deck = 0, Skyartec rf module = -17.
    	if(PROTOCOL_FRSKY2WAY == Model.protocol) Model.proto_opts[1] = 1; // Telemetry on.
    }

    PROTO_Cmds = select_protocol_cmds(Model.protocol);
    set_proto_leds(Model.protocol);

    printf_u3("Using Protocol %s \n\r",ProtocolNames[Model.protocol]);
    display_proto_options(Model.protocol);

    TELEMETRY_SetTypeByProtocol(Model.protocol); // Using this here, but really each protocol should select the telemetry type.
    // For some reason FrSky does not.
    // Need to know telemetry type so that mpx telemetry units can be initialised.

#if UNIMOD == 1
    MPX_SERIAL_Cmds(PROTOCMD_INIT);
#else
    if(read_bind_sw()) proto_mode = BIND_MODE;
//    else proto_mode = NORMAL_MODE;

    PPM_IC_Cmds(PROTOCMD_INIT);
#endif

    // Wait until we have received 5 frames.
    // Does not start protocol until valid pulses have been received.
    while (g_sync_count < 6);

    if(NULL != PROTO_Cmds)
    {
    	if(BIND_MODE == proto_mode)	PROTO_Cmds(PROTOCMD_BIND);
    	else if(NORMAL_MODE == proto_mode) PROTO_Cmds(PROTOCMD_INIT);
    	else // RANGE_MODE == proto_mode
    	{
    		// TODO Set output power to reduced value  -30 dB.
    		PROTO_Cmds(PROTOCMD_INIT);
    	}
    }


#if UNIMOD == 1
    // Choose Telemetry type and initialise units for multiplex telemetry output.
    if(TELEM_DEVO == TELEMETRY_Type())
    {
    	//mpx_telemetry[TELEM_MPX_CHANNEL_0].units = 	TELEM_MPX_P_CENT_LQI;

    	mpx_telemetry[TELEM_MPX_CHANNEL_1].units = 	TELEM_MPX_VOLTS_TENTHS;
    	mpx_telemetry[TELEM_MPX_CHANNEL_2].units = 	TELEM_MPX_VOLTS_TENTHS;
    	mpx_telemetry[TELEM_MPX_CHANNEL_3].units = 	TELEM_MPX_VOLTS_TENTHS;

    	mpx_telemetry[TELEM_MPX_CHANNEL_4].units = 	TELEM_MPX_DEGREES_TENTHS;
    	mpx_telemetry[TELEM_MPX_CHANNEL_5].units = 	TELEM_MPX_DEGREES_TENTHS;
    	mpx_telemetry[TELEM_MPX_CHANNEL_6].units = 	TELEM_MPX_DEGREES_TENTHS;
    	mpx_telemetry[TELEM_MPX_CHANNEL_7].units = 	TELEM_MPX_DEGREES_TENTHS;

    	mpx_telemetry[TELEM_MPX_CHANNEL_8].units = 	TELEM_MPX_RPM_HUNDREDS;
    	mpx_telemetry[TELEM_MPX_CHANNEL_9].units = 	TELEM_MPX_RPM_HUNDREDS;
    }
    else if(TELEM_DSM == TELEMETRY_Type())
    {
    	mpx_telemetry[TELEM_MPX_CHANNEL_0].units = 	TELEM_MPX_P_CENT_LQI;
    }
    else if(TELEM_FRSKY == TELEMETRY_Type())
    {
		mpx_telemetry[TELEM_MPX_CHANNEL_0].units = TELEM_MPX_P_CENT_LQI; // Just an interim measure to fit with Ricks FrSky protocol.

		mpx_telemetry[TELEM_FRSKY_VOLT1].units = TELEM_MPX_VOLTS_TENTHS;
    	mpx_telemetry[TELEM_FRSKY_VOLT2].units = TELEM_MPX_VOLTS_TENTHS;
    	mpx_telemetry[TELEM_FRSKY_VOLT3].units = TELEM_MPX_VOLTS_TENTHS;

    	mpx_telemetry[TELEM_FRSKY_TEMP1].units = TELEM_MPX_DEGREES_TENTHS;
    	mpx_telemetry[TELEM_FRSKY_TEMP2].units = TELEM_MPX_DEGREES_TENTHS;

    	mpx_telemetry[TELEM_FRSKY_RPM].units  =  TELEM_MPX_RPM_HUNDREDS;

    	mpx_telemetry[TELEM_FRSKY_ALTITUDE].units =	TELEM_MPX_METRES;
    }
    else{}; // No telemetry.
#endif

    while(1) // main loopy.
    {
    	if( bind_button_duration() > 5) // 6 seconds.
   			change_protocol();

 //   	output_data();

#if UNIMOD == 1
    	// If trainer port is used, suspend the RF protocol.
    	if(! RF_EN_STATE() && (msecs > 4000))
    	{
    	MPX_SERIAL_Cmds(PROTOCMD_DEINIT);
    	if(NULL != PROTO_Cmds) PROTO_Cmds(PROTOCMD_DEINIT);
    	proto_mode = RANGE_MODE;
    	set_proto_leds(PROTOCOL_NONE);
    	}
    	// MPX telemetry stuff.
     	for(int i=1; (i< TELEM_MPX_CHANNEL_LAST) && (i < TELEM_VALS); i++)
    	{
    	if(TELEMETRY_IsUpdated(i))
    		mpx_telemetry[i].value = TELEMETRY_GetValue(i);
    	}
    	if(TELEMETRY_IsUpdated(TELEM_DSM_FLOG_FRAMELOSS))
    		mpx_telemetry[TELEM_MPX_CHANNEL_0].value = Telemetry.p.dsm.flog.frameloss;
#endif // UNIMOD
    }
}



void set_proto_leds(enum Protocols idx)
{
LED_OFF(LED_G); LED_OFF(LED_R); LED_OFF(LED_Y);

	if(PROTOCOL_FBL100 == idx) LED_ON(LED_G);
	else if(PROTOCOL_HUBSAN == idx) LED_ON(LED_R);
	else if(PROTOCOL_YD717  == idx) LED_ON(LED_Y);
	else if(PROTOCOL_FRSKY1WAY == idx) {LED_ON(LED_G); LED_ON(LED_R);}
	else if(PROTOCOL_FRSKY2WAY == idx) {LED_ON(LED_R); LED_ON(LED_Y);}
}


void change_protocol(void)
{
// Protocol selection.
enum Protocols temp_protocol = Model.protocol;

	// TODO Add TX stick / slider selection method ?.

	printf_u3("Press Bind button to step through protocols\n\r");
	printf_u3("Press Bind button for more than 3 seconds to confirm selection\n\r");
	while(1)
	{
	s8 time = bind_button_duration();
		if(time == 0)
		{
			// short press < 1 sec.
			temp_protocol ++;
				if(TX_MODULE_LAST == get_module(temp_protocol) || temp_protocol >= PROTOCOL_COUNT)
	    			temp_protocol = PROTOCOL_NONE;

			printf_u3("Use Protocol %s ?\n\r",ProtocolNames[temp_protocol]);
	    	set_proto_leds(temp_protocol);	// Update LED's to indicate selection.
		}

		if(time > 2)
		{
			printf_u3("Selected Protocol is %s \n\r",ProtocolNames[temp_protocol]);
			display_proto_options(temp_protocol);
			// TODO Store temp_protocol as Model.protocol in EEPROM.
			set_proto_leds(PROTOCOL_NONE);
			// eeprom_write_word ((u16 *)6, temp_protocol);
			CLOCK_delayms(250);
			// while (!eeprom_is_ready()); //Make sure EEPROM write finishes
					for(u8 i=1; i<4; i++)
					{
					set_proto_leds(temp_protocol);
					CLOCK_delayms(250);
					set_proto_leds(PROTOCOL_NONE);
					CLOCK_delayms(250);
					}
			break;
		}
	}
}


#ifdef UNIMOD
void sys_tick_handler(void)
{
// Update milli-second count.
	msecs++;

// Read bind button with de-bounce.
static u8 prev_state =0;
u8 curr_state;
static u32 press_t =0;
u32 release_t;

curr_state = read_bind_sw();

	if( curr_state && ! prev_state) press_t = msecs;
	else if( ! curr_state && prev_state )
	{
	release_t = msecs;
		if((release_t - press_t) > 50) // De-bounce time 50ms.
			bind_press_time = release_t - press_t;
	}

	prev_state = curr_state;

// Update binding LED ... Mine is now blue.
	if(NORMAL_MODE == proto_mode) LED_OFF(LED_O);
	else if(BIND_MODE == proto_mode) LED_ON(LED_O);
	else if(RANGE_MODE == proto_mode)
	{
		if(msecs & 0x00000200) LED_ON(LED_O);
		else LED_OFF(LED_O);
	}
}
#endif // UNIMOD


s8 bind_button_duration(void)
{
s8 temp;

	if(bind_press_time)
	{
	// return as seconds.
	temp = bind_press_time / 1000;
	if(temp > 10) temp = 10;
	bind_press_time = 0; // Reset variable when acted upon.
	return temp;
	}
	else return -1;
}


u8 read_bind_sw(void)
{
// Change to +ve logic.
	return gpio_get(GPIOA, BIND_SWITCH) ? 0 : 1;
}


void CLOCK_delayms(u32 delay_ms)
{
u32 start_ms;

start_ms = msecs;
while(msecs < (start_ms + delay_ms));
}


#if 0
// Just some a7105 spi test code from another project.
// Please remove functions from protocol dir.
void a7105_spi_test(void)
{
unsigned long errors = 0;
unsigned long test = 0;
unsigned long start_ms;
u8 test_data_r[4];
u8 test_data_w[4] = {0x01,0x02,0x03,0x04};

u32 start_ms = msecs;
printf_u3("\n\rTesting SPI");
  for(test=0; test < 16653600 ; test++) // should run for X mins.
  {
  a7105_write_id(test_data_w);
  a7105_read_id(test_data_r);
    if(0 != memcmp(test_data_r, test_data_w, sizeof(test_data_r))) errors++;
  test_data_w[0] ++;
  test_data_w[1] ++;
  test_data_w[2] ++;
  test_data_w[3] ++;
  }

printf_u3("\n\rtest   "); printf_u3("%10d", test);
printf_u3("\n\rerrors "); printf_u3("%10d", errors);
printf_u3("\n\rtime   "); printf_u3("%10d", msecs - start_ms ); printf_u3("\n\r");

// 148374
// 145134
// 141737
// 142509
}
#endif

void NRF24L01_spi_test(void)
{
unsigned long errors = 0;
unsigned long test = 0;
unsigned long time;
u8 test_data_r[5];
u8 test_data_w[5] = {0x01,0x02,0x03,0x04,0x05};

u32 start_ms = msecs;
printf_u3("\n\rTesting SPI");
  for(test=0; test < 2775600 ; test++) // should run for X mins.
  {
	  NRF24L01_WriteRegisterMulti_test(NRF24L01_0B_RX_ADDR_P1, test_data_w, 5);
	  NRF24L01_ReadRegisterMulti_test(NRF24L01_0B_RX_ADDR_P1, test_data_r, 5);
    if(0 != memcmp(test_data_r, test_data_w, sizeof(test_data_r))) errors++;
  test_data_w[0] ++;
  test_data_w[1] ++;
  test_data_w[2] ++;
  test_data_w[3] ++;
  test_data_w[4] ++;
  }

  printf_u3("\n\rtest   "); printf_u3("%10d", test);
  printf_u3("\n\rerrors "); printf_u3("%10d", errors);
  printf_u3("\n\rtime   "); printf_u3("%10d", msecs - start_ms ); printf_u3("\n\r");

// 124211960
//  90899216
}

void usart_three_setup(void) // Debug USART.
{
	/* Enable USART3 clock */
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_USART3EN);

	/* Setup USART parameters. */
	usart_set_baudrate(USART3, 115200);
	usart_set_parity(USART3, USART_PARITY_NONE);
	usart_set_databits(USART3, 8);
	usart_set_stopbits(USART3, USART_STOPBITS_1);
	usart_set_mode(USART3, USART_MODE_TX_RX);
	usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART3);

	/* Configure the TX Pin */
		gpio_set_mode(GPIO_BANK_USART3_TX,
				GPIO_MODE_OUTPUT_50_MHZ,
			    GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
			    GPIO_USART3_TX);

		/* Configure the RX Pin */
		gpio_set_mode(GPIO_BANK_USART3_RX,
				GPIO_MODE_INPUT,
			    GPIO_CNF_INPUT_FLOAT,
			    GPIO_USART3_RX);
}


void output_data(void)
{
  printf_u3("\x1B\x5BH"); // const char vt100_cursor_home[] = "\x1B\x5BH";
  printf_u3("Channel\n\r");

  for(u8 i=0; i<8; i++) printf_u3("%2d  %8d       \n\r", i+1, (s16) Channels[i]);
}


const void * select_protocol_cmds(enum Protocols idx)
{
const void * (*Sel_PROTO_Cmds)(enum ProtoCmds);
	#define PROTODEF(proto, module, map, cmd, name) case proto: Sel_PROTO_Cmds = cmd; break;
	switch(idx) {
		#include "protocol/protocol.h"
	#undef PROTODEF
	default: Sel_PROTO_Cmds = NULL; break;
	}
return Sel_PROTO_Cmds;
}


static int get_module(enum Protocols idx)
{
	int m;
	#define PROTODEF(proto, module, map, cmd, name) case proto: m = module; break;
    switch(idx) {
        #include "protocol/protocol.h"
	#undef PROTODEF
	default: m = TX_MODULE_LAST; break;
    }
    return m;
}


void display_proto_options(enum Protocols proto_idx)
{
const char ** opts = NULL;
const void * (*Display_PROTO_Cmds)(enum ProtoCmds) = NULL;

Display_PROTO_Cmds = select_protocol_cmds(proto_idx);
if(Display_PROTO_Cmds) opts = (const char **)Display_PROTO_Cmds(PROTOCMD_GETOPTIONS);

if(! Display_PROTO_Cmds || ! opts || ! *opts )
{
	printf_u3("No Protocol Options\n\r");
}
else
{
char n_ops = 0;
	while( n_ops < NUM_PROTO_OPTS )
	{
	printf_u3("Protocol Option %d - %s\n\r", n_ops, *opts);
	opts ++;
		while(*opts)
		{
			printf_u3(" [%s]", *opts);
			opts++; // step to end of current option.
		}
		printf_u3("\n\r");
		n_ops ++;
		opts ++;
	if(! *opts) break;
	}
}
printf_u3("\n\r\n\r");
}


void initialise_hardware()
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	#if 0
	/* Set STM32 to run at 72MHz from external 8MHz Crystal */

	//rcc_clock_setup_in_hse_4mhz_out_72mhz();
	/*-----------------------------------------------------------------------------*/
	/** @brief RCC Set System Clock PLL at 72MHz from HSE at 8MHz

	*/

	void rcc_clock_setup_in_hse_8mhz_out_72mhz(void)
	{
		/* Enable internal high-speed oscillator. */
		rcc_osc_on(HSI);
		rcc_wait_for_osc_ready(HSI);

		/* Select HSI as SYSCLK source. */
		rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSICLK);

		/* Enable external high-speed oscillator 8MHz. */
		rcc_osc_on(HSE);
		rcc_wait_for_osc_ready(HSE);
		rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSECLK);

		/*
		 * Set prescalers for AHB, ADC, ABP1, ABP2.
		 * Do this before touching the PLL (TODO: why?).
		 */
		rcc_set_hpre(RCC_CFGR_HPRE_SYSCLK_NODIV);	/* Set. 72MHz Max. 72MHz */
		rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV8);	/* Set. 9MHz Max. 14MHz */
		rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_DIV2);	/* Set. 36MHz Max. 36MHz */
		rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV);	/* Set. 72MHz Max. 72MHz */

		/*
		 * Sysclk runs with 72MHz -> 2 waitstates.
		 * 0WS from 0-24MHz
		 * 1WS from 24-48MHz
		 * 2WS from 48-72MHz
		 */
		flash_set_ws(FLASH_LATENCY_2WS);

		/*
		 * Set the PLL multiplication factor to 9.
		 * 8MHz (external) * 9 (multiplier) = 72MHz
		 */
		rcc_set_pll_multiplication_factor(RCC_CFGR_PLLMUL_PLL_CLK_MUL9);

		/* Select HSE as PLL source. */
		rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_CLK);

		/*
		 * External frequency undivided before entering PLL
		 * (only valid/needed for HSE).
		 */
		rcc_set_pllxtpre(RCC_CFGR_PLLXTPRE_HSE_CLK);

		/* Enable PLL oscillator and wait for it to stabilize. */
		rcc_osc_on(PLL);
		rcc_wait_for_osc_ready(PLL);

		/* Select PLL as SYSCLK source. */
		rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_PLLCLK);

		/* Set the peripheral clock frequencies used */
		rcc_ppre1_frequency = 36000000;
		rcc_ppre2_frequency = 72000000;
	}
#endif


    scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP16_NOSUB); // 16 priority groups each with no sub-priority.
    /* STM32F103 uses four priority bits which can be sub-divided into priority groups (preemptive interrupt priority)
    and sub-priority (pending interrupt priority) within group if required.
    If interrupt priorities are equal, the lowest irq number takes precedence.
    */

	/* Setup SYSTICK Timer */
	/* 72MHz / 8 => 9000000 counts per second */
	systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8); // older lib
	//  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // newer lib

    /* 9000000 / 9000 = 1000 overflows per second - one interrupt every 1ms */
    systick_set_reload(9000-1); // Reload value is n-1 for a free running timer.
    nvic_set_priority(NVIC_SYSTICK_IRQ, 0 ); // Priority group 0 (Highest).
    systick_interrupt_enable();
    msecs = 0;
    systick_counter_enable();

    // Start other peripherals.
    // Port A
    rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
    		      GPIO_CNF_OUTPUT_PUSHPULL,
    		      // SPARE3 | SPARE2 | SPARE1 |
    				LED_G | LED_R | LED_Y | LED_O );

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
        		      GPIO_CNF_INPUT_PULL_UPDOWN,
        		      BIND_SWITCH );
    gpio_set(GPIOA, BIND_SWITCH); // Pull up.

    // Port B
    rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);

    gpio_set_mode(GPIOB,
    			GPIO_MODE_OUTPUT_50_MHZ,
     		    GPIO_CNF_OUTPUT_PUSHPULL,
     		    HEARTBEAT_CPU );

    gpio_set_mode(GPIOB,
    			GPIO_MODE_INPUT,
				GPIO_CNF_INPUT_PULL_UPDOWN,
        		ALT_CONF | CH_ORD | RF_EN);

    gpio_set(GPIOB,
    		ALT_CONF | CH_ORD | RF_EN); // Pull up.

    gpio_set_mode(GPIOB,
    		GPIO_MODE_INPUT,
        	GPIO_CNF_INPUT_FLOAT,
        	// SCIO |
        	BOOT1 );

    // *** Start protocol callback TIM4 ***
    /* Enable TIM4 clock. */
    rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);
    timer_enable_1us_oc1(TIM4);
    CLOCK_StopTimer();
    nvic_set_priority(NVIC_TIM4_IRQ, ( 3 << 4) ); // Priority group 3.
    nvic_enable_irq(NVIC_TIM4_IRQ);
    timer_generate_event(TIM4, TIM_EGR_UG);
    timer_enable_counter(TIM4);


    // *** Start SPI(2) ***
    /* Enable SPI2 */
    rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_SPI2EN);
    spi_reset(SPI2);

    /* SCK, MOSI */
    gpio_set_mode(GPIO_BANK_SPI2_SCK, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_SPI2_SCK | GPIO_SPI2_MOSI);
    /* MISO */
    gpio_set_mode(GPIO_BANK_SPI2_MISO, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT, GPIO_SPI2_MISO);

    /* SPARE1 */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, SPARE1 );

    gpio_clear(GPIOA, SPARE1);

    /* CS */
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_PUSHPULL, CS );

    gpio_set(GPIOB, CS); // Set CS inactive.

    spi_init_master(SPI2,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_16,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);

    spi_enable_ss_output(SPI2);
    spi_set_unidirectional_mode(SPI2);
    spi_enable(SPI2);

    usart_three_setup(); // Debug USART.
}


void timer_enable_1us_oc1(u32 timer_peripheral)
{
    timer_reset(timer_peripheral); // Reset TIM peripheral.
    timer_set_prescaler(timer_peripheral, 72 - 1); // Timer updates each microsecond.
    timer_continuous_mode(timer_peripheral); // Run continuously ...
    timer_set_period(timer_peripheral, 0xffff);  // ... as a 16 bit counter.
    timer_set_repetition_counter(timer_peripheral, 0); // Update event after 0 repetitions.
    timer_set_clock_division(timer_peripheral, TIM_CR1_CKD_CK_INT); // tDTS = tCLK_INT. Dead time and sampling frequency.
    timer_set_alignment(timer_peripheral, TIM_CR1_CMS_EDGE ); // Counts up or down dependent on ...
    timer_direction_up(timer_peripheral); // ... this.

	// Configure channel 1.
    timer_disable_oc_preload(timer_peripheral, TIM_OC1);
    timer_set_oc_mode(timer_peripheral, TIM_OC1, TIM_OCM_FROZEN);

 	timer_clear_flag(timer_peripheral, TIM_SR_CC1IF);
	timer_enable_irq(timer_peripheral, TIM_DIER_CC1IE);
}
