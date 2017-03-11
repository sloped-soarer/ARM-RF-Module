/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Deviation is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Deviation.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.h"
#include "protocol/interface.h"
#include "timer.h"
#include "config/model.h"
#include "config/tx.h"

#ifdef UNIMOD
#include "unimod.h"
#endif // UNIMOD

void Init();
void EventLoop();
volatile u8 priority_ready;

struct Model Model;
const char * const RADIO_TX_POWER_VAL[TXPOWER_LAST] =
     { "100uW", "300uW", "1mW", "3mW", "10mW", "30mW", "100mW", "150mW" };

int main() {
#ifdef UNIMOD
	uni_mod();
#endif // UNIMOD

#ifndef UNIMOD

	Init();

    //CONFIG_LoadTx();
    SPI_ProtoInit();
    // CONFIG_ReadModel(CONFIG_GetCurrentModel());
    //MIXER_Init();
    //PAGE_Init();
    CLOCK_StartWatchdog();

    priority_ready = 0;
    CLOCK_SetMsecCallback(LOW_PRIORITY, LOW_PRIORITY_MSEC);
    CLOCK_SetMsecCallback(MEDIUM_PRIORITY, MEDIUM_PRIORITY_MSEC);

    // We need to wait until we've actually measured the ADC before proceeding
    while(! (priority_ready & (1 << LOW_PRIORITY)))
        ;

    //Only do this after we've initialized all channel data so the saftey works
    PROTOCOL_InitModules();


    while(1) {
        if(priority_ready) {
            EventLoop();
        }
        //PWR_Sleep();  //This does not appear to have any impact on power
    }
#endif // UNIMOD
}


void Init() {
    PWR_Init();
    CLOCK_Init();
    UART_Initialize();
    CHAN_Init();
    PPMin_TIM_Init();
}



void medium_priority_cb()
{
    //MIXER_CalcChannels();
}

void EventLoop()
{
    CLOCK_ResetWatchdog();
    unsigned int time;

    priority_ready &= ~(1 << MEDIUM_PRIORITY);

   // BUTTON_Handler();
    if (priority_ready & (1 << LOW_PRIORITY)) {
        priority_ready  &= ~(1 << LOW_PRIORITY);
       // PAGE_Event();
        PROTOCOL_CheckDialogs();
        TIMER_Update();

    }
}
