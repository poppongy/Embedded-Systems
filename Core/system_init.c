/*
 * system_init.c
 *
 *  Created on: Feb 15, 2026
 *      Author: Prince
 *
 *      This file handles: clocks, low-level MCU setup
 */

#include "system_init.h"


void system_init(void)
{
    // later: clocks, systick timebase, etc.
	//SystemInit();

	//initialize timer at startup
	//TIM2_Timebase_Init(); ---Blocking
	timer2_init();// --non-blocking with interrupts
}
