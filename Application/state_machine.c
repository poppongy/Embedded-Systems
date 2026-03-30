/*
 * state_machine.c
 *
 *  Created on: Feb 15, 2026
 *      Author: Prince
 */
#include "state_machine.h"
#include "gpio_drv.h"
#include "timer_drv.h"

/*static void delay(volatile uint32_t count)
{
    while (count--) { __asm__("NOP"); }
    //Repeat N times:
    	// wait one CPU cycle
}*/

//static uint32_t last_toggle = 0;
static soft_timer_t led_timer;

void state_machine_init(void)
{
	// later: set initial state
	//gpio_init_output(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN);
	gpio_init_output(GPIO_PORT_D, PIN_12);
	timer_start(&led_timer, 1000);
}

/*BLOCKING DELAY
 * void state_machine_run(void)
{
   // later: transition + actions
	gpio_toggle(GPIO_PORT_D, PIN_12);

	delay_ms(500); wait loop - blocking
	//delay(500000);

}*/

/*USING INTERRUPT
 * this logic uses time comparisons not loops making it non-blocking
 * The CPU: checks elapsed time
 *toggles if ready
 *immediately continues executing the rest of the program
 *There is no pause, no stall, and no polling loop holding execution hostage.
 */
void state_machine_run(void)
{
	//“How much time has passed since the last toggle?”
	//with the condition, you asking has 1s elapsed since last toggle?
    /*if ((timer_get_ticks() - last_toggle) >= 1000)
    {
        //Reset the reference point
    	last_toggle = timer_get_ticks();

         //Toggle PD12
        GPIOD->ODR ^= GPIO_ODR_OD12;
    }*/

	//cooperative scheduler primitive
	 if (timer_expired(&led_timer))
	    {
	        timer_reset(&led_timer);
	        GPIOD->ODR ^= GPIO_ODR_OD12;
	    }
}

