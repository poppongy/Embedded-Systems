//entering into initialization mode 
Software sets this bit to request the CAN hardware to enter initialization mode. Once
software has set the INRQ bit, the CAN hardware waits until the current CAN activity
(transmission or reception) is completed before entering the initialization mode. Hardware
signals this event by setting the INAK bit in the CAN_MSR register.

CAN1->MCR — master control register:

Set INRQ bit to request init mode
Poll MSR (master status regiser) register — wait until INAK bit is set confirming init mode active
Clear SLEEP bit — CAN wakes from sleep on reset, must clear it

//corrections 
Setting INRQ is a request — you simply set it and wait. Clearing it first is harmless but unnecessary.
Issue 3 — MSR is a READ-ONLY status register
This is the most critical mistake. You cannot write to CAN1->MSR. The hardware sets and clears INAK and SLAK automatically 
in response to your MCR writes. Trying to write to MSR does nothing — or worse, undefined behavior.
c// WRONG — MSR is read-only
CAN1->MSR &= ~(CAN_MSR_INAK);
CAN1->MSR |= CAN_MSR_INAK;
CAN1->MSR &= ~(CAN_MSR_SLAK);
What you actually need to do with MSR is poll it — read it and wait until the hardware confirms init mode is active

MCR = command register → you write requests
MSR = status register → hardware reports back

Key Lesson
In embedded peripherals always ask: is this register writable or read-only? Status registers (SR, MSR, ISR) are almost 
always read-only — they reflect hardware state. Control registers (CR, MCR, DIER) are writable — they command the hardware. 
The reference manual marks each register field as r, w, or rw. Check it before writing.

//importance of the polling -- waitsThe INAK polling loop is not optional housekeeping — it is a hardware synchronization barrier. 
Without it your code races ahead and writes to BTR while the peripheral is still in normal mode, those writes are silently discarded,
and CAN never actually gets configured. 
The bug that follows is extremely difficult to trace because the code looks correct — the real problem is timing, not logic.

//wrong write
overwriting CAN1->BTR every line.
So only this final line survives:
CAN1->BTR = 0x1UL << 30;
	//bit timing configuration in order of bit flag
	CAN1->BTR = 0x3UL << 0;   //BRP[9:0]: Baud rate prescaler
	CAN1->BTR = 0x4UL << 16;  //TS1[3:0]: Time segment 1
	CAN1->BTR = 0x1UL << 20;  //TS2[2:0]: Time segment 2
	CAN1->BTR = 0x0UL << 24;  //SJW[1:0]: Resynchronization jump width
	//Setting Loop back mode(debug)
	CAN1->BTR = 0x1UL << 30;
	
writing mask mode configuration explicitly is redundant
When you set both FR1 and FR2 to zero, mask mode is already the default after reset and it does not need to be
 explicitly configured in your case.
 
Mailbox configuration
	//mailbox 0 must be clear before writing anything
while(CAN1->TSR & CAN_TSR_TME0);
TME0 means Transmit Mailbox 0 Empty. When this bit is SET it means the mailbox IS empty and ready to use.
Your loop says "keep waiting WHILE TME0 is set" — meaning you wait while the mailbox is available and proceed 
when it is busy. That is the opposite of what you want.

//clearing TX mailbox 0 identifier register
	CAN1->sTxMailBox[0].TIR = 0; This sets every bit to zero including bit 1 (RTR).

	//Set the frame ID and frame type in TIR
	CAN1->sTxMailBox[0].TIR |= (0x123UL << 21);
	//CAN1->sTxMailBox[0].TIR &= ~(0x1 << 1); Clearing it again is redundant.
	
solidifying understanding of DLC and data bytes  (Because the second line overwrites the first).
	//data payload configuration into TDLR (data bytes)
	CAN1->sTxMailBox[0].TDLR = 0xA5UL << 8;
	CAN1->sTxMailBox[0].TDLR = 0x85UL;
	
Note on exiting initialization mode without a transceiver
	//Wait for hardware to confirm normal mode active
 	//while (CAN1->MSR & CAN_MSR_INAK);//polling wait until INAK clears
	//without a transceiver  connected PD0 (CAN_RX) is floating — not a clean recessive signal —
	//so the hardware never sees those 11 bits and INAK never clears.
	In loopback mode without a transceiver you will never see 11 recessive bits so that loop will hang forever. 
	A fixed delay is sufficient for your testing purposes.
	
IMPORTANT NOTE
By following this sequence, we were able to finally resolve the issue with looping freeze coming from 
//while(CAN1->MSR & CAN_MSR_SLAK); // wait for sleep exit -- send the request for initialization first
1. Enable CAN1 clock
2. Request initialization mode: MCR.INRQ = 1
3. Clear sleep: MCR.SLEEP = 0
4. Wait for INAK = 1
5. Wait for SLAK = 0

Another thing was, to call the uart message in can_init, I had to remove from state_machine_init into state_machine_run. That 
is how I got it to print "CAN init complete". A better way is to introduce a delay to be able to observe since init runs too fast
for logic analyzer to log anything.

//release FIFO0.. could also mean clearing the interrupt pending flap
	CAN1->RF0R |= CAN_RF0R_RFOM0;
For CAN RX FIFO0:
FMP0 = read-only pending message count
It clears only when you release the FIFO message after reading it.So there is no need to explicitly clear pending
 registers as in EXTI
That means the interrupt source is cleared by:
read FIFO0 message
then release FIFO0 using RFOM0

BUG: the code stops after pressing the button. ISR fires but the code is stopping in the can1_tx() api at the function which 
calls uart_write() under condition that the transmission was successful.
 what could potentially be causing this?
 button press
→ state_machine calls can1_tx()
→ CAN transmits in loopback
→ CAN RX interrupt fires immediately
→ CPU jumps to CAN ISR
→ ISR sets CAN event flag
→ returns to can1_tx()
→ can1_tx() calls uart2_write_string("CAN TX OK")

ISR getting called multiple times
FMP0 is the interrupt source. As long as FMP0 is non-zero the interrupt keeps firing. You need to prevent re-entry 
without releasing the frame data.
The Fix — Disable the interrupt inside the ISR
Then in your state machine after can1_rx() processes and releases the mailbox

quick question. does using the same mail box for heartbeat not cause any buffer/ mail box conflict since can1_tx()
 uses the same mail box? no Both functions begin with:

while (!(CAN1->TSR & CAN_TSR_TME0));

which waits until Mailbox 0 is empty before writing anything. So only one frame can occupy Mailbox 0 at a time.
The only time you'd have a conflict is if:
Two contexts (e.g., ISR and main loop) tried to write to the mailbox simultaneously, or
You removed the TME0 check.

NOTICE: CAN_RX() fires with heartbeat transmissions. can1_rx() is absolutely being called—not directly by the scheduler, 
but indirectly because the heartbeat transmission generates a receive interrupt.

After connecting all wiring to both nodes and transceiver,refactored the can code base for both producer/transmitter and receiver nodes. 
For the producer node, we kept things the same except adding some macros to enhance readability. For the receiver, all heartbeat 
initialization and transmission were disabled. Note: refactored file: state_machine.c. initially logic analyzer showed producer was transmitting
but receiver was not able to process and log payload to uart. to debug had to introduce breakpoint. at first breakpoint in ISR was getting skipped
execution never reached the ISR but the FIFO registers showed frame queued up. it was not until introduced breakwatch to monitor anything 
that changes CAN1->IER. this was because i realized that after setting CAN1->IER during initialization, it would be cleared after initialization. 
through breakwatch, discovered the ISR is actually the one clearing the CAN1->IER indicating it was working. about the same time, frames started getting
logged to uart correctly. Additionally, the arrangement of these in if(can1_get_rx_event()) was causing issues so had to clear event flag before
re-enabling  CAN1->IER
		 can1_rx();
		 can1_clear_rx_event();

		 //re-enable RX interrupt now that mailbox is released
		 CAN1->IER |= CAN_IER_FMPIE0;
BEFORE REFACTORING TO IMPLEMENT WEEK6 LOGIC 
state_machine.c
#include "state_machine.h"
#include "gpio_drv.h"
#include "timer_drv.h"
#include "exti_drv.h"
#include "uart_drv.h"
#include "can_drv.h"
#include <stdio.h>



static soft_timer_t led_timer;
static soft_timer_t heartbeat_timer;

void state_machine_init(void)
{
	gpio_init(GPIO_PORT_D, PIN_12, GPIO_MODE_OUTPUT, NO_PUPD, AF0);
	gpio_init(GPIO_PORT_D, PIN_14, GPIO_MODE_OUTPUT, NO_PUPD, AF0);
	gpio_init(GPIO_PORT_B, PIN_12, GPIO_MODE_INPUT,  PULL_UP, AF0);
	exti_init();
	uart2_init();
	timer_start(&led_timer, PERIOD);

	#if NODE_ROLE == NODE_PRODUCER
		timer_start(&heartbeat_timer, HB_PERIOD);
	#endif

	can1_init();
}

void state_machine_run(void)
{
	///////////cooperative scheduler primitive/////////////

	//LED TOGGLING
	if (timer_expired_periodic(&led_timer))
	 {
		gpio_toggle(GPIO_PORT_D, PIN_14);
		uart2_write_string("Timer event\r\n");
	 }

	//HEARTBEAT
	#if NODE_ROLE == NODE_PRODUCER
		if (timer_expired_periodic(&heartbeat_timer))
		 {
			 can1_heartbeat();
		 }
	#endif

	//BUTTON EVENT
	 if (exti_get_button_event())
	 {
		 for(volatile int i = 0; i < 50000; i++); // debounce delay
		 gpio_toggle(GPIO_PORT_D, PIN_12);
		 uart2_write_string("Button pressed\r\n");
		 can1_tx();
		 exti_clear_button_event();
	 }

	 //CAN INTERRUPT -- EXTRACTING FRAME IN CAN RECEIVER
	 if(can1_get_rx_event())
	 {
		 can1_rx();
		 can1_clear_rx_event();

		 //re-enable RX interrupt now that mailbox is released
		 CAN1->IER |= CAN_IER_FMPIE0;
	 }

}
can_drv.c
/*
 * can_drv.c
 *
 *  Created on: May 12, 2026
 *      Author: Prince
 */
#include "can_drv.h"
#include "stm32f4xx.h"
#include <stdio.h>

static volatile uint8_t intrpt_flag = 0;


void can1_init(void)
{
	//initializing GPIO pins PD0 and PD1
	gpio_init(GPIO_PORT_D, PIN_0, GPIO_MODE_AF, NO_PUPD, AF9);//CAN1_RX
	gpio_init(GPIO_PORT_D, PIN_1, GPIO_MODE_AF, NO_PUPD, AF9);//CAN1_TX

	//enable CAN clock
	RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

/////////////ONLY WORKED FOR LOOP BACK MODE/////////////////////////////////////
	/*CAN1->MCR |= CAN_MCR_INRQ;
	// 1. Exit sleep first
	CAN1->MCR &= ~CAN_MCR_SLEEP;
	//while(CAN1->MSR & CAN_MSR_SLAK); // wait for sleep exit

	// 2. Request init mode
	//CAN1->MCR |= CAN_MCR_INRQ;
	while(!(CAN1->MSR & CAN_MSR_INAK)); // wait for init confirm
	while(CAN1->MSR & CAN_MSR_SLAK);*/
////////////////////////////////////////////////////////////////////////////////////
	CAN1->MCR &= ~CAN_MCR_SLEEP;
	while (CAN1->MSR & CAN_MSR_SLAK);

	CAN1->MCR |= CAN_MCR_INRQ;
	while (!(CAN1->MSR & CAN_MSR_INAK));

	//bit timing configuration -- NORMAL MODE
	CAN1->BTR = (0x0UL << 24) |  // SJW  = 1 TQ  (0+1)
	            (0x1UL << 20) |  // TS2  = 2 TQ  (1+1)
	            (0x4UL << 16) |  // TS1  = 5 TQ  (4+1)
	            (0x3UL << 0);// |  // BRP  = 4     (3+1)
	            //(0x1UL << 30);   // LBKM loopback mode
				//(0x1UL << 31); //SILM -- silent mode added to debug program

	//CAN FILTER CONFIGURATION
	CAN1->FMR |= CAN_FMR_FINIT;//entering filter init mode

	//Deactivate bank 0 via FA1R before configuring it
	CAN1->FA1R &= ~CAN_FA1R_FACT0;

	//CAN1->FM1R &= ~CAN_FM1R_FBM0;  // mask mode — redundant but acceptable

	//Set 32-bit scale
	CAN1->FS1R |= CAN_FS1R_FSC0;

	//Set FR1 and FR2 to zero — accept all
	CAN1->sFilterRegister[0].FR1 = 0;
	CAN1->sFilterRegister[0].FR2 = 0;

	//Assigning to FIFO0
	CAN1->FFA1R &= ~ CAN_FFA1R_FFA0;

	//Activate bank 0
	CAN1->FA1R |= CAN_FA1R_FACT0;

	//Exit filter init mode
	CAN1->FMR &= ~CAN_FMR_FINIT;

	//Enable FIFO 0 message pending interrupt
	CAN1->IER |= CAN_IER_FMPIE0;

	//Enable NVIC to send the interrupt request IRQ
	NVIC_EnableIRQ(CAN1_RX0_IRQn);

	//Exit can initialization mode
	CAN1->MCR &= ~CAN_MCR_INRQ;

	//wait to enter into normal mode
	while (CAN1->MSR & CAN_MSR_INAK);

	//volatile uint32_t d = 10000;
	//while(d--);

	//uart2_write_string("CAN init complete\r\n");

}



void can1_tx(void)
{
	// disable RX interrupt during transmission
	//CAN1->IER &= ~CAN_IER_FMPIE0;

	//mailbox 0 must be clear before writing anything.
	//When this bit is SET it means the mailbox IS empty and ready to use
	while(!(CAN1->TSR & CAN_TSR_TME0));

	//clearing TX mailbox 0 identifier register
	CAN1->sTxMailBox[0].TIR = 0; //This sets every bit to zero including bit 1 (RTR=  0, IDE = 0).

	//Set the frame ID
	CAN1->sTxMailBox[0].TIR |= (0x123UL << 21);//set 11-bit ID

	//Setting the data length code
	CAN1->sTxMailBox[0].TDTR = 0x3UL;

	//data payload configuration into TDLR (data bytes)
	CAN1->sTxMailBox[0].TDLR = (0x33UL << 16) |
	                           (0x22UL << 8)  |
	                           (0x11UL);

	//Request transmission
	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;

	/* wait until mailbox 0 request completes */
	while (!(CAN1->TSR & CAN_TSR_RQCP0));

	if (CAN1->TSR & CAN_TSR_TXOK0)
	{
	    uart2_write_string("CAN TX OK\r\n");
	}
	else
	{
	    uart2_write_string("CAN TX FAILED\r\n");
	}

	/* clear request complete flag */
	CAN1->TSR |= CAN_TSR_RQCP0;

	// re-enable RX interrupt after TX complete
	 //CAN1->IER |= CAN_IER_FMPIE0;
}


void can1_rx(void)
{
	//verify pending messages on FIFO 0
	while(!(CAN1->RF0R & CAN_RF0R_FMP0)); //Wait for pending message

	//Extracting the message id
	uint32_t ext_id = (uint32_t) ((CAN1->sFIFOMailBox[0].RIR >> 21) & 0x7FFUL);

	//Extracting the data length code
	uint32_t ext_DLC = (uint32_t) ((CAN1->sFIFOMailBox[0].RDTR) & 0xFUL);

	//Extracting the data
	uint32_t ext_data2 = (uint32_t) ((CAN1->sFIFOMailBox[0].RDLR >> 16) & 0xFFUL);

	//Extracting the data
	uint32_t ext_data1 = (uint32_t) ((CAN1->sFIFOMailBox[0].RDLR >> 8) & 0xFFUL);

	//Extracting the data
	uint32_t ext_data0 = (uint32_t) ((CAN1->sFIFOMailBox[0].RDLR) & 0xFFUL);

	//log over UART //"ID:%#lx DLC:%lu DATA:0x%02lx 0x%02lx 0x%02lx\r\n",
	char payload[100];
	snprintf(payload, sizeof(payload),
	         "ID:%#lx DLC:%lu DATA:0x%02lx 0x%02lx 0x%02lx\r\n",
	         ext_id, ext_DLC, ext_data0, ext_data1, ext_data2);

	uart2_write_string(payload);

	//release FIFO0
	CAN1->RF0R |= CAN_RF0R_RFOM0;
}

void CAN1_RX0_IRQHandler(void)
{
	//check for pending messages in FIFO 0
	if(CAN1->RF0R & CAN_RF0R_FMP0)
	{
		//disable further RX interrupts until frame is processed
		CAN1->IER &= ~CAN_IER_FMPIE0;
		//set event flag
		intrpt_flag = 1;
	}
}

uint8_t can1_get_rx_event(void)
{
	return intrpt_flag;
}

void can1_clear_rx_event(void)
{
	intrpt_flag = 0;
}

void can1_heartbeat(void)
{
	//mailbox 0 must be clear before writing anything.
	//When this bit is SET it means the mailbox IS empty and ready to use
	while(!(CAN1->TSR & CAN_TSR_TME0));

	//clearing TX mailbox 0 identifier register
	CAN1->sTxMailBox[0].TIR = 0; //This sets every bit to zero including bit 1 (RTR=  0, IDE = 0).

	//Set the frame ID
	CAN1->sTxMailBox[0].TIR |= (0x100UL << 21);//set 11-bit ID

	//Setting the data length code
	CAN1->sTxMailBox[0].TDTR = 0x1UL;

	//data payload configuration into TDLR (data bytes)
	CAN1->sTxMailBox[0].TDLR = (0xAAUL);

	//INTRODUCING SEQUENCE NUMBER FOR DIAGNOSIS
	//CAN1->sTxMailBox[0].TDLR = heartbeat_counter++;
	//Request transmission
	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;

	//wait until mailbox 0 request completes
	while (!(CAN1->TSR & CAN_TSR_RQCP0));

	if (CAN1->TSR & CAN_TSR_TXOK0)
	{
		uart2_write_string("HB TX OK\r\n");
	}
	else
	{
		uart2_write_string("HB TX FAILED\r\n");
	}
	/* clear request complete flag */
	CAN1->TSR |= CAN_TSR_RQCP0;
}

NOTES ON VERIFICATION OF CAN_HEARTBEAT ON NODE_B
uint8_t previous_status = node_state.heartbeat_status;

	node_state.heartbeat_status = NODE_A_ALIVE;
	node_state.last_heartbeat_timestamp = timer_get_ticks();
	
//only works first time you start. previous_status changes to 1 after BECAUSE
 node_state.heartbeat_status = NODE_A_ALIVE; Thus causing the conditions to 
 evaluate to false
 
	if (previous_status == NODE_A_UNKNOWN) 
	{
		uart2_write_string("Heartbeat detected\r\n");
	}
	else if (previous_status == NODE_A_OFFLINE)
	{
		uart2_write_string("Heartbeat recovered\r\n");
	}
	//REPEATS ALL THE TIME -- NOT DESIRABLE
	else
	{
		uart2_write_string("Heartbeat Alive\r\n");
	}

final phase of project -- MAKING CODE MORE SCALABLE TO INDUSTRY STANDARD
/*
 * can_drv.c
 *
 *  Created on: May 12, 2026
 *      Author: Prince
 */
#include "can_drv.h"
#include "stm32f4xx.h"
#include <stdio.h>

static volatile uint8_t intrpt_flag = 0;


void can1_init(void)
{
	//initializing GPIO pins PD0 and PD1
	gpio_init(GPIO_PORT_D, PIN_0, GPIO_MODE_AF, NO_PUPD, AF9);//CAN1_RX
	gpio_init(GPIO_PORT_D, PIN_1, GPIO_MODE_AF, NO_PUPD, AF9);//CAN1_TX

	//enable CAN clock
	RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

	CAN1->MCR &= ~CAN_MCR_SLEEP;
	while (CAN1->MSR & CAN_MSR_SLAK);

	CAN1->MCR |= CAN_MCR_INRQ;
	while (!(CAN1->MSR & CAN_MSR_INAK));

	//bit timing configuration -- NORMAL MODE
	CAN1->BTR = (0x0UL << 24) |  // SJW  = 1 TQ  (0+1)
	            (0x1UL << 20) |  // TS2  = 2 TQ  (1+1)
	            (0x4UL << 16) |  // TS1  = 5 TQ  (4+1)
	            (0x3UL << 0);// |  // BRP  = 4     (3+1)

	//CAN FILTER CONFIGURATION
	CAN1->FMR |= CAN_FMR_FINIT;//entering filter init mode

	//Deactivate bank 0 via FA1R before configuring it
	CAN1->FA1R &= ~CAN_FA1R_FACT0;

	//Set 32-bit scale
	CAN1->FS1R |= CAN_FS1R_FSC0;

	//Set FR1 and FR2 to zero — accept all
	CAN1->sFilterRegister[0].FR1 = 0;
	CAN1->sFilterRegister[0].FR2 = 0;

	//Assigning to FIFO0
	CAN1->FFA1R &= ~ CAN_FFA1R_FFA0;

	//Activate bank 0
	CAN1->FA1R |= CAN_FA1R_FACT0;

	//Exit filter init mode
	CAN1->FMR &= ~CAN_FMR_FINIT;

	//Enable FIFO 0 message pending interrupt
	CAN1->IER |= CAN_IER_FMPIE0;

	//Enable NVIC to send the interrupt request IRQ
	NVIC_EnableIRQ(CAN1_RX0_IRQn);

	//Exit can initialization mode
	CAN1->MCR &= ~CAN_MCR_INRQ;

	//wait to enter into normal mode
	while (CAN1->MSR & CAN_MSR_INAK);
}

void CAN1_RX0_IRQHandler(void)
{
	//check for pending messages in FIFO 0
	if(CAN1->RF0R & CAN_RF0R_FMP0)
	{
		//disable further RX interrupts until frame is processed
		CAN1->IER &= ~CAN_IER_FMPIE0;
		//set event flag
		intrpt_flag = 1;
	}
}

uint8_t can1_get_rx_event(void)
{
	return intrpt_flag;
}

void can1_clear_rx_event(void)
{
	intrpt_flag = 0;
}


can_status_t can1_send(const can_frame_t *frame)
{
	//validate frame pointer and DLC
/*	if(frame == NULL || frame->dlc > 8)
	{
		return 0;
	}*/
	if (frame == NULL)
	{
	    return CAN_STATUS_NULL_POINTER;
	}

	if (frame->dlc > 8U)
	{
	    return CAN_STATUS_INVALID_DLC;
	}
	//wait for an empty mailbox
	while(!(CAN1->TSR & CAN_TSR_TME0));

	//clearing TX mailbox 0 identifier register
	CAN1->sTxMailBox[0].TIR = 0;

	CAN1->sTxMailBox[0].TIR |= ((frame->id & 0x7FFUL) << 21);

	CAN1->sTxMailBox[0].TDTR = frame->dlc & 0xFUL;

	//clearing data registers first before writing to them
	CAN1->sTxMailBox[0].TDLR = 0;
	CAN1->sTxMailBox[0].TDHR = 0;

	CAN1->sTxMailBox[0].TDLR =
	    ((uint32_t)frame->data[0] << 0)  |
	    ((uint32_t)frame->data[1] << 8)  |
	    ((uint32_t)frame->data[2] << 16) |
	    ((uint32_t)frame->data[3] << 24);

	CAN1->sTxMailBox[0].TDHR =
	    ((uint32_t)frame->data[4] << 0)  |
	    ((uint32_t)frame->data[5] << 8)  |
	    ((uint32_t)frame->data[6] << 16) |
	    ((uint32_t)frame->data[7] << 24);

	//Request transmission
	CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;

	//wait until mailbox 0 request completes
	while (!(CAN1->TSR & CAN_TSR_RQCP0));

	uint8_t result = 0;

	if (CAN1->TSR & CAN_TSR_TXOK0)
	{
	    result = 1;
	}

	/* clear request complete flag */
	CAN1->TSR |= CAN_TSR_RQCP0;

	return result;
}

uint8_t can1_read(can_frame_t *frame)
{
	if(frame == NULL)
	{
		return 0;
	}
	//verify pending messages on FIFO 0
	if (!(CAN1->RF0R & CAN_RF0R_FMP0)) //non-blocking like while(!(CAN1->RF0R & CAN_RF0R_FMP0)); //Wait for pending message
	    {
	        return 0;
	    }
	//Extracting the message id
	frame->id = (uint32_t) ((CAN1->sFIFOMailBox[0].RIR >> 21) & 0x7FFUL);

	//Extracting the data length code
	frame->dlc = (uint8_t) ((CAN1->sFIFOMailBox[0].RDTR) & 0xFUL);

	//Extracting the data
	frame->data[0] = (uint8_t)((CAN1->sFIFOMailBox[0].RDLR >> 0)  & 0xFFUL);
	frame->data[1] = (uint8_t)((CAN1->sFIFOMailBox[0].RDLR >> 8)  & 0xFFUL);
	frame->data[2] = (uint8_t)((CAN1->sFIFOMailBox[0].RDLR >> 16) & 0xFFUL);
	frame->data[3] = (uint8_t)((CAN1->sFIFOMailBox[0].RDLR >> 24) & 0xFFUL);

	frame->data[4] = (uint8_t)((CAN1->sFIFOMailBox[0].RDHR >> 0)  & 0xFFUL);
	frame->data[5] = (uint8_t)((CAN1->sFIFOMailBox[0].RDHR >> 8)  & 0xFFUL);
	frame->data[6] = (uint8_t)((CAN1->sFIFOMailBox[0].RDHR >> 16) & 0xFFUL);
	frame->data[7] = (uint8_t)((CAN1->sFIFOMailBox[0].RDHR >> 24) & 0xFFUL);

	//release FIFO0
	CAN1->RF0R |= CAN_RF0R_RFOM0;
	return 1;
}


#include "state_machine.h"
#include "gpio_drv.h"
#include "timer_drv.h"
#include "exti_drv.h"
#include "uart_drv.h"
#include "can_drv.h"
#include "can_protocol.h"
#include <stdio.h>

#define LED_PERIOD_MS                 		1000U
#define HEARTBEAT_TX_PERIOD_MS         		100U
#define HEARTBEAT_CHECK_PERIOD_MS       	150U
#define HEARTBEAT_TIMEOUT_MS           		500U

//Heartbeat status
#define NODE_A_UNKNOWN   	  				0U
#define NODE_A_ALIVE          				1U
#define NODE_A_OFFLINE        				2U

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

		if (previous_status == NODE_A_UNKNOWN)// only works first time you start. previous status changes to 1 after
		{
			uart2_write_string("Heartbeat detected\r\n");
		}
		else if (previous_status == NODE_A_OFFLINE)
		{
			uart2_write_string("Heartbeat recovered\r\n");
		}
		/*else
		{
			uart2_write_string("Heartbeat Alive\r\n");
		}*/

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
			 if(can1_send(&heartbeat_frame))
			 {
				 uart2_write_string("HB TX OK\r\n");
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
			 if(can1_read(&read_frame))
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
			 for(volatile int i = 0; i < 50000; i++); // debounce delay
			 gpio_toggle(GPIO_PORT_D, PIN_12);

			 //implementing can frame transmission
			 if(can1_send(&button_frame))
			 {
				 uart2_write_string("CAN TX OK\r\n");
			 }
			 else
			 {
				 uart2_write_string("CAN TX FAILED\r\n");
			 }
			 exti_clear_button_event();
		 }
	#endif

	//LED TOGGLING
	if (timer_expired_periodic(&led_timer))
	 {
		gpio_toggle(GPIO_PORT_D, PIN_14);
		uart2_write_string("Timer event\r\n");
	 }

}
//CAN PERIPHERAL BEHAVIOR
//wait until mailbox 0 request completes
	while (!(CAN1->TSR & CAN_TSR_RQCP0))
	{
		if((timer_get_ticks() - start_time2) >=CAN_TX_TIMEOUT_MS)
		{
			CAN1->TSR |= CAN_TSR_ABRQ0; // ABORT ALL TRANSMISSION 
			BEFORE RETURNING
			return CAN_STATUS_TIMEOUT;
		}
	}
Once you set the **`TXRQ`** bit, the CAN hardware begins transmitting
the frame independently of the CPU. Your code then waits for the 
transmission to complete. If your timeout expires and you simply 
return from `can1_send()`, the CAN controller may still be trying to
transmit that frame in the background because you never told it to stop.
Setting **`ABRQ0`** requests the hardware to abort mailbox 0's transmission
before returning `CAN_STATUS_TIMEOUT`, leaving the peripheral in a clean state.
This abort is only needed after `TXRQ` has been set; if you're still waiting
for an empty mailbox before requesting transmission, there's nothing to abort yet.

//TIMER DEBOUNCE DELAY
//DEBOUNCE DELAY WITH THE SCHEDULER
	 uint32_t time_now = timer_get_ticks();
	 if(time_now - last_button_timestamp >= BTN_DEBOUNCE_MS)@A
	 {
		 last_button_timestamp = timer_get_ticks();
	A. Ignore any button events that occur within 50 milliseconds of the previous valid press."
The scheduler continues running during those 50 ms—it doesn't stop. If the user presses the button 
again after 60 ms, it 
is accepted; if the contacts bounce 5 ms later, it is ignored.
