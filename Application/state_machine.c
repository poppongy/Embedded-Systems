#include "state_machine.h"
#include "gpio_drv.h"
#include "timer_drv.h"
#include "exti_drv.h"
#include "uart_drv.h"
#include "can_drv.h"
#include <stdio.h>

static soft_timer_t led_timer;
void state_machine_init(void)
{
	gpio_init(GPIO_PORT_D, PIN_12, GPIO_MODE_OUTPUT, NO_PUPD, AF0);
	gpio_init(GPIO_PORT_D, PIN_14, GPIO_MODE_OUTPUT, NO_PUPD, AF0);
	gpio_init(GPIO_PORT_B, PIN_12, GPIO_MODE_INPUT,  PULL_UP, AF0);
	exti_init();
	uart2_init();
	timer_start(&led_timer, PERIOD);
	can1_init();
}

void state_machine_run(void)
{
	//cooperative scheduler primitive
	if (timer_expired(&led_timer))
	 {
		timer_reset(&led_timer);
		gpio_toggle(GPIO_PORT_D, PIN_14);
		uart2_write_string("Timer event\r\n");
	 }

	 if (exti_get_button_event())
	 {
		 for(volatile int i = 0; i < 50000; i++); // debounce delay
		 gpio_toggle(GPIO_PORT_D, PIN_12);
		 uart2_write_string("Button pressed\r\n");
		 can1_tx();
		 exti_clear_button_event();
	 }
	 if(can1_get_rx_event())
	 {
		 can1_rx();
		 can1_clear_rx_event();
		 // re-enable RX interrupt now that mailbox is released
		 CAN1->IER |= CAN_IER_FMPIE0;
	 }
}

