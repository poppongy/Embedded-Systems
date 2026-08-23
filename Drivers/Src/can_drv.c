/*
 * can_drv.c
 *
 *  Created on: May 12, 2026
 *      Author: Prince
 */
#include "can_drv.h"
#include "timer_drv.h"
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
	if (frame == NULL)
	{
	    return CAN_STATUS_NULL_POINTER;
	}

	if (frame->dlc > 8U)
	{
	    return CAN_STATUS_INVALID_DLC;
	}

	uint32_t start_time1 = timer_get_ticks();
	//wait for an empty mailbox
	while(!(CAN1->TSR & CAN_TSR_TME0))
	{
		if((timer_get_ticks() - start_time1) >= CAN_TX_TIMEOUT_MS)
		{
			return CAN_STATUS_TIMEOUT;
		}
	}

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

	uint32_t start_time2 = timer_get_ticks();

	//wait until mailbox 0 request completes
	while (!(CAN1->TSR & CAN_TSR_RQCP0))
	{
		if((timer_get_ticks() - start_time2) >=CAN_TX_TIMEOUT_MS)
		{
			CAN1->TSR |= CAN_TSR_ABRQ0;
			return CAN_STATUS_TIMEOUT;
		}
	}

	can_status_t result = CAN_STATUS_TX_FAILED;

	if (CAN1->TSR & CAN_TSR_TXOK0)
	{
	    result = CAN_STATUS_OK;
	}

	/* clear request complete flag */
	CAN1->TSR |= CAN_TSR_RQCP0;

	return result;
}

can_status_t can1_read(can_frame_t *frame)
{
	if(frame == NULL)
	{
		return CAN_STATUS_NULL_POINTER;
	}
	//verify pending messages on FIFO 0
	if (!(CAN1->RF0R & CAN_RF0R_FMP0)) //non-blocking like while(!(CAN1->RF0R & CAN_RF0R_FMP0)); //Wait for pending message
	{
		return CAN_STATUS_NO_MESSAGE;
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
	return CAN_STATUS_OK;
}
