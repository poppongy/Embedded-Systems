#include "state_machine.h"
#include "gpio_drv.h"
#include "timer_drv.h"
#include "exti_drv.h"
#include "uart_drv.h"
#include "can_drv.h"
#include "can_protocol.h"
#include <stdio.h>

/////////////////PRODUCER IMPLEMENTATION////////////////////////////////////////////////////////////
#if NODE_ROLE == NODE_PRODUCER
	static const can_frame_t heartbeat_frame =
	{
		.id = CAN_ID_HEARTBEAT,
		.dlc = CAN_HEARTBEAT_DLC,
		.data[0] = HEARTBEAT_ALIVE
	};
	static const can_frame_t button_frame =
	{
		.id = CAN_ID_BUTTON_EVENT,
		.dlc = CAN_BUTTON_EVENT_DLC,
		.data[0] = BTNFRAME_DATA0,
		.data[1] = BTNFRAME_DATA1,
		.data[2] = BTNFRAME_DATA2
	};
	static soft_timer_t heartbeat_timer;

	static uint32_t last_button_timestamp = 0;
#endif
/////////////////////////////////////////////////////////////////////////////////

//**********************RECEIVER CONFIGURATION**********************************
#if	NODE_ROLE == NODE_RECEIVER
	typedef struct
	{
	    uint8_t heartbeat_status;
	    uint32_t last_heartbeat_timestamp;
	} node_state_t;

	static node_state_t node_state =
	{
	    .heartbeat_status = NODE_A_UNKNOWN
	};

	static can_frame_t read_frame;
	static soft_timer_t heartbeat_timer_timeout;

	//FRAME PROCESSING APIs
	static void processHeartbeat(const can_frame_t *read_frame)
	{
		if ((read_frame->dlc < 1U) ||(read_frame->data[0] != HEARTBEAT_ALIVE))
			{
				return;
			}

		uint8_t previous_status = node_state.heartbeat_status;

		node_state.heartbeat_status = NODE_A_ALIVE;
		node_state.last_heartbeat_timestamp = timer_get_ticks();

		// only works first time you start. previous status changes to 1 or alive after
		if (previous_status == NODE_A_UNKNOWN)
		{
			uart2_write_string("Heartbeat detected\r\n");
		}
		else if (previous_status == NODE_A_OFFLINE)
		{
			uart2_write_string("Heartbeat recovered\r\n");
		}

	}
	static void processButtonEvent(const can_frame_t *read_frame)
	{
		if(read_frame->dlc != CAN_BUTTON_EVENT_DLC)
		{
			return;
		}
		gpio_toggle(GPIO_PORT_D, PIN_13);

		char payload[50];
			snprintf(payload, sizeof(payload),
					 "ID:%#lx DLC:%lu DATA:0x%02lx 0x%02lx 0x%02lx\r\n",
					 (unsigned long)read_frame->id,
					 (unsigned long)read_frame->dlc,
					 (unsigned long)read_frame->data[0],
					 (unsigned long)read_frame->data[1],
					 (unsigned long)read_frame->data[2]);

		uart2_write_string(payload);
	}

	static void checkHeartbeatTimeout(void)
	{
		uint32_t elapsed_time = timer_get_ticks() - node_state.last_heartbeat_timestamp;

		if(node_state.heartbeat_status == NODE_A_UNKNOWN)
		{
			return;
		}

		if(elapsed_time >=HEARTBEAT_TIMEOUT_MS && node_state.heartbeat_status == NODE_A_ALIVE)
		{
			node_state.heartbeat_status = NODE_A_OFFLINE;
			uart2_write_string("Heartbeat is offline\r\n");
		}
	}
#endif
//***********************************************************************************
static can_status_t can_status;
static soft_timer_t led_timer;

void state_machine_init(void)
{
	gpio_init(GPIO_PORT_D, PIN_12, GPIO_MODE_OUTPUT, NO_PUPD, AF0);
	gpio_init(GPIO_PORT_D, PIN_13, GPIO_MODE_OUTPUT, NO_PUPD, AF0);
	gpio_init(GPIO_PORT_D, PIN_14, GPIO_MODE_OUTPUT, NO_PUPD, AF0);
	gpio_init(GPIO_PORT_B, PIN_12, GPIO_MODE_INPUT,  PULL_UP, AF0);
	exti_init();
	uart2_init();
	timer_start(&led_timer, LED_PERIOD_MS);

	#if NODE_ROLE == NODE_PRODUCER
		timer_start(&heartbeat_timer, HEARTBEAT_TX_PERIOD_MS);
	#endif

	#if NODE_ROLE == NODE_RECEIVER
		timer_start(&heartbeat_timer_timeout, HEARTBEAT_CHECK_PERIOD_MS);
	#endif

	can1_init();
}

void state_machine_run(void)
{
	///////////cooperative scheduler primitive/////////////
	//HEARTBEAT
	#if NODE_ROLE == NODE_PRODUCER
		if (timer_expired_periodic(&heartbeat_timer))
		 {
			can_status = can1_send(&heartbeat_frame);
			if (can_status == CAN_STATUS_OK)
			{
			    uart2_write_string("HB TX OK\r\n");
			}
			else if (can_status == CAN_STATUS_TIMEOUT)
			{
			    uart2_write_string("HB TX TIMEOUT\r\n");
			}
			else
			{
			    uart2_write_string("HB TX FAILED\r\n");
			}
		 }
	#endif

	//CAN INTERRUPT -- EXTRACTING FRAME IN CAN RECEIVER
	#if NODE_ROLE == NODE_RECEIVER
		 if(can1_get_rx_event())
		 {
			 can_status = can1_read(&read_frame);
			 if(can_status == CAN_STATUS_OK)
			 {
				 switch(read_frame.id)
				 {
					 case CAN_ID_HEARTBEAT:
						 processHeartbeat(&read_frame);
						 break;
					 case CAN_ID_BUTTON_EVENT:
						 processButtonEvent(&read_frame);
						 break;
					 default:
						 break;
				 }
			 }
			 can1_clear_rx_event();

			 //re-enable RX interrupt now that mailbox is released
			 CAN1->IER |= CAN_IER_FMPIE0;
		 }
	#endif

	#if NODE_ROLE == NODE_RECEIVER
		if (timer_expired_periodic(&heartbeat_timer_timeout))
		{
			checkHeartbeatTimeout();
		}
	#endif


	//BUTTON EVENT
	#if NODE_ROLE == NODE_PRODUCER
		 if (exti_get_button_event())
		 {
			 //DEBOUNCE DELAY WITH THE SCHEDULER
			 uint32_t time_now = timer_get_ticks();
			 if(time_now - last_button_timestamp >= BTN_DEBOUNCE_MS)
			 {
				 last_button_timestamp = timer_get_ticks();
				 gpio_toggle(GPIO_PORT_D, PIN_12);

				 //implementing can frame transmission
				 can_status = can1_send(&button_frame);
				 if (can_status == CAN_STATUS_OK)
				 {
					 uart2_write_string("CAN TX OK\r\n");
				 }
				 else if (can_status == CAN_STATUS_TIMEOUT)
				 {
					 uart2_write_string("CAN TX TIMEOUT\r\n");
				 }
				 else
				 {
					 uart2_write_string("CAN TX FAILED\r\n");
				 }

				 exti_clear_button_event();
			 }
			 }

	#endif

	//LED TOGGLING
	if (timer_expired_periodic(&led_timer))
	 {
		gpio_toggle(GPIO_PORT_D, PIN_14);
		uart2_write_string("Timer event\r\n");
	 }

}

