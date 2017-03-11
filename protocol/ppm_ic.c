

#ifdef MODULAR
  //Allows the linker to properly relocate
  #define PPM_IC_Cmds PROTO_Cmds
  #pragma long_calls
#endif

#include "common.h"
#include "interface.h"
#include "mixer.h"
#include "config/model.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>



#ifdef MODULAR
  #pragma long_calls_off
  extern unsigned _data_loadaddr;
  const unsigned long protocol_type = (unsigned long)&_data_loadaddr;
#endif


#define MICRO_SEC_CONVERT(uS) (2*(uS))  //  Timer resolution is 0.5uS so multiply by 2 to make code more readable.
extern volatile u8 g_sync_count;

#if 0
MODULE_CALLTYPE
static u16 ppm_ic_cb()
{
return 0;
}
#endif

static void initialize()
{
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_TIM1EN);
	// rcc_periph_clock_enable(RCC_TIM1); // new libopencm3

    timer_reset(TIM1);
    timer_set_prescaler(TIM1, 36-1); // 72MHz / 36 = 2MHz : 1 count = 0.5 uS
    timer_continuous_mode(TIM1); // Run continuously ...
    timer_set_period(TIM1, 0xffff);  // ... as a 16 bit counter.
    timer_set_repetition_counter(TIM1, 0); // Update event after 0 repetitions.
    timer_set_clock_division(TIM1, TIM_CR1_CKD_CK_INT); // tDTS = tCLK_INT. Dead time and sampling frequency.
    timer_set_alignment(TIM1, TIM_CR1_CMS_EDGE ); // Counts up or down dependent on ...
    timer_direction_up(TIM1); // ... this.

	// Configure TIM1_CH2 as input.
	gpio_set_mode(GPIO_BANK_TIM1_CH2,
				GPIO_MODE_INPUT,
		        GPIO_CNF_INPUT_FLOAT,
		        GPIO_TIM1_CH2);

	// Configure channel 2
	timer_ic_set_input     (TIM1, TIM_IC2, TIM_IC_IN_TI2);
	timer_ic_set_filter    (TIM1, TIM_IC2, TIM_IC_OFF); // just trying something ...
	timer_ic_set_polarity  (TIM1, TIM_IC2, TIM_IC_FALLING); // Rising or falling : choose the cleanest edge.
	timer_ic_set_prescaler (TIM1, TIM_IC2, TIM_IC_PSC_OFF); // No input capture prescaler
	timer_ic_enable        (TIM1, TIM_IC2);
	timer_clear_flag       (TIM1, TIM_SR_CC2IF);
	timer_enable_irq       (TIM1, TIM_DIER_CC2IE);

	nvic_set_priority(NVIC_TIM1_CC_IRQ, (2 << 4) ); // Priority 2.
	nvic_enable_irq(NVIC_TIM1_CC_IRQ);
	timer_generate_event(TIM1, TIM_EGR_UG);
	timer_enable_counter(TIM1);
}


void tim1_cc_isr(void)
{
static u16 ccr_previous = 0;
static u8 channel_count = 0;
static u8 need_to_sync = 1;

u16 ccr_current;
u16 ccr_diff;

	if(timer_get_flag(TIM1, TIM_SR_CC2IF))
	{
//	capture = TIM_CCR2(TIM1); // use this or ...
//  capture = TIM1_CCR2 & 0xffff; // ... this to read capture register.
//  Read overcapture flag ?
//	timer_clear_flag(TIM1, TIM_SR_CC2IF); // Clears pending interrupt as does reading capture register.


	ccr_current = TIM1_CCR2 & 0xffff;
	if(ccr_current >= ccr_previous) ccr_diff = ccr_current - ccr_previous;
	else ccr_diff = ((TIM1_ARR & 0xffff) - ccr_previous) + ccr_current + 1 ; // Method using ARR (max counter value).

	ccr_previous = ccr_current;

		if(ccr_diff > MICRO_SEC_CONVERT(2300)) // > 2.3ms pulse seen as frame sync.
		{
			need_to_sync =0;
			g_sync_count ++;
			channel_count =0;
		}
		else if(ccr_diff < MICRO_SEC_CONVERT(700))
		{
			// < 0.7ms pulse seen as glitch - do nothing.
		}
		else if(! need_to_sync) // Pulse within limits and we don't need to sync.
		{
			if(channel_count < MAX_PPM_IN_CHANNELS)
			{
				// Constrain values.
				if(ccr_diff > MICRO_SEC_CONVERT(1500 + DELTA_PPM_IN)) // unimod rick Removed hard coded value 2050.
					ccr_diff = MICRO_SEC_CONVERT(1500 + DELTA_PPM_IN);
				else if(ccr_diff < MICRO_SEC_CONVERT(1500 - DELTA_PPM_IN))// unimod rick Removed hard coded value 950.
					ccr_diff = MICRO_SEC_CONVERT(1500 - DELTA_PPM_IN);

			Channels[channel_count] = (ccr_diff - (MICRO_SEC_CONVERT(1500))) * 10; // Subtract 1.5 ms centre offset ...
			// ... and multiply by 10 to get roughly the same scaling as Deviation.
			// See mixer.h for ratiometric madness.
			channel_count ++; // unimod rick Added due to cut 'n' paste omission.
			}
			else need_to_sync = 1; // More channel pulses than we can handle ... need to sync.
		}
	}
}


const void * PPM_IC_Cmds(enum ProtoCmds cmd)
{
    switch(cmd) {
        case PROTOCMD_INIT:  initialize(); return 0;
        case PROTOCMD_DEINIT:
        	timer_ic_set_input(TIM1, TIM_IC2, TIM_IC_OUT);// uni-mod-rick Added.
        	timer_ic_disable(TIM1, TIM_IC2);// uni-mod-rick Added.
        	return (void *) 1L;

        case PROTOCMD_RESET:
            timer_reset(TIM1);// unimod rick Added.
            return (void *) 1L;

        case PROTOCMD_CHECK_AUTOBIND: return (void *)0L; // Never Autobind
        case PROTOCMD_BIND:  initialize(); return 0;
        case PROTOCMD_NUMCHAN: return (void *) 8L;
        case PROTOCMD_DEFAULT_NUMCHAN: return (void *)8L;
        case PROTOCMD_GETOPTIONS:
//            if (Model.proto_opts[CENTER_PW] == 0) {
//              Model.proto_opts[CENTER_PW] = 1100;
//                Model.proto_opts[DELTA_PW] = 400;
//                Model.proto_opts[NOTCH_PW] = 400;
//                Model.proto_opts[PERIOD_PW] = 22500;
//            }
//            return ppm_opts;
        	return 0;
        case PROTOCMD_TELEMETRYSTATE: return (void *)(long)PROTO_TELEM_UNSUPPORTED;
        default: break;
    }
    return 0;
}

