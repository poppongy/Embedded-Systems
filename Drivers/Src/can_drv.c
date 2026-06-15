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

	CAN1->MCR |= CAN_MCR_INRQ;
	// 1. Exit sleep first
	CAN1->MCR &= ~CAN_MCR_SLEEP;
	//while(CAN1->MSR & CAN_MSR_SLAK); // wait for sleep exit

	// 2. Request init mode
	//CAN1->MCR |= CAN_MCR_INRQ;
	while(!(CAN1->MSR & CAN_MSR_INAK)); // wait for init confirm
	while(CAN1->MSR & CAN_MSR_SLAK);

	//bit timing configuration
	CAN1->BTR = (0x0UL << 24) |  // SJW  = 1 TQ  (0+1)
	            (0x1UL << 20) |  // TS2  = 2 TQ  (1+1)
	            (0x4UL << 16) |  // TS1  = 5 TQ  (4+1)
	            (0x3UL << 0)  |  // BRP  = 4     (3+1)
	            (0x1UL << 30);   // LBKM loopback mode
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

	volatile uint32_t d = 10000;
	while(d--);

	uart2_write_string("CAN init complete\r\n");
}



void can1_tx(void)
{
	// disable RX interrupt during transmission
	    CAN1->IER &= ~CAN_IER_FMPIE0;

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
	 CAN1->IER |= CAN_IER_FMPIE0;
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

	//log over UART
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
		 // disable further RX interrupts until frame is processed
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
